// Copyright (c) 2009 Satoshi Nakamoto
// Distributed under the MIT/X11 software license, see the accompanying
// file license.txt or http://www.opensource.org/licenses/mit-license.php.
#ifndef CXX_BT_MAIN_H
#define CXX_BT_MAIN_H
#include "headers.h"
#include "bignum.h"
#include "uint256.h"
#include "script.h"
#include "net.h"
class COutPoint;
class CInPoint;
class CDiskTxPos;
class CCoinBase;
class CTxIn;
class CTxOut;
class CTransaction;
class CBlock;
class CBlockIndex;
class CWalletTx;
class CKeyItem;
class CNode;
static const unsigned int MAX_SIZE = 0x02000000;
// COIN 表示的是一个比特币，而且100000000就是表示一个比特币，比特币最小单位为小数点后8位
static const int64 COIN = 100000000;
static const int64 CENT = 1000000;
static const int COINBASE_MATURITY = 100;// 币基成熟度
// 工作量证明的难度
static const CBigNum bnProofOfWorkLimit(~uint256(0) >> 32);






//extern CCriticalSection cs_main;
extern map<uint256, CBlockIndex*> mapBlockIndex;
extern const uint256 hashGenesisBlock;
extern CBlockIndex* pindexGenesisBlock;
extern int nBestHeight;
extern uint256 hashBestChain;
extern CBlockIndex* pindexBest;
extern unsigned int nTransactionsUpdated;
extern string strSetDataDir;
extern int nDropMessagesTest;

// Settings
extern int fGenerateBitcoins;
extern int64 nTransactionFee;
extern CAddress addrIncoming;







string GetAppDir();
bool CheckDiskSpace(int64 nAdditionalBytes=0);
FILE* OpenBlockFile(unsigned int m_nFile, unsigned int m_nBlockPos, const char* pszMode="rb");
FILE* AppendBlockFile(unsigned int& m_nFileRet);
bool AddKey(const CKey& key);
vector<unsigned char> GenerateNewKey();
bool AddToWallet(const CWalletTx& wtxIn);
void ReacceptWalletTransactions();
void RelayWalletTransactions();
bool LoadBlockIndex(bool fAllowNew=true);
void PrintBlockTree();
bool BitcoinMiner();
bool ProcessMessages(CNode* pfrom);
bool ProcessMessage(CNode* pfrom, string strCommand, CDataStream& vRecv);
bool SendMessages(CNode* pto);
int64 GetBalance();
bool CreateTransaction(CScript m_cScriptPubKey, int64 nValue, CWalletTx& txNew, int64& nFeeRequiredRet);
bool CommitTransactionSpent(const CWalletTx& wtxNew);
bool SendMoney(CScript m_cScriptPubKey, int64 nValue, CWalletTx& wtxNew);











class CDiskTxPos
{
public:
    unsigned int m_nFile; // 块所在文件的信息，而且块文件的命名一般是blk${nFile}.dat
    unsigned int m_nBlockPos; // 当前块在对应块文件中的偏移
    unsigned int m_nTxPos; // 交易在对应块中的偏移

    CDiskTxPos()
    {
        SetNull();
    }

    CDiskTxPos(unsigned int nFileIn, unsigned int nBlockPosIn, unsigned int nTxPosIn)
    {
        m_nFile = nFileIn;
        m_nBlockPos = nBlockPosIn;
        m_nTxPos = nTxPosIn;
    }

    IMPLEMENT_SERIALIZE( READWRITE(FLATDATA(*this)); )
    void SetNull() { m_nFile = -1; m_nBlockPos = 0; m_nTxPos = 0; }
    bool IsNull() const { return (m_nFile == -1); }

    friend bool operator==(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return (a.m_nFile     == b.m_nFile &&
                a.m_nBlockPos == b.m_nBlockPos &&
                a.m_nTxPos    == b.m_nTxPos);
    }

    friend bool operator!=(const CDiskTxPos& a, const CDiskTxPos& b)
    {
        return !(a == b);
    }

    string ToString() const
    {
        if (IsNull())
            return strprintf("null");
        else
            return strprintf("(m_nFile=%d, m_nBlockPos=%d, m_nTxPos=%d)", m_nFile, m_nBlockPos, m_nTxPos);
    }

    void print() const
    {
        printf("%s", ToString().c_str());
    }
};




class CInPoint
{
public:
    CTransaction* m_pcTrans; // 交易指针
    unsigned int m_nIndex; // 对应交易当前的第几个输入

    CInPoint() { SetNull(); }
    CInPoint(CTransaction* ptxIn, unsigned int nIn) { m_pcTrans = ptxIn; m_nIndex = nIn; }
    void SetNull() { m_pcTrans = NULL; m_nIndex = -1; }
    bool IsNull() const { return (m_pcTrans == NULL && m_nIndex == -1); }
};




class COutPoint
{
public:
    uint256 m_u256Hash; // 交易对应的hash
    unsigned int m_nIndex; // 交易对应的第几个输出

