/*
 * =====================================================================================
 *
 *       Filename:  CTransaction.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 11:04:33 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "WalletService/CTransaction.h"
#include "WalletService/CTxIndex.h"
#include "WalletService/CTxOutPoint.h"
#include "WalletService/CTxInPoint.h"
#include "WalletService/WalletServ.h"
#include "BlockEngine/CBlockIndex.h"
#include "DAO/DaoServ.h"
#include "CommonBase/ProtocSerialize.h"
#include "ProtocSrc/Transaction.pb.h"
using namespace Enze;
//////////////////////////////////////////////////////////////////////////////
//
// CTransaction
//

// 判断这边交易能不能被接受，如果能接受将对应的交易放入全局变量中mapTransactions，mapNextTx中
bool CTransaction::AcceptTransaction(bool fCheckInputs, bool* pfMissingInputs)
{
    if (pfMissingInputs)
        *pfMissingInputs = false;

	// 币基交易仅仅在块中有效，币基交易不能做为一个单独的交易
    // Coinbase is only valid in a block, not as a loose transaction
    if (IsCoinBase())
        return error("AcceptTransaction() : coinbase as individual tx");

    if (!CheckTransaction())
        return error("AcceptTransaction() : CheckTransaction failed");

	// 判断当前交易是否我们已经接收到过了
    // Do we already have it?
    uint256 hash = GetHash();
    map<uint256, CTransaction>& mapTransactions = WalletServ::getInstance()->mapTransactions;
    map<COutPoint, CInPoint>& mapNextTx = WalletServ::getInstance()->mapNextTx;
    if (mapTransactions.count(hash)) // 判断内存对象map中是否已经存在
        return false;
    if (fCheckInputs)
        if (DaoServ::getInstance()->ContainsTx(hash)) // 判断交易db中是否已经存在
            return false;

	// 判断当前交易对象是否和内存中的交易对象列表冲突
    // Check for conflicts with in-memory transactions
    CTransaction* ptxOld = NULL;
    for (int i = 0; i < m_vTxIn.size(); i++)
    {
        COutPoint outpoint = m_vTxIn[i].m_cPrevOut;
		// 根据当前交易对应的输入交易，获得对应输入交易对应的输出交易
        if (mapNextTx.count(outpoint))
        {
            // Allow replacing with a newer version of the same transaction
			// i ==0 为coinbase，也就是coinbase可以替换
            if (i != 0)
                return false;
			// 相对于当前交易更老的交易
            ptxOld = mapNextTx[outpoint].m_pcTrans;
            if (!IsNewerThan(*ptxOld)) // 判断是否比原来交易更新，通过nSequences判断
                return false;
            for (int i = 0; i < m_vTxIn.size(); i++)
            {
                COutPoint outpoint = m_vTxIn[i].m_cPrevOut;
				// 当前交易的输入在内存对象mapNextTx对应的输出如果都存在，且都指向原来老的交易，则接收此交易
                if (!mapNextTx.count(outpoint) || mapNextTx[outpoint].m_pcTrans != ptxOld)
                    return false;
            }
            break;
        }
    }

	// 对前交易进行校验和设置前交易对应的输出为花费标记
    // Check against previous transactions
    map<uint256, CTxIndex> mapUnused;
    int64 nFees = 0;
    if (fCheckInputs && !ConnectInputs(mapUnused, CDiskTxPos(1,1,1), 0, nFees, false, false))
    {
        if (pfMissingInputs)
            *pfMissingInputs = true;
        return error("AcceptTransaction() : ConnectInputs failed %s", hash.ToString().substr(0,6).c_str());
    }

	// 将当前交易存储在内存，如果老的交易存在，则从内存中将对应的交易移除
    // Store transaction in memory
    {
        if (ptxOld)
        {
            printf("mapTransaction.erase(%s) replacing with new version\n", ptxOld->GetHash().ToString().c_str());
            mapTransactions.erase(ptxOld->GetHash());
        }
		// 将当前交易存储到内存对象中
        AddToMemoryPool();
    }

	// 如果老的交易存在，则从钱包中将老的交易移除
    ///// are we sure this is ok when loading transactions or restoring block txes
    // If updated, erase old tx from wallet
    if (ptxOld)
		// 将交易从钱包映射对象mapWallet中移除，同时将交易从CWalletDB中移除
        WalletServ::getInstance()->EraseFromWallet(ptxOld->GetHash());

    printf("AcceptTransaction(): accepted %s\n", hash.ToString().substr(0,6).c_str());
    return true;
}

// 将当前交易增加到内存池mapTransactions,mapNextTx中，并且更新交易更新的次数
bool CTransaction::AddToMemoryPool()
{
    // Add to memory pool without checking anything.  Don't call this directly,
    // call AcceptTransaction to properly check the transaction first.
    map<uint256, CTransaction>& mapTransactions = WalletServ::getInstance()->mapTransactions;
    map<COutPoint, CInPoint>& mapNextTx = WalletServ::getInstance()->mapNextTx;
    {
        uint256 hash = GetHash();
        mapTransactions[hash] = *this; // 将当前交易放入到内存对象mapTransactions中
		// 更新或者设置对应的mapNextTx 是的交易对应的输入的输出对应的是本交易
        for (int i = 0; i < m_vTxIn.size(); i++)
            mapNextTx[m_vTxIn[i].m_cPrevOut] = CInPoint(&mapTransactions[hash], i);

		// 记录交易被更新的次数
        WalletServ::getInstance()->nTransactionsUpdated++;
    }
    return true;
}

// 将当前交易从内存对象mapTransactions，mapNextTx中移除，并且增加交易对应的更新次数
bool CTransaction::RemoveFromMemoryPool()
{
    // Remove transaction from memory pool
    map<uint256, CTransaction>& mapTransactions = WalletServ::getInstance()->mapTransactions;
    map<COutPoint, CInPoint>& mapNextTx = WalletServ::getInstance()->mapNextTx;
    {
        foreach(const CTxIn& txin, m_vTxIn)
            mapNextTx.erase(txin.m_cPrevOut);
        mapTransactions.erase(GetHash());
        WalletServ::getInstance()->nTransactionsUpdated++;
    }
    return true;
}

// 断开连接输入，就是释放交易对应的输入的占用：即是释放交易输入对应的交易索引的标记占用
bool CTransaction::DisconnectInputs()
{
	// 放弃或者让出前一个交易对应的花费标记指针
    // Relinquish previous transactions' spent pointers
    if (!IsCoinBase()) // 币基
    {
        foreach(const CTxIn& txin, m_vTxIn)
        {
            COutPoint prevout = txin.m_cPrevOut;

            // Get prev txindex from disk
            CTxIndex txindex;
			// 从数据库中读取对应的交易的索引
            if (!DaoServ::getInstance()->ReadTxIndex(prevout.m_u256Hash, txindex))
                return error("DisconnectInputs() : ReadTxIndex failed");

            if (prevout.m_nIndex >= txindex.m_vSpent.size())
                return error("DisconnectInputs() : prevout.m_nIndex out of range");

            // Mark outpoint as not spent
            txindex.m_vSpent[prevout.m_nIndex].SetNull();

            // Write back
            DaoServ::getInstance()->UpdateTxIndex(prevout.m_u256Hash, txindex);
        }
    }

	// 将当前交易从交易索引表中移除
    // Remove transaction from index
    if (!DaoServ::getInstance()->EraseTxIndex(*this))
        return error("DisconnectInputs() : EraseTxPos failed");

    return true;
}

// 交易输入链接，将对应的交易输入占用对应的交易输入的花费标记
bool CTransaction::ConnectInputs(map<uint256, CTxIndex>& mapTestPool, const CDiskTxPos& posThisTx, int m_nCurHeight, int64& nFees, bool fBlock, bool fMiner, int64 nMinFee)
{
	// 占用前一个交易对应的花费指针
    // Take over previous transactions' spent pointers
    if (!IsCoinBase())
    {
        map<uint256, CTransaction>& mapTransactions = WalletServ::getInstance()->mapTransactions;
        const int nBestHeight = BlockEngine::getInstance()->getBestHeight();
        CBlockIndex* pindexBest = BlockEngine::getInstance()->getBestIndex();
        int64 nValueIn = 0;
        for (int i = 0; i < m_vTxIn.size(); i++)
        {
            COutPoint prevout = m_vTxIn[i].m_cPrevOut;

            // Read txindex
            CTxIndex txindex;
            bool fFound = true;
            if (fMiner && mapTestPool.count(prevout.m_u256Hash))
            {
                // Get txindex from current proposed changes
                txindex = mapTestPool[prevout.m_u256Hash];
            }
            else
            {
                // Read txindex from txdb
                fFound = DaoServ::getInstance()->ReadTxIndex(prevout.m_u256Hash, txindex);
            }
            if (!fFound && (fBlock || fMiner))
                return fMiner ? false : error("ConnectInputs() : %s prev tx %s index entry not found", GetHash().ToString().substr(0,6).c_str(),  prevout.m_u256Hash.ToString().substr(0,6).c_str());

            // Read txPrev
            CTransaction txPrev;
            if (!fFound || txindex.m_cDiskPos == CDiskTxPos(1,1,1))
            {
                // Get prev tx from single transactions in memory
  //              CRITICAL_BLOCK(cs_mapTransactions)
                {
                    if (!mapTransactions.count(prevout.m_u256Hash))
                        return error("ConnectInputs() : %s mapTransactions prev not found %s", GetHash().ToString().substr(0,6).c_str(),  prevout.m_u256Hash.ToString().substr(0,6).c_str());
                    txPrev = mapTransactions[prevout.m_u256Hash];
                }
                if (!fFound)
                    txindex.m_vSpent.resize(txPrev.m_vTxOut.size());
            }
            else
            {
                // Get prev tx from disk
                if (!txPrev.ReadFromDisk(txindex.m_cDiskPos))
                    return error("ConnectInputs() : %s ReadFromDisk prev tx %s failed", GetHash().ToString().substr(0,6).c_str(),  prevout.m_u256Hash.ToString().substr(0,6).c_str());
            }

            if (prevout.m_nIndex >= txPrev.m_vTxOut.size() || prevout.m_nIndex >= txindex.m_vSpent.size())
                return error("ConnectInputs() : %s prevout.m_nIndex out of range %d %d %d", GetHash().ToString().substr(0,6).c_str(), prevout.m_nIndex, txPrev.m_vTxOut.size(), txindex.m_vSpent.size());

            // If prev is coinbase, check that it's matured
            if (txPrev.IsCoinBase())
                for (CBlockIndex* pindex = pindexBest; pindex && nBestHeight - pindex->m_nCurHeight < COINBASE_MATURITY-1; pindex = pindex->m_pPrevBlkIndex)
                    if (pindex->m_nBlockPos == txindex.m_cDiskPos.m_nBlockPos && pindex->m_nFile == txindex.m_cDiskPos.m_nFile)
                        return error("ConnectInputs() : tried to spend coinbase at depth %d", nBestHeight - pindex->m_nCurHeight);

            // Verify signature
            if (!VerifySignature(txPrev, *this, i))
                return error("ConnectInputs() : %s VerifySignature failed", GetHash().ToString().substr(0,6).c_str());

            // Check for conflicts
            if (!txindex.m_vSpent[prevout.m_nIndex].IsNull())
                return fMiner ? false : error("ConnectInputs() : %s prev tx already used at %s", GetHash().ToString().substr(0,6).c_str(), txindex.m_vSpent[prevout.m_nIndex].ToString().c_str());

			// 标记前一个交易对应的交易索引对应的花费标记
            // Mark outpoints as spent
            txindex.m_vSpent[prevout.m_nIndex] = posThisTx;

            // Write back
            if (fBlock)
                DaoServ::getInstance()->UpdateTxIndex(prevout.m_u256Hash, txindex);
            else if (fMiner)
                mapTestPool[prevout.m_u256Hash] = txindex;

            nValueIn += txPrev.m_vTxOut[prevout.m_nIndex].m_nValue;
        }

        // Tally transaction fees
        int64 nTxFee = nValueIn - GetValueOut();
        if (nTxFee < 0)
            return error("ConnectInputs() : %s nTxFee < 0", GetHash().ToString().substr(0,6).c_str());
        if (nTxFee < nMinFee)
            return false;
        nFees += nTxFee;
    }

    if (fBlock)
    {
        // Add transaction to disk index
        if (!DaoServ::getInstance()->AddTxIndex(*this, posThisTx, m_nCurHeight))
            return error("ConnectInputs() : AddTxPos failed");
    }
    else if (fMiner)
    {
		// 如果是矿工，将对应的交易放入对应的交易测试池中
        // Add transaction to test pool
        mapTestPool[GetHash()] = CTxIndex(CDiskTxPos(1,1,1), m_vTxOut.size());
    }

    return true;
}

// 客户端连接输入，对交易本身进行验证
bool CTransaction::ClientConnectInputs()
{
    if (IsCoinBase())
        return false;

	// 占用前一个交易对应的花费标记
    // Take over previous transactions' spent pointers
    map<uint256, CTransaction>& mapTransactions = WalletServ::getInstance()->mapTransactions;
    {
        int64 nValueIn = 0;
        for (int i = 0; i < m_vTxIn.size(); i++)
        {
            // Get prev tx from single transactions in memory
            COutPoint prevout = m_vTxIn[i].m_cPrevOut;
            if (!mapTransactions.count(prevout.m_u256Hash))
                return false;
            CTransaction& txPrev = mapTransactions[prevout.m_u256Hash];

            if (prevout.m_nIndex >= txPrev.m_vTxOut.size())
                return false;

            // Verify signature
            if (!VerifySignature(txPrev, *this, i))
                return error("ConnectInputs() : VerifySignature failed");

            ///// this is redundant with the mapNextTx stuff, not sure which I want to get rid of
            ///// this has to go away now that posNext is gone
            // // Check for conflicts
            // if (!txPrev.vout[prevout.m_nIndex].posNext.IsNull())
            //     return error("ConnectInputs() : prev tx already used");
            //
            // // Flag outpoints as used
            // txPrev.vout[prevout.m_nIndex].posNext = posThisTx;

            nValueIn += txPrev.m_vTxOut[prevout.m_nIndex].m_nValue;
        }
        if (GetValueOut() > nValueIn)
            return false;
    }

    return true;
}


void CTransaction::SetNull()
{
    m_nCurVersion = 1;
    m_vTxIn.clear();
    m_vTxOut.clear();
    m_nLockTime = 0;
}

bool CTransaction::IsNull() const
{
    return (m_vTxIn.empty() && m_vTxOut.empty());
}

uint256 CTransaction::GetHash() const
{
    Transaction cProtoc;
    SeriaTransaction(*this, cProtoc);
    string strSerial;
    cProtoc.SerializePartialToString(&strSerial);
    return Hash(strSerial.begin(), strSerial.end());
   // return SerializeHash(*this);
}

// 判断是否是最终的交易
bool CTransaction::IsFinal() const
{
    // 如果锁定时间等于0或者锁定时间小于主链的长度，则说明是最终的交易
    const int& nBestHeight = BlockEngine::getInstance()->getBestHeight();
    if (m_nLockTime == 0 || m_nLockTime < nBestHeight)
        return true;
    foreach(const CTxIn& txin, m_vTxIn)
        if (!txin.IsFinal())
            return false;
    return true;
}
// 对比相同的交易哪一个更新点：对于相同输入的交易判断哪一个更新
bool CTransaction::IsNewerThan(const CTransaction& old) const
{
    if (m_vTxIn.size() != old.m_vTxIn.size())
        return false;
    for (int i = 0; i < m_vTxIn.size(); i++)
        if (m_vTxIn[i].m_cPrevOut != old.m_vTxIn[i].m_cPrevOut)
            return false;

    bool fNewer = false;
    unsigned int nLowest = UINT_MAX;
    for (int i = 0; i < m_vTxIn.size(); i++)
    {
        if (m_vTxIn[i].m_uSequence != old.m_vTxIn[i].m_uSequence)
        {
            if (m_vTxIn[i].m_uSequence <= nLowest)
            {
                fNewer = false;
                nLowest = m_vTxIn[i].m_uSequence;
            }
            if (old.m_vTxIn[i].m_uSequence < nLowest)
            {
                fNewer = true;
                nLowest = old.m_vTxIn[i].m_uSequence;
            }
        }
    }
    return fNewer;
}

// 判断当前交易是否是币基交易：币基交易的特点是交易输入大小为1，但是对应的输入的输出为空
bool CTransaction::IsCoinBase() const
{
    return (m_vTxIn.size() == 1 && m_vTxIn[0].m_cPrevOut.IsNull());
}
/* 对这边交易进行检查：
(1)交易对应的输入或者输出列表都不能为空
(2)交易对应的输出金额不能小于0
(3)如果是币基交易，则对应的输入只能为1，且对应的输入签名大小不能大于100
(4)如果是非币基交易，则对应的交易输入的输出不能为空
*/
bool CTransaction::CheckTransaction() const
{
    // Basic checks that don't depend on any context
    if (m_vTxIn.empty() || m_vTxOut.empty())
        return error("CTransaction::CheckTransaction() : m_vTxIn or m_vTxOut empty");

    // Check for negative values
    foreach(const CTxOut& txout, m_vTxOut)
        if (txout.m_nValue < 0)
            return error("CTransaction::CheckTransaction() : txout.nValue negative");

    if (IsCoinBase())
    {
        if (m_vTxIn[0].m_cScriptSig.size() < 2 || m_vTxIn[0].m_cScriptSig.size() > 100)
            return error("CTransaction::CheckTransaction() : coinbase script size");
    }
    else
    {
        foreach(const CTxIn& txin, m_vTxIn)
            if (txin.m_cPrevOut.IsNull())
                return error("CTransaction::CheckTransaction() : preout is null");
    }

    return true;
}

