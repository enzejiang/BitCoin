/*
 * =====================================================================================
 *
 *       Filename:  WalletServ.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/14/2018 04:01:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "CommonBase/uint256.h"
#include "CommonBase/base58.h"
#include "CommonBase/ProtocSerialize.h"
#include "ProtocSrc/Transaction.pb.h"
#include "DAO/DaoServ.h"
#include "BlockEngine/BlockEngine.h"
#include "WalletService/WalletServ.h"
#include "WalletService/CTransaction.h"
#include "WalletService/CMerkleTx.h"
#include "WalletService/CWalletTx.h"
#include "WalletService/CTxInPoint.h"
#include "WalletService/CTxIndex.h"
#include "NetWorkService/CInv.h"

using namespace Enze;

WalletServ* WalletServ::m_pInstance = NULL;
pthread_mutex_t WalletServ::m_mutexLock;

WalletServ* WalletServ::getInstance()
{
    if (NULL == m_pInstance)
    {
        pthread_mutex_lock(&m_mutexLock); 
        
        if (NULL == m_pInstance) 
        {
            m_pInstance = new WalletServ();
        }
        
        pthread_mutex_unlock(&m_mutexLock); 
    
    }
    
    return m_pInstance;
}

void WalletServ::Destory()
{
    pthread_mutex_lock(&m_mutexLock); 
    if (NULL != m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
    pthread_mutex_unlock(&m_mutexLock); 

}

void WalletServ::initiation()
{
    //
    // Load data files
    //
    string strErrors;

    printf("Loading wallet...\n");
    if (!LoadWallet())
        strErrors += "Error loading wallet.dat      \n";

    printf("Done loading\n");

    //// debug print
    printf("mapKeys.size() = %d\n",         mapKeys.size());
    printf("mapPubKeys.size() = %d\n",      mapPubKeys.size());
    printf("mapWallet.size() = %d\n",       mapWallet.size());
    printf("mapAddressBook.size() = %d\n",  mapAddressBook.size());
    map<string, string>::iterator it = mapAddressBook.begin();
    for (;it != mapAddressBook.end(); ++it) {
        printf("mapAddressBook, key[%s]--value[%s]\n", it->first.c_str(), it->second.c_str());

    }

    if (!strErrors.empty())
    {
        printf("Initiation Error\n");
        exit(-1);
    }

}

WalletServ::WalletServ()
{
    nTransactionsUpdated = 0; // 每次对mapTransactions中交易进行更新，都对该字段进行++操作
    nTransactionFee = 0; // 交易费用

}

WalletServ::~WalletServ()
{

}

bool WalletServ::LoadWallet()
{
    vector<unsigned char> vchDefaultKey;
    if (!DaoServ::getInstance()->LoadWallet(vchDefaultKey))
        return false;
    printf("%s---%d---\n", __FILE__, __LINE__);
    if (mapKeys.count(vchDefaultKey))
    {
        printf("%s---%d---add key at mem\n", __FILE__, __LINE__);
        // Set keyUser
        keyUser.SetPubKey(vchDefaultKey);
        keyUser.SetPrivKey(mapKeys[vchDefaultKey]);
    }
    else
    {
        printf("%s---%d---add key at db\n", __FILE__, __LINE__);
        // Create new keyUser and set as default key
        RandAddSeed(true);
        keyUser.MakeNewKey();
        if (!AddKey(keyUser))
            return false;
        if (!DaoServ::getInstance()->SetAddressBookName(PubKeyToAddress(keyUser.GetPubKey()), "Enze"))
            return false;
        DaoServ::getInstance()->WriteDefaultKey(keyUser.GetPubKey());
    }
    printf("%s---%d---\n", __FILE__, __LINE__);
    return true;
}

//////////////////////////////////////////////////////////////////////////////
//
// mapKeys
//
// 将对应key的信息存放到对应的全局变量中
bool WalletServ::AddKey(const CKey& key)
{
     printf("%s---%d--%s\n", __FILE__, __LINE__, __func__);
    {
        mapKeys[key.GetPubKey()] = key.GetPrivKey();
        mapPubKeys[Hash160(key.GetPubKey())] = key.GetPubKey();
    }
     printf("%s---%d--%s\n", __FILE__, __LINE__, __func__);

    return DaoServ::getInstance()->WriteKey(key.GetPubKey(), key.GetPrivKey());
}
// 产生新的公私钥对
vector<unsigned char> WalletServ::GenerateNewKey()
{
    CKey key;
    key.MakeNewKey();
    if (!AddKey(key))
        throw runtime_error("GenerateNewKey() : AddKey failed\n");
    return key.GetPubKey();
}




//////////////////////////////////////////////////////////////////////////////
//
// mapWallet
//
// 将当前交易增加到钱包mapWallet中：无则插入，有则更新，mapWallet仅仅存放和本节点相关的交易
bool WalletServ::AddToWallet(const CWalletTx& wtxIn)
{
    uint256 hash = wtxIn.GetHash();
 //   CRITICAL_BLOCK(cs_mapWallet)
    {
        // Inserts only if not already there, returns tx inserted or tx found
        pair<map<uint256, CWalletTx>::iterator, bool> ret = mapWallet.insert(make_pair(hash, wtxIn));
        CWalletTx& wtx = (*ret.first).second;
        bool fInsertedNew = ret.second; // 判断是否是新插入的（也即是原来对应mapWallet中没有）
        if (fInsertedNew)
            wtx.m_uTimeReceived = GetAdjustedTime(); // 交易被节点接收的时间

        //// debug print
        printf("AddToWallet %s  %s\n", wtxIn.GetHash().ToString().substr(0,6).c_str(), fInsertedNew ? "new" : "update");

        if (!fInsertedNew)
        {
			// 当前交易已经在mapWallet中存在
            // Merge
            bool fUpdated = false;
            if (wtxIn.m_hashBlock != 0 && wtxIn.m_hashBlock != wtx.m_hashBlock)
            {
                wtx.m_hashBlock = wtxIn.m_hashBlock;
                fUpdated = true;
            }
            if (wtxIn.m_nIndex != -1 && (wtxIn.m_vMerkleBranch != wtx.m_vMerkleBranch || wtxIn.m_nIndex != wtx.m_nIndex))
            {
                wtx.m_vMerkleBranch = wtxIn.m_vMerkleBranch;
                wtx.m_nIndex = wtxIn.m_nIndex;
                fUpdated = true;
            }
            if (wtxIn.m_bFromMe && wtxIn.m_bFromMe != wtx.m_bFromMe)
            {
                wtx.m_bFromMe = wtxIn.m_bFromMe;
                fUpdated = true;
            }
            if (wtxIn.m_bSpent && wtxIn.m_bSpent != wtx.m_bSpent)
            {
                wtx.m_bSpent = wtxIn.m_bSpent;
                fUpdated = true;
            }
            if (!fUpdated)
                return true;
        }

        // Write to disk
        if (!wtx.WriteToDisk())
            return false;

        // Notify UI
        vWalletUpdated.push_back(make_pair(hash, fInsertedNew));
    }

    // Refresh UI
    //MainFrameRepaint();
    return true;
}

// 如果当前交易属于本节点，则将当前交易加入到钱包中
bool WalletServ::AddToWalletIfMine(const CTransaction& tx, const CBlock* pblock)
{
    if (tx.IsMine() || mapWallet.count(tx.GetHash()))
    {
        CWalletTx wtx(tx);
        // Get merkle branch if transaction was found in a block
        if (pblock)
            wtx.SetMerkleBranch(pblock);
        return AddToWallet(wtx);
    }
    return true;
}

// 将交易从钱包映射对象mapWallet中移除，同时将交易从CWalletDB中移除
bool WalletServ::EraseFromWallet(uint256 hash)
{
 //   CRITICAL_BLOCK(cs_mapWallet)
    {
        if (mapWallet.erase(hash))
            DaoServ::getInstance()->EraseTx(hash);
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////
//
// mapOrphanTransactions
//
// 增加孤儿交易
void WalletServ::AddOrphanTx(const Transaction& pbTx)
{
    CTransaction *tx = new CTransaction();
    UnSeriaTransaction(pbTx, *tx);

    uint256 hash = tx->GetHash();
    if (mapOrphanTransactions.count(hash))
        return;
   mapOrphanTransactions[hash] = tx;
    // 当前交易对应的输入对应的交易hash
    foreach(const CTxIn& txin, tx->m_vTxIn)
        mapOrphanTransactionsByPrev.insert(make_pair(txin.m_cPrevOut.m_u256Hash, tx));
}
// 删除对应的孤儿交易
void WalletServ::EraseOrphanTx(uint256 hash)
{
    if (!mapOrphanTransactions.count(hash))
        return;
    const CTransaction* tx = mapOrphanTransactions[hash];

    foreach(const CTxIn& txin, tx->m_vTxIn)
    {
        for (multimap<uint256, CTransaction*>::iterator mi = mapOrphanTransactionsByPrev.lower_bound(txin.m_cPrevOut.m_u256Hash);
             mi != mapOrphanTransactionsByPrev.upper_bound(txin.m_cPrevOut.m_u256Hash);)
        {
            if ((*mi).second == tx)
                mapOrphanTransactionsByPrev.erase(mi++);
            else
                mi++;
        }
    }
    delete tx;
    mapOrphanTransactions.erase(hash);
}


void WalletServ::ReacceptWalletTransactions()
{
    // Reaccept any txes of ours that aren't already in a block
    {
        foreach(PAIRTYPE(const uint256, CWalletTx)& item, mapWallet)
        {
            CWalletTx& wtx = item.second;
            if (!wtx.IsCoinBase() && !DaoServ::getInstance()->ContainsTx(wtx.GetHash()))
                wtx.AcceptWalletTransaction(false);
        }
    }
}

// 在相连的节点之间转播那些到目前为止还没有进入block中的钱包交易
void WalletServ::RelayWalletTransactions()
{
    static int64 nLastTime;
	// 转播钱包交易时间的间隔是10分钟，小于10分钟则不进行转播
    if (GetTime() - nLastTime < 10 * 60)
        return;
    nLastTime = GetTime();

    // Rebroadcast any of our txes that aren't in a block yet
    printf("RelayWalletTransactions()\n");
    {
		// 按照时间（被当前节点接收的时间）顺序对钱包中的交易进行排序
        // Sort them in chronological order
        multimap<unsigned int, CWalletTx*> mapSorted;// 默认是按照unsigned int对应的值升序排列，即是越早时间越靠前
        foreach(PAIRTYPE(const uint256, CWalletTx)& item, mapWallet)
        {
            CWalletTx& wtx = item.second;
            mapSorted.insert(make_pair(wtx.m_uTimeReceived, &wtx));
        }
        foreach(PAIRTYPE(const unsigned int, CWalletTx*)& item, mapSorted)
        {
            CWalletTx& wtx = *item.second;
			// 钱包交易进行转播
            wtx.RelayWalletTransaction();
        }
    }
}

// Messages
//

// 判断对应的请求消息是否已经存在
bool WalletServ::AlreadyHave(const CInv& inv)
{
    switch (inv.type)
    {
        case MSG_TX:        return mapTransactions.count(inv.hash) || DaoServ::getInstance()->ContainsTx(inv.hash);
    case MSG_BLOCK:     return BlockEngine::getInstance()->getMapBlockIndex().count(inv.hash) || BlockEngine::getInstance()->mapOrphanBlocks.count(inv.hash);
    case MSG_REVIEW:    return true;
    case MSG_PRODUCT:   return mapProducts.count(inv.hash);
    }
    // Don't know what it is, just say we already got one
    return true;
}

//////////////////////////////////////////////////////////////////////////////
//
// Actions
//


int64 WalletServ::GetBalance()
{
    int64 nStart, nEnd;
  //  QueryPerformanceCounter((LARGE_INTEGER*)&nStart);

    int64 nTotal = COIN*100;
    //CRITICAL_BLOCK(cs_mapWallet)
    {
        for (map<uint256, CWalletTx>::iterator it = mapWallet.begin(); it != mapWallet.end(); ++it)
        {
            CWalletTx* pcoin = &(*it).second;
            if (!pcoin->IsFinal() || pcoin->m_bSpent)
                continue;
            nTotal += pcoin->GetCredit();
        }
    }

//    QueryPerformanceCounter((LARGE_INTEGER*)&nEnd);
    printf(" GetBalance() nTotal = %lld\n", nTotal);
    return nTotal;
}


bool WalletServ::SelectCoins(int64 nTargetValue, set<CWalletTx*>& setCoinsRet)
{
    setCoinsRet.clear();

    // List of values less than target
    int64 nLowestLarger = _I64_MAX;
    CWalletTx* pcoinLowestLarger = NULL;
    vector<pair<int64, CWalletTx*> > vValue;
    int64 nTotalLower = 0;
    mapWallet.count(18);
    printf("SelectCoins() WalletSize[%d]\n", mapWallet.size());
    printf("nTragetValue[%lld]\n", nTargetValue);
    //CRITICAL_BLOCK(cs_mapWallet)
    {
        for (map<uint256, CWalletTx>::iterator it = mapWallet.begin(); it != mapWallet.end(); ++it)
        {
            CWalletTx* pcoin = &(*it).second;
            if (!pcoin->IsFinal() || pcoin->m_bSpent)
                continue;
            int64 n = pcoin->GetCredit();
            printf("SelectCoins---coin Value[%lld], nLowestLarger[%lld]\n", n, nLowestLarger);
            if (n <= 0)
                continue;
            if (n < nTargetValue)
            {
                vValue.push_back(make_pair(n, pcoin));
                nTotalLower += n;
            }
            else if (n == nTargetValue)
            {
                setCoinsRet.insert(pcoin);
                return true;
            }
            else if (n < nLowestLarger)
            {
                printf("SelectCoins---Find coin Value[%lld]\n", n);
                nLowestLarger = n;
                pcoinLowestLarger = pcoin;
            }
        }
    }
    printf("SelectCoins() nTotalLower:[%lld] \n", nTotalLower);
    if (nTotalLower < nTargetValue)
    {
        if (pcoinLowestLarger == NULL)
            return false;
        setCoinsRet.insert(pcoinLowestLarger);
        return true;
    }

    // Solve subset sum by stochastic approximation
    sort(vValue.rbegin(), vValue.rend());
    vector<char> vfIncluded;
    vector<char> vfBest(vValue.size(), true);
    int64 nBest = nTotalLower;

    for (int nRep = 0; nRep < 1000 && nBest != nTargetValue; nRep++)
    {
        vfIncluded.assign(vValue.size(), false);
        int64 nTotal = 0;
        bool fReachedTarget = false;
        for (int nPass = 0; nPass < 2 && !fReachedTarget; nPass++)
        {
            for (int i = 0; i < vValue.size(); i++)
            {
                if (nPass == 0 ? rand() % 2 : !vfIncluded[i])
                {
                    nTotal += vValue[i].first;
                    vfIncluded[i] = true;
                    if (nTotal >= nTargetValue)
                    {
                        fReachedTarget = true;
                        if (nTotal < nBest)
                        {
                            nBest = nTotal;
                            vfBest = vfIncluded;
                        }
                        nTotal -= vValue[i].first;
                        vfIncluded[i] = false;
                    }
                }
            }
        }
    }

    // If the next larger is still closer, return it
    if (pcoinLowestLarger && nLowestLarger - nTargetValue <= nBest - nTargetValue)
        setCoinsRet.insert(pcoinLowestLarger);
    else
    {
        for (int i = 0; i < vValue.size(); i++)
            if (vfBest[i])
                setCoinsRet.insert(vValue[i].second);

        //// debug print
        printf("SelectCoins() best subset: ");
        for (int i = 0; i < vValue.size(); i++)
            if (vfBest[i])
                printf("%s ", FormatMoney(vValue[i].first).c_str());
        printf("total %s\n", FormatMoney(nBest).c_str());
    }
    printf("SelectCoins Return true\n");
    return true;
}


bool WalletServ::CreateTransaction(CScript scriptPubKey, int64 nValue, CWalletTx& wtxNew, int64& nFeeRequiredRet)
{
    nFeeRequiredRet = 0;
    {
        // txdb must be opened before the mapWallet lock
        {
            int64 nFee = nTransactionFee;
            loop
            {
                wtxNew.m_vTxIn.clear();
                wtxNew.m_vTxOut.clear();
                if (nValue < 0)
                    return false;
                int64 nValueOut = nValue;
                nValue += nFee;

                // Choose coins to use
                set<CWalletTx*> setCoins;
                if (!SelectCoins(nValue, setCoins)) 
                {
                     printf("CreateTransaction--SelectCoins Fail\n");
                    return false;
                
                }
                printf("CreateTransaction--SelectCoins Ok\n");
                int64 nValueIn = 0;
                foreach(CWalletTx* pcoin, setCoins)
                    nValueIn += pcoin->GetCredit();

                printf("CreateTransaction--nValueOut[%lu]--nValue[%lu]--nValueIn[%lu]\n", nValueOut, nValue, nValueIn);
                printf("CreateTransaction--scriptPubKey[%s]\n", scriptPubKey.ToString().c_str());
                // Fill vout[0] to the payee
                wtxNew.m_vTxOut.push_back(CTxOut(nValueOut, scriptPubKey));

                // Fill vout[1] back to self with any change
                if (nValueIn > nValue)
                {
                    /// todo: for privacy, should randomize the order of outputs,
                    //        would also have to use a new key for the change.
                    // Use the same key as one of the coins
                    vector<unsigned char> vchPubKey;
                    CTransaction& txFirst = *(*setCoins.begin());
                    foreach(const CTxOut& txout, txFirst.m_vTxOut)
                        if (txout.IsMine())
                            if (ExtractPubKey(txout.m_cScriptPubKey, true, vchPubKey))
                                break;
                    if (vchPubKey.empty())
                        return false;

                    // Fill vout[1] to ourself
                    CScript scriptPubKey;
                    scriptPubKey << vchPubKey << OP_CHECKSIG;
                    wtxNew.m_vTxOut.push_back(CTxOut(nValueIn - nValue, scriptPubKey));
                }

                // Fill vin
                foreach(CWalletTx* pcoin, setCoins)
                    for (int nOut = 0; nOut < pcoin->m_vTxOut.size(); nOut++)
                        if (pcoin->m_vTxOut[nOut].IsMine())
                            wtxNew.m_vTxIn.push_back(CTxIn(pcoin->GetHash(), nOut));

                // Sign
                int nIn = 0;
                foreach(CWalletTx* pcoin, setCoins)
                    for (int nOut = 0; nOut < pcoin->m_vTxOut.size(); nOut++)
                        if (pcoin->m_vTxOut[nOut].IsMine())
                            SignSignature(*pcoin, wtxNew, nIn++);
                // Check that enough fee is included
                if (nFee < wtxNew.GetMinFee(true))
                {
                    nFee = nFeeRequiredRet = wtxNew.GetMinFee(true);
                    continue;
                }
                // Fill vtxPrev by copying from previous transactions vtxPrev
                wtxNew.AddSupportingTransactions();
                wtxNew.m_bTimeReceivedIsTxTime = true;

                break;
            }
        }
    }
    return true;
}

// Call after CreateTransaction unless you want to abort
bool WalletServ::CommitTransactionSpent(const CWalletTx& wtxNew)
{
    //// todo: make this transactional, never want to add a transaction
    ////  without marking spent transactions
    printf("CommitTransactionSpent---1\n");
    // Add tx to wallet, because if it has change it's also ours,
    // otherwise just for transaction history.
    AddToWallet(wtxNew);

    // Mark old coins as spent
    set<CWalletTx*> setCoins;
    foreach(const CTxIn& txin, wtxNew.m_vTxIn)
        setCoins.insert(&mapWallet[txin.m_cPrevOut.m_u256Hash]);
    foreach(CWalletTx* pcoin, setCoins)
    {
        pcoin->m_bSpent = true;
        pcoin->WriteToDisk();
        vWalletUpdated.push_back(make_pair(pcoin->GetHash(), false));
    }
    return true;
}

bool WalletServ::SendMoney(CScript scriptPubKey, int64 nValue, CWalletTx& wtxNew)
{
//CRITICAL_BLOCK(cs_main)
    {
        int64 nFeeRequired;
        if (!CreateTransaction(scriptPubKey, nValue, wtxNew, nFeeRequired))
        {
            string strError;
            if (nValue + nFeeRequired > GetBalance())
                strError = strprintf("Error: This is an oversized transaction that requires a transaction fee of %s  ", FormatMoney(nFeeRequired).c_str());
            else
                strError = "Error: Transaction creation failed  ";
            //wxMessageBox(strError, "Sending...");
            return error("SendMoney() : %s\n", strError.c_str());
        }
        if (!CommitTransactionSpent(wtxNew))
        {
            //wxMessageBox("Error finalizing transaction  ", "Sending...");
            return error("SendMoney() : Error finalizing transaction");
        }

        printf("SendMoney: %s\n", wtxNew.GetHash().ToString().substr(0,6).c_str());

        // Broadcast
        if (!wtxNew.AcceptTransaction())
        {
            // This must not fail. The transaction has already been signed and recorded.
            throw runtime_error("SendMoney() : wtxNew.AcceptTransaction() failed\n");
            //wxMessageBox("Error: Transaction not valid  ", "Sending...");
            return error("SendMoney() : Error: Transaction not valid");
        }
        wtxNew.RelayWalletTransaction();
    }
    //MainFrameRepaint();
    return true;
}

/* EOF */