    COutPoint() { SetNull(); }
    COutPoint(uint256 hashIn, unsigned int nIn) { m_u256Hash = hashIn; m_nIndex = nIn; }
    IMPLEMENT_SERIALIZE( READWRITE(FLATDATA(*this)); )
    void SetNull() { m_u256Hash = 0; m_nIndex = -1; }
    bool IsNull() const { return (m_u256Hash == 0 && m_nIndex == -1); }

    friend bool operator<(const COutPoint& a, const COutPoint& b)
    {
        return (a.m_u256Hash < b.m_u256Hash || (a.m_u256Hash == b.m_u256Hash && a.m_nIndex < b.m_nIndex));
    }

    friend bool operator==(const COutPoint& a, const COutPoint& b)
    {
        return (a.m_u256Hash == b.m_u256Hash && a.m_nIndex == b.m_nIndex);
    }

    friend bool operator!=(const COutPoint& a, const COutPoint& b)
    {
        return !(a == b);
    }

    string ToString() const
    {
        return strprintf("COutPoint(%s, %d)", m_u256Hash.ToString().substr(0,6).c_str(), m_nIndex);
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};




//
// An input of a transaction.  It contains the location of the previous
// transaction's output that it claims and a signature that matches the
// output's public key.
//
class CTxIn
{
public:
    COutPoint m_cPrevOut; // 前一个交易对应的输出（叫一个交易对应的hash值和对应的第几个输出）
    CScript m_cScriptSig; // 输入脚本对应的签名
    unsigned int m_uSequence;// 主要是用于判断相同输入的交易哪一个更新，值越大越新

    CTxIn()
    {
        m_uSequence = UINT_MAX;
    }

    explicit CTxIn(COutPoint preoutIn, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=UINT_MAX)
    {
        m_cPrevOut = preoutIn;
        m_cScriptSig = scriptSigIn;
        m_uSequence = nSequenceIn;
    }

    CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=UINT_MAX)
    {
        m_cPrevOut = COutPoint(hashPrevTx, nOut);
        m_cScriptSig = scriptSigIn;
        m_uSequence = nSequenceIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(m_cPrevOut);
        READWRITE(m_cScriptSig);
        READWRITE(m_uSequence);
    )
    // 交易对应nSequence是最大，已经是最新了，是最终的
    bool IsFinal() const
    {
        return (m_uSequence == UINT_MAX);
    }

    friend bool operator==(const CTxIn& a, const CTxIn& b)
    {
        return (a.m_cPrevOut   == b.m_cPrevOut &&
                a.m_cScriptSig == b.m_cScriptSig &&
                a.m_uSequence == b.m_uSequence);
    }

    friend bool operator!=(const CTxIn& a, const CTxIn& b)
    {
        return !(a == b);
    }

    string ToString() const
    {
        string str;
        str += strprintf("CTxIn(");
        str += m_cPrevOut.ToString();
        if (m_cPrevOut.IsNull())
            str += strprintf(", coinbase %s", HexStr(m_cScriptSig.begin(), m_cScriptSig.end(), false).c_str());
        else
            str += strprintf(", scriptSig=%s", m_cScriptSig.ToString().substr(0,24).c_str());
        if (m_uSequence != UINT_MAX)
            str += strprintf(", nSequence=%u", m_uSequence);
        str += ")";
        return str;
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }

	// 判断当前输入的交易是否属于本节点，就是对应的脚本签名是否在本地能够找到
    bool IsMine() const;
	// 获得对应交易的借方金额，如果对应的输入是本节点的账号，则借方金额就是交易输入金额
    int64 GetDebit() const;
};




//
// An output of a transaction.  It contains the public key that the next input
// must be able to sign with to claim it.
//
class CTxOut
{
public:
    int64 m_nValue; // 交易输出对应的金额
    CScript m_cScriptPubKey; // 交易对应的公钥

public:
    CTxOut()
    {
        SetNull();
    }

    CTxOut(int64 nValueIn, CScript cScriptPubKeyIn)
    {
        m_nValue = nValueIn;
        m_cScriptPubKey = cScriptPubKeyIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(m_nValue);
        READWRITE(m_cScriptPubKey);
    )

    void SetNull()
    {
        m_nValue = -1;
        m_cScriptPubKey.clear();
    }