// 判断当前的交易是否包含节点本身的交易（在输出列表中）
bool CTransaction::IsMine() const
{
    foreach(const CTxOut& txout, m_vTxOut)
        if (txout.IsMine())
            return true;
    return false;
}

// 获得当前交易总的输入：借方
int64 CTransaction::GetDebit() const
{
    int64 nDebit = 0;
    foreach(const CTxIn& txin, m_vTxIn)
        nDebit += txin.GetDebit();
    return nDebit;
}

// 获得当前交易总的贷方金额：属于节点自身的
int64 CTransaction::GetCredit() const
{
    int64 nCredit = 0;
    foreach(const CTxOut& txout, m_vTxOut)
        nCredit += txout.GetCredit();
    return nCredit;
}

// 获取交易对应所有输出金额之和
int64 CTransaction::GetValueOut() const
{
    int64 nValueOut = 0;
    foreach(const CTxOut& txout, m_vTxOut)
    {
        if (txout.m_nValue < 0)
            throw runtime_error("CTransaction::GetValueOut() : negative value");
        nValueOut += txout.m_nValue;
    }
    return nValueOut;
}
// 获取交易对应的最小交易费：大小小于10000字节则对应的最小交易费为0，否则为按照大小进行计算交易费
// Transaction fee requirements, mainly only needed for flood control
// Under 10K (about 80 inputs) is free for first 100 transactions
// Base rate is 0.01 per KB
int64 CTransaction::GetMinFee(bool fDiscount) const
{
    return 0;
    // Base fee is 1 cent per kilobyte
    printf("error %s ---%d\n", __FILE__, __LINE__);
    //unsigned int nBytes = ::GetSerializeSize(*this, SER_NETWORK);
    unsigned int nBytes = 0;//::GetSerializeSize(*this, SER_NETWORK);
    int64 nMinFee = (1 + (int64)nBytes / 1000) * CENT;

    // First 100 transactions in a block are free
    if (fDiscount && nBytes < 10000)
        nMinFee = 0;

    // To limit dust spam, require a 0.01 fee if any output is less than 0.01
    if (nMinFee < CENT)
        foreach(const CTxOut& txout, m_vTxOut)
            if (txout.m_nValue < CENT)
                nMinFee = CENT;

    return nMinFee;
}

