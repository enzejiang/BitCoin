/*
 * =====================================================================================
 *
 *       Filename:  CWalletTx.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 05:36:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CXX_BT_CWALLETTX_H
#define CXX_BT_CWALLETTX_H
#include "Db/CWalletDB.h"
#include "Db/CTxDB.h"
#include "TX/CMerkleTx.h"
#include "serialize.h"
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
  //  bool AcceptTransaction();
};





#endif /* CXX_BT_CWALLETTX_H */
/* EOF */