    bool IsNull()
    {
        return (m_nValue == -1);
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

	// 判断交易的输出是否是节点自己本身对应的交易，也是在当前中根据公钥能够找到对应的私钥
    bool IsMine() const
    {
        return ::IsMine(m_cScriptPubKey);
    }

	// 获取当前交易数据贷方金额，如果是节点本身的交易则返回对应输出的金额，否则对节点来说其对应的贷方金额为0
    int64 GetCredit() const
    {
        if (IsMine())
            return m_nValue;
        return 0;
    }

    friend bool operator==(const CTxOut& a, const CTxOut& b)
    {
        return (a.m_nValue       == b.m_nValue &&
                a.m_cScriptPubKey == b.m_cScriptPubKey);
    }

    friend bool operator!=(const CTxOut& a, const CTxOut& b)
    {
        return !(a == b);
    }

    string ToString() const
    {
        if (m_cScriptPubKey.size() < 6)
            return "CTxOut(error)";
        return strprintf("CTxOut(nValue=%I64d.%08I64d, m_cScriptPubKey=%s)", m_nValue / COIN, m_nValue % COIN, m_cScriptPubKey.ToString().substr(0,24).c_str());
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};




//
// The basic transaction that is broadcasted on the network and contained in
// blocks.  A transaction can contain multiple inputs and outputs.
//
class CTransaction
{
public:
    int m_nCurVersion; // 交易的版本号，用于升级
    vector<CTxIn> m_vTxIn; // 交易对应的输入
    vector<CTxOut> m_vTxOut; // 交易对应的输出
    int m_nLockTime; // 交易对应的锁定时间


    CTransaction()
    {
        SetNull();
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->m_nCurVersion);
        nVersion = this->m_nCurVersion;
        READWRITE(m_vTxIn);
        READWRITE(m_vTxOut);
        READWRITE(m_nLockTime);
    )

    void SetNull()
    {
        m_nCurVersion = 1;
        m_vTxIn.clear();
        m_vTxOut.clear();
        m_nLockTime = 0;
    }

    bool IsNull() const
    {
        return (m_vTxIn.empty() && m_vTxOut.empty());
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

    // 判断是否是最终的交易
    bool IsFinal() const
    {
        // 如果锁定时间等于0或者锁定时间小于主链的长度，则说明是最终的交易
        if (m_nLockTime == 0 || m_nLockTime < nBestHeight)
            return true;
        foreach(const CTxIn& txin, m_vTxIn)
            if (!txin.IsFinal())
                return false;
        return true;
    }
	// 对比相同的交易哪一个更新点：对于相同输入的交易判断哪一个更新
    bool IsNewerThan(const CTransaction& old) const
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
    bool IsCoinBase() const
    {
        return (m_vTxIn.size() == 1 && m_vTxIn[0].m_cPrevOut.IsNull());
    }
	/* 对这边交易进行检查：
	(1)交易对应的输入或者输出列表都不能为空
	(2)交易对应的输出金额不能小于0
	(3)如果是币基交易，则对应的输入只能为1，且对应的输入签名大小不能大于100
	(4)如果是非币基交易，则对应的交易输入的输出不能为空
	*/
    bool CheckTransaction() const
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
    bool IsMine() const
    {
        foreach(const CTxOut& txout, m_vTxOut)
            if (txout.IsMine())
                return true;
        return false;
    }

	// 获得当前交易总的输入：借方
    int64 GetDebit() const
    {
        int64 nDebit = 0;
        foreach(const CTxIn& txin, m_vTxIn)
            nDebit += txin.GetDebit();
        return nDebit;
    }

	// 获得当前交易总的贷方金额：属于节点自身的
    int64 GetCredit() const
    {
        int64 nCredit = 0;
        foreach(const CTxOut& txout, m_vTxOut)
            nCredit += txout.GetCredit();
        return nCredit;
    }
	// 获取交易对应所有输出金额之和
    int64 GetValueOut() const
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
    int64 GetMinFee(bool fDiscount=false) const
    {
        // Base fee is 1 cent per kilobyte
        unsigned int nBytes = ::GetSerializeSize(*this, SER_NETWORK);
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
    bool ReadFromDisk(CDiskTxPos pos, FILE** pfileRet=NULL)
    {
        CAutoFile filein = OpenBlockFile(pos.m_nFile, 0, pfileRet ? "rb+" : "rb");
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


    friend bool operator==(const CTransaction& a, const CTransaction& b)
    {
        return (a.m_nCurVersion  == b.m_nCurVersion &&
                a.m_vTxIn       == b.m_vTxIn &&
                a.m_vTxOut      == b.m_vTxOut &&
                a.m_nLockTime == b.m_nLockTime);
    }

    friend bool operator!=(const CTransaction& a, const CTransaction& b)
    {
        return !(a == b);
    }


    string ToString() const
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

    void print() const
    {
        printf("%s", ToString().c_str());
    }


	// 断开连接：释放交易对应输入的占用和将交易从对应的交易索引表中释放掉
    bool DisconnectInputs(CTxDB& txdb);
	// 交易输入链接，将对应的交易输入占用对应的交易输入的花费标记
    bool ConnectInputs(CTxDB& txdb, map<uint256, CTxIndex>& mapTestPool, CDiskTxPos posThisTx, int m_nCurHeight, int64& nFees, bool fBlock, bool fMiner, int64 nMinFee=0);
	// 客户端连接输入，对交易本身进行验证
	bool ClientConnectInputs();
	// 判断这笔交易是否应该被接受
    bool AcceptTransaction(CTxDB& txdb, bool fCheckInputs=true, bool* pfMissingInputs=NULL);

    bool AcceptTransaction(bool fCheckInputs=true, bool* pfMissingInputs=NULL)
    {
        CTxDB txdb("r");
        return AcceptTransaction(txdb, fCheckInputs, pfMissingInputs);
    }

protected:
	// 将当前交易增加到内存池mapTransactions,mapNextTx中，并且更新交易更新的次数
    bool AddToMemoryPool();
public:
	// 将当前交易从内存对象mapTransactions，mapNextTx中移除，并且增加交易对应的更新次数
    bool RemoveFromMemoryPool();
};





//
// A transaction with a merkle branch linking it to the block chain
//
class CMerkleTx : public CTransaction
{
public:
    uint256 m_hashBlock;// 交易所在block对应的hash值，因为block中有对应整个交易的默克尔树，这样才能根据分支来校验当前交易是否在block中
    vector<uint256> m_vMerkleBranch; // 当前交易对应的默克尔分支
    int m_nIndex;// 当前交易在对应的block对应的输入m_vTrans列表中的索引，CMerkleTx就是根据索引来计算这个交易对应的默克尔树分支的

    // memory only
    mutable bool m_bMerkleVerified;// 标记默克尔交易是否已经校验，如果没有校验则进行校验，校验之后将这个值设为true


    CMerkleTx()
    {
        Init();
    }

    CMerkleTx(const CTransaction& txIn) : CTransaction(txIn)
    {
        Init();
    }

    void Init()
    {
        m_hashBlock = 0;
        m_nIndex = -1;
        m_bMerkleVerified = false;
    }

	// 获取默克尔树对应的贷方金额的时候，对于币基交易，一定要等对应的block足够成熟了才能使用
    int64 GetCredit() const
    {
        // Must wait until coinbase is safely deep enough in the chain before valuing it
        if (IsCoinBase() && GetBlocksToMaturity() > 0)
            return 0;
        return CTransaction::GetCredit();
    }

    IMPLEMENT_SERIALIZE
    (
        nSerSize += SerReadWrite(s, *(CTransaction*)this, nType, m_nCurVersion, ser_action);
        nVersion = this->m_nCurVersion;
        READWRITE(m_hashBlock);
        READWRITE(m_vMerkleBranch);
        READWRITE(m_nIndex);
    )

    // 如果交易在对应的区块中，则设置交易对应的默克尔树分支
    int SetMerkleBranch(const CBlock* pblock=NULL);
	//获取默克尔交易在主链中的深度--当前块距离最长链末尾中间隔了多少个block
    int GetDepthInMainChain() const;
	// 判断当前交易是否在主链上
    bool IsInMainChain() const { return GetDepthInMainChain() > 0; }
	// 判断对应的块是否成熟，即是被其他矿工所接受认可，如果是非币基交易对应的为块成熟度为0，否则要进行计算
    // 成熟度越小越好，说明当前交易被认可的度越高
    int GetBlocksToMaturity() const;
	// 判断这边交易能不能被接受，如果能接受将对应的交易放入全局变量中mapTransactions，mapNextTx中
    bool AcceptTransaction(CTxDB& txdb, bool fCheckInputs=true);
    bool AcceptTransaction() { CTxDB txdb("r"); return AcceptTransaction(txdb); }
};




//
// A transaction with a bunch of additional info that only the owner cares
// about.  It includes any unrecorded transactions needed to link it back
// to the block chain.
//
class CWalletTx : public CMerkleTx
{
public:
    vector<CMerkleTx> m_vPrevTx; // 当前交易A对应的输入对应的交易B，如果B所在block到最长链末尾的长度小于3，则将次交易放入
    /*
	主要用于存放一下自定义的值
	wtx.mapValue["to"] = strAddress;
	wtx.mapValue["from"] = m_textCtrlFrom->GetValue();
	wtx.mapValue["message"] = m_textCtrlMessage->GetValue();
	*/
	map<string, string> m_mapValue;
	// 表单控件信息
    vector<pair<string, string> > m_vOrderForm;
    //unsigned int m_bTimeReceivedIsTxTime;// 接收时间是否是交易时间标记
    bool m_bTimeReceivedIsTxTime;// 接收时间是否是交易时间标记
    unsigned int m_m_uTimeReceived;  // time received by this node 交易被这个节点接收的时间
    /*  
    char m_bFromMe;
    char m_bSpent; // 是否花费交易标记
    */
    bool m_bFromMe;
    char m_bSpent; // 是否花费交易标记
    //// probably need to sign the order info so know it came from payer

    // memory only
    mutable unsigned int m_m_uTimeDisplayed;


    CWalletTx()
    {
        Init();
    }

    CWalletTx(const CMerkleTx& txIn) : CMerkleTx(txIn)
    {
        Init();
    }

    CWalletTx(const CTransaction& txIn) : CMerkleTx(txIn)
    {
        Init();
    }

    void Init()
    {
        m_bTimeReceivedIsTxTime = false;
        m_m_uTimeReceived = 0;
        m_bFromMe = false;
        m_bSpent = false;
        m_m_uTimeDisplayed = 0;
    }

    IMPLEMENT_SERIALIZE
    (
        nSerSize += SerReadWrite(s, *(CMerkleTx*)this, nType, m_nCurVersion, ser_action);
        nVersion = this->m_nCurVersion;
        READWRITE(m_vPrevTx);
        READWRITE(m_mapValue);
        READWRITE(m_vOrderForm);
        READWRITE(m_bTimeReceivedIsTxTime);
        READWRITE(m_m_uTimeReceived);
        READWRITE(m_bFromMe);
        READWRITE(m_bSpent);
    )

    bool WriteToDisk()
    {
        return CWalletDB().WriteTx(GetHash(), *this);
    }

	// 获取交易时间
    int64 GetTxTime() const;
	// 增加支持的交易
    void AddSupportingTransactions(CTxDB& txdb);
	// 判断当前交易能够被接收
    bool AcceptWalletTransaction(CTxDB& txdb, bool fCheckInputs=true);
    bool AcceptWalletTransaction() { CTxDB txdb("r"); return AcceptWalletTransaction(txdb); }
	// 转播钱包交易
    void RelayWalletTransaction(CTxDB& txdb);
    void RelayWalletTransaction() { CTxDB txdb("r"); RelayWalletTransaction(txdb); }
};




//
// A txdb record that contains the disk location of a transaction and the
// locations of transactions that spend its outputs.  vSpent is really only
// used as a flag, but ham_vTxIng the location is very helpful for debugging.
//
// 交易索引---每一个交易对应一个索引
class CTxIndex
{
public:
    CDiskTxPos m_cDiskPos; // 交易对应的在硬盘中文件的位置
    vector<CDiskTxPos> m_vSpent; // 标记交易的输出是否已经被消费了，根据下标来标记对应交易指定位置的输出是否已经被消费了

    CTxIndex()
    {
        SetNull();
    }

    CTxIndex(const CDiskTxPos& posIn, unsigned int nOutputs)
    {
        m_cDiskPos = posIn;
        m_vSpent.resize(nOutputs);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(m_cDiskPos);
        READWRITE(m_vSpent);
    )

    void SetNull()
    {
        m_cDiskPos.SetNull();
        m_vSpent.clear();
    }

    bool IsNull()
    {
        return m_cDiskPos.IsNull();
    }

    friend bool operator==(const CTxIndex& a, const CTxIndex& b)
    {
        if (a.m_cDiskPos != b.m_cDiskPos || a.m_vSpent.size() != b.m_vSpent.size())
            return false;
        for (int i = 0; i < a.m_vSpent.size(); i++)
            if (a.m_vSpent[i] != b.m_vSpent[i])
                return false;
        return true;
    }

    friend bool operator!=(const CTxIndex& a, const CTxIndex& b)
    {
        return !(a == b);
    }
};





//
// Nodes collect new transactions into a block, hash them into a hash tree,
// and scan through nonce values to make the block's hash satisfy proof-of-work
// requirements.  When they solve the proof-of-work, they broadcast the block
// to everyone and the block is added to the block chain.  The first transaction
// in the block is a special one that creates a new coin owned by the creator
// of the block.
//
// Blocks are appended to blk0001.dat files on disk.  Their location on disk
// is indexed by CBlockIndex objects in memory.
//
// 块定义
class CBlock
{
public:
    // header
    int m_nCurVersion; // 块的版本，主要为了后续的升级使用
    uint256 m_hashPrevBlock; // 前一个块对应的hash
    uint256 m_hashMerkleRoot; // 默克尔对应的根
	// 取前11个区块对应的创建时间平均值
    unsigned int m_uTime; // 单位为秒，取区块链中对应的前多少个区块对应时间的中位数，如果不存在前一个则去当前时间
    unsigned int m_uBits; // 记录本区块难度
    unsigned int m_uNonce; // 工作量证明获得随机数，这个随机数正好满足当前挖矿对应的难度

    // network and disk
    vector<CTransaction> m_vTrans; // 块中交易列表

    // memory only
    mutable vector<uint256> m_vMerkleTree; // 整个交易对应的默克尔树列表


    CBlock()
    {
        SetNull();
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->m_nCurVersion);
        nVersion = this->m_nCurVersion;
        READWRITE(m_hashPrevBlock);
        READWRITE(m_hashMerkleRoot);
        READWRITE(m_uTime);
        READWRITE(m_uBits);
        READWRITE(m_uNonce);

        // ConnectBlock depends on m_vTrans being last so it can calculate offset
        if (!(nType & (SER_GETHASH|SER_BLOCKHEADERONLY)))
            READWRITE(m_vTrans);
        else if (fRead)
            const_cast<CBlock*>(this)->m_vTrans.clear();
    )

    void SetNull()
    {
        m_nCurVersion = 1;
        m_hashPrevBlock = 0;
        m_hashMerkleRoot = 0;
        m_uTime = 0;
        m_uBits = 0;
        m_uNonce = 0;
        m_vTrans.clear();
        m_vMerkleTree.clear();
    }

    bool IsNull() const
    {
        return (m_uBits == 0);
    }

	// 块hash值仅仅包含从m_nCurVersion 到 m_uNonce 的值
    uint256 GetHash() const
    {
        return Hash(BEGIN(m_nCurVersion), END(m_uNonce));
    }

	// 根据交易构建对应的默克尔树
    uint256 BuildMerkleTree() const
    {
        m_vMerkleTree.clear();
        foreach(const CTransaction& tx, m_vTrans)
            m_vMerkleTree.push_back(tx.GetHash());
        int j = 0;
        for (int nSize = m_vTrans.size(); nSize > 1; nSize = (nSize + 1) / 2)
        {
            for (int i = 0; i < nSize; i += 2)
            {
                int i2 = min(i+1, nSize-1);
                m_vMerkleTree.push_back(Hash(BEGIN(m_vMerkleTree[j+i]),  END(m_vMerkleTree[j+i]),
                                           BEGIN(m_vMerkleTree[j+i2]), END(m_vMerkleTree[j+i2])));
            }
            j += nSize;
        }
        return (m_vMerkleTree.empty() ? 0 : m_vMerkleTree.back());
    }
	// 根据交易对应的索引获得交易对应的默克尔分支
    vector<uint256> GetMerkleBranch(int nIndex) const
    {
        if (m_vMerkleTree.empty())
            BuildMerkleTree();
        vector<uint256> vMerkleBranch;
        int j = 0;
        for (int nSize = m_vTrans.size(); nSize > 1; nSize = (nSize + 1) / 2)
        {
            int i = min(nIndex^1, nSize-1);
            vMerkleBranch.push_back(m_vMerkleTree[j+i]);
            nIndex >>= 1;
            j += nSize;
        }
        return vMerkleBranch;
    }
	// 判断当前对应的交易hash和默克尔分支来验证对应的交易是否在对应的blcok中
    static uint256 CheckMerkleBranch(uint256 hash, const vector<uint256>& vMerkleBranch, int nIndex)
    {
        if (nIndex == -1)
            return 0;
        foreach(const uint256& otherside, vMerkleBranch)
        {
            if (nIndex & 1)
                hash = Hash(BEGIN(otherside), END(otherside), BEGIN(hash), END(hash));
            else
                hash = Hash(BEGIN(hash), END(hash), BEGIN(otherside), END(otherside));
            nIndex >>= 1;
        }
        return hash;
    }

	// 将block写入到文件中
    bool WriteToDisk(bool fWriteTransactions, unsigned int& nFileRet, unsigned int& nBlockPosRet)
    {
        // Open history file to append
        CAutoFile fileout = AppendBlockFile(nFileRet);
        if (!fileout)
            return error("CBlock::WriteToDisk() : AppendBlockFile failed");
        if (!fWriteTransactions)
            fileout.nType |= SER_BLOCKHEADERONLY;

        // Write index header
        unsigned int nSize = fileout.GetSerializeSize(*this);
		// 放入消息头和对应块的大小值
        fileout << FLATDATA(pchMessageStart) << nSize;

        // Write block
        nBlockPosRet = ftell(fileout);
        if (nBlockPosRet == -1)
            return error("CBlock::WriteToDisk() : ftell failed");
		// 将block的内容写入到文件中
        fileout << *this;

        return true;
    }

	// 从文件中读取块信息
    bool ReadFromDisk(unsigned int nFile, unsigned int nBlockPos, bool fReadTransactions)
    {
        SetNull();

        // Open history file to read
        CAutoFile filein = OpenBlockFile(nFile, nBlockPos, "rb");
        if (!filein)
            return error("CBlock::ReadFromDisk() : OpenBlockFile failed");
        if (!fReadTransactions)
            filein.nType |= SER_BLOCKHEADERONLY;

        // Read block 将文件中的内容读取到块中
        filein >> *this;

        // Check the header 1. 工作量证明难度比较 2. 计算的hash值要小于对应的工作量难度
        if (CBigNum().SetCompact(m_uBits) > bnProofOfWorkLimit)
            return error("CBlock::ReadFromDisk() : m_uBits errors in block header");
        if (GetHash() > CBigNum().SetCompact(m_uBits).getuint256())
            return error("CBlock::ReadFromDisk() : GetHash() errors in block header");

        return true;
    }



    void print() const
    {
        printf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, m_uTime=%u, m_uBits=%08x, m_uNonce=%u, m_vTrans=%d)\n",
            GetHash().ToString().substr(0,14).c_str(),
            m_nCurVersion,
            m_hashPrevBlock.ToString().substr(0,14).c_str(),
            m_hashMerkleRoot.ToString().substr(0,6).c_str(),
            m_uTime, m_uBits, m_uNonce,
            m_vTrans.size());
        for (int i = 0; i < m_vTrans.size(); i++)
        {
            printf("  ");
            m_vTrans[i].print();
        }
        printf("  m_vMerkleTree: ");
        for (int i = 0; i < m_vMerkleTree.size(); i++)
            printf("%s ", m_vMerkleTree[i].ToString().substr(0,6).c_str());
        printf("\n");
    }

	// 获取这个区块对应的价值（奖励+交易手续费）
    int64 GetBlockValue(int64 nFees) const;
	// 将一个区块block断开连接（就是释放区块对应的信息，同时释放区块对应的区块索引）
    bool DisconnectBlock(CTxDB& txdb, CBlockIndex* pindex);
	// 区块链接：每一个交易链接，增加到区块索引链中
    bool ConnectBlock(CTxDB& txdb, CBlockIndex* pindex);
	// 根据区块索引从数据库文件中读取对应的区块信息
    bool ReadFromDisk(const CBlockIndex* blockindex, bool fReadTransactions);
	// 将当前区块增加到对应的区块索引中
    bool AddToBlockIndex(unsigned int m_nFile, unsigned int m_nBlockPos);
	// 区块校验
    bool CheckBlock() const;
	// 判断当前区块能够被接收
    bool AcceptBlock();
};






//
// The block chain is a tree shaped structure starting with the
// genesis block at the root, with each block potentially having multiple
// candidates to be the next block.  m_pPrevBlkIndex and m_pNextBlkIndex link a path through the
// main/longest chain.  A blockindex may have multiple m_pPrevBlkIndex pointing back
// to it, but m_pNextBlkIndex will only point forward to the longest branch, or will
// be null if the block is not part of the longest chain.
//
// 如果块索引对应的pNext不为空，则这个块索引一定对应的是主链
// 块索引
class CBlockIndex
{
public:
    const uint256* m_pHashBlock; // 对应块hash值指针
    CBlockIndex* m_pPrevBlkIndex; // 指向前一个blockIndex
    CBlockIndex* m_pNextBlkIndex; // 指向当前区块索引的下一个，只有当前区块索引在主链上的时候，这个值才是非空
	// 块所在文件中的信息
    unsigned int m_nFile; 
    unsigned int m_nBlockPos;
    int m_nCurHeight; // 块索引在最长链的深度，即是中间隔了多少个block，即是从创世区块到当前区块中间隔了多少个区块