// 从硬盘中进行读取
bool CTransaction::ReadFromDisk(const CDiskTxPos& pos, FILE** pfileRet)
{
    CAutoFile filein = BlockEngine::getInstance()->OpenBlockFile(pos.m_nFile, 0, pfileRet ? "rb+" : "rb");
    if (!filein)
        return error("CTransaction::ReadFromDisk() : OpenBlockFile failed");

    // Read transaction
    if (fseek(filein, pos.m_nTxPos, SEEK_SET) != 0)
        return error("CTransaction::ReadFromDisk() : fseek failed");
    filein >> *this;

    // Return file pointer
    if (pfileRet)
    {
        if (fseek(filein, pos.m_nTxPos, SEEK_SET) != 0)
            return error("CTransaction::ReadFromDisk() : second fseek failed");
        *pfileRet = filein.release();
    }
    return true;
}




string CTransaction::ToString() const
{
    string str;
    str += strprintf("CTransaction(hash=%s, ver=%d, m_vTxIn.size=%d, m_vTxOut.size=%d, nLockTime=%d)\n",
        GetHash().ToString().substr(0,6).c_str(),
        m_nCurVersion,
        m_vTxIn.size(),
        m_vTxOut.size(),
        m_nLockTime);
    for (int i = 0; i < m_vTxIn.size(); i++)
        str += "    " + m_vTxIn[i].ToString() + "\n";
    for (int i = 0; i < m_vTxOut.size(); i++)
        str += "    " + m_vTxOut[i].ToString() + "\n";
    return str;
}

void CTransaction::print() const
{
    printf("%s", ToString().c_str());
}

bool Enze::operator!=(const CTransaction& a, const CTransaction& b)
{
    return !(a == b);
}

bool Enze::operator==(const CTransaction& a, const CTransaction& b)
{
    return (a.m_nCurVersion  == b.m_nCurVersion &&
            a.m_vTxIn       == b.m_vTxIn &&
            a.m_vTxOut      == b.m_vTxOut &&
            a.m_nLockTime == b.m_nLockTime);
}

/* EOF */