    // block header 块的头部信息
    int m_nCurVersion;
    uint256 m_hashMerkleRoot;
	// 取前11个区块对应的创建时间平均值
    unsigned int m_uTime;// 块创建时间，取块链上时间中位数
    unsigned int m_uBits;
    unsigned int m_uNonce;


    CBlockIndex()
    {
        m_pHashBlock = NULL;
        m_pPrevBlkIndex = NULL;
        m_pNextBlkIndex = NULL;
        m_nFile = 0;
        m_nBlockPos = 0;
        m_nCurHeight = 0;

        m_nCurVersion       = 0;
        m_hashMerkleRoot = 0;
        m_uTime          = 0;
        m_uBits          = 0;
        m_uNonce         = 0;
    }

    CBlockIndex(unsigned int nFileIn, unsigned int nBlockPosIn, CBlock& block)
    {
        m_pHashBlock = NULL;
        m_pPrevBlkIndex = NULL;
        m_pNextBlkIndex = NULL;
        m_nFile = nFileIn;
        m_nBlockPos = nBlockPosIn;
        m_nCurHeight = 0;

        m_nCurVersion       = block.m_nCurVersion;
        m_hashMerkleRoot = block.m_hashMerkleRoot;
        m_uTime          = block.m_uTime;
        m_uBits          = block.m_uBits;
        m_uNonce         = block.m_uNonce;
    }

    uint256 GetBlockHash() const
    {
        return *m_pHashBlock;
    }

	// 判断是否在主链，通过m_pNextBlkIndex是否为空和当前块索引指针是否就是最长链指针
    bool IsInMainChain() const
    {
        return (m_pNextBlkIndex || this == pindexBest);
    }

	// 从文件中移除对应的块
    bool EraseBlockFromDisk()
    {
        // Open history file
        CAutoFile fileout = OpenBlockFile(m_nFile, m_nBlockPos, "rb+");
        if (!fileout)
            return false;

		// 在文件对应的位置重新写一个空块，这样就相当于从文件中删除对应的当期块
        // Overwrite with empty null block
        CBlock block;
        block.SetNull();
        fileout << block;

        return true;
    }

	// 取前11个区块对应的创建时间平均值
    enum { nMedianTimeSpan=11 };

	// 从当前块往前推，获取过去对应的中位数时间，在对应的区块链中获取每一个区块对应的时间，然后取中位数
    int64 GetMedianTimePast() const
    {
        unsigned int pmedian[nMedianTimeSpan];
        unsigned int* pbegin = &pmedian[nMedianTimeSpan];
        unsigned int* pend = &pmedian[nMedianTimeSpan];

        const CBlockIndex* pindex = this;
        for (int i = 0; i < nMedianTimeSpan && pindex; i++, pindex = pindex->m_pPrevBlkIndex)
            *(--pbegin) = pindex->m_uTime;

        sort(pbegin, pend);
        return pbegin[(pend - pbegin)/2];
    }
	// 从当前块往后推，取中位数时间
    int64 GetMedianTime() const
    {
        const CBlockIndex* pindex = this;
        for (int i = 0; i < nMedianTimeSpan/2; i++)
        {
            if (!pindex->m_pNextBlkIndex)
                return m_uTime;
            pindex = pindex->m_pNextBlkIndex;
        }
        return pindex->GetMedianTimePast();
    }



    string ToString() const
    {
        return strprintf("CBlockIndex(nprev=%08x, m_pNextBlkIndex=%08x, m_nFile=%d, m_nBlockPos=%-6d m_nCurHeight=%d, merkle=%s, hashBlock=%s)",
            m_pPrevBlkIndex, m_pNextBlkIndex, m_nFile, m_nBlockPos, m_nCurHeight,
            m_hashMerkleRoot.ToString().substr(0,6).c_str(),
            GetBlockHash().ToString().substr(0,14).c_str());
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};



//
// Used to marshal pointers into hashes for db storage.
// 用于将指针替换成hash值用于数据库存储
//
class CDiskBlockIndex : public CBlockIndex
{
public:
    uint256 m_PrevHash; // block对应的hash值，将指针变成对应的hash
    uint256 m_NextHash;

    CDiskBlockIndex()
    {
        m_PrevHash = 0;
        m_NextHash = 0;
    }

    explicit CDiskBlockIndex(CBlockIndex* pindex) : CBlockIndex(*pindex)
    {
        m_PrevHash = (m_pPrevBlkIndex ? m_pPrevBlkIndex->GetBlockHash() : 0);
        m_NextHash = (m_pNextBlkIndex ? m_pNextBlkIndex->GetBlockHash() : 0);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(m_nCurVersion);

        READWRITE(m_NextHash);
        READWRITE(m_nFile);
        READWRITE(m_nBlockPos);
        READWRITE(m_nCurHeight);

        // block header
        READWRITE(this->m_nCurVersion);
        READWRITE(m_PrevHash);
    char fFromMe;
    char fSpent; // 是否花费交易标记
        READWRITE(m_hashMerkleRoot);
        READWRITE(m_uTime);
        READWRITE(m_uBits);
        READWRITE(m_uNonce);
    )

    uint256 GetBlockHash() const
    {
        CBlock block;
        block.m_nCurVersion        = m_nCurVersion;
        block.m_hashPrevBlock   = m_PrevHash;
        block.m_hashMerkleRoot  = m_hashMerkleRoot;
        block.m_uTime           = m_uTime;
        block.m_uBits           = m_uBits;
        block.m_uNonce          = m_uNonce;
        return block.GetHash(); // 块的hash仅仅包含上面这些内容
    }


    string ToString() const
    {
        string str = "CDiskBlockIndex(";
        str += CBlockIndex::ToString();
        str += strprintf("\n                hashBlock=%s, hashPrev=%s, hashNext=%s)",
            GetBlockHash().ToString().c_str(),
            m_PrevHash.ToString().substr(0,14).c_str(),
            m_NextHash.ToString().substr(0,14).c_str());
        return str;
    }

    void print() const
    {
        printf("%s\n", ToString().c_str());
    }
};








//
// Describes a place in the block chain to another node such that if the
// other node doesn't have the same branch, it can find a recent common trunk.
// The further back it is, the further before the fork it may be.
//
class CBlockLocator
{
protected:
    vector<uint256> vHave; // 区块链对应的block索引
public:

    CBlockLocator()
    {
    }

    explicit CBlockLocator(const CBlockIndex* pindex)
    {
        Set(pindex);
    }

    explicit CBlockLocator(uint256 hashBlock)
    {
        map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hashBlock);
        if (mi != mapBlockIndex.end())
            Set((*mi).second);
    }

    IMPLEMENT_SERIALIZE
    (
        if (!(nType & SER_GETHASH))
            READWRITE(nVersion);
        READWRITE(vHave);
    )

    void Set(const CBlockIndex* pindex)
    {
        vHave.clear();
        int nStep = 1;
        while (pindex)
        {
            vHave.push_back(pindex->GetBlockHash());

			// 指数快速回退算法：前10个保存，后面是指数回退一直到区块链头部为止
            // Exponentially larger steps back
            for (int i = 0; pindex && i < nStep; i++)
                pindex = pindex->m_pPrevBlkIndex;
            if (vHave.size() > 10)
                nStep *= 2;
        }
        vHave.push_back(hashGenesisBlock); // 默认放置一个创世区块
    }
	// 找到本地有的且在主链上的块的索引
    CBlockIndex* GetBlockIndex()
    {
        // Find the first block the caller has in the main chain
        foreach(const uint256& hash, vHave)
        {
			// 找到本地有的且在主链上的
            map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;
                if (pindex->IsInMainChain())
                    return pindex;
            }
        }
        return pindexGenesisBlock;
    }

    uint256 GetBlockHash()
    {
        // Find the first block the caller has in the main chain
        foreach(const uint256& hash, vHave)
        {
            map<uint256, CBlockIndex*>::iterator mi = mapBlockIndex.find(hash);
            if (mi != mapBlockIndex.end())
            {
                CBlockIndex* pindex = (*mi).second;
                if (pindex->IsInMainChain())
                    return hash;
            }
        }
        return hashGenesisBlock;
    }

    int GetHeight()
    {
        CBlockIndex* pindex = GetBlockIndex();
        if (!pindex)
            return 0;
        return pindex->m_nCurHeight;
    }
};












extern map<uint256, CTransaction> mapTransactions;
extern map<uint256, CWalletTx> mapWallet;
extern vector<pair<uint256, bool> > vWalletUpdated;
//extern CCriticalSection cs_mapWallet;
extern map<vector<unsigned char>, CPrivKey> mapKeys;
extern map<uint160, vector<unsigned char> > mapPubKeys;
//extern CCriticalSection cs_mapKeys;
extern CKey keyUser;
#endif 
/* EOF */
