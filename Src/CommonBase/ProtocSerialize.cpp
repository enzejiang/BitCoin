/*
 * =====================================================================================
 *
 *       Filename:  ProtocSerialize.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/14/2018 09:21:00 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "NetWorkService/CAddress.h"
#include "BlockEngine/CDiskTxPos.h"
#include "BlockEngine/CDiskBlockIndex.h"
#include "BlockEngine/CBlock.h"
#include "WalletService/CTxIndex.h"
#include "WalletService/CTxIn.h"
#include "WalletService/CTxOut.h"
#include "WalletService/CTransaction.h"
#include "WalletService/CWalletTx.h"
#include "ProtocSrc/Address.pb.h"
#include "ProtocSrc/DiskTxPos.pb.h"
#include "ProtocSrc/DiskBlockIndex.pb.h"
#include "ProtocSrc/TxIndex.pb.h"
#include "ProtocSrc/TxIn.pb.h"
#include "ProtocSrc/TxOut.pb.h"
#include "ProtocSrc/Transaction.pb.h"
#include "ProtocSrc/WalletTx.pb.h"
#include "ProtocSrc/Block.pb.h"
#include "ProtocSerialize.h"

namespace Enze
{
bool UnSeriaAddress(const Address& cProtoc, CAddress& addr)
{
    addr.ip = cProtoc.ip();
    addr.m_nServices = cProtoc.nservices();
    addr.nTime = cProtoc.ntime();
    addr.port  = cProtoc.port();
    return true;
}

bool SeriaAddress(const CAddress& addr, Address& cProtoc)
{
    cProtoc.Clear();
    cProtoc.set_ip(addr.ip) ; 
    cProtoc.set_nservices(addr.m_nServices);
    cProtoc.set_ntime(addr.nTime);
    cProtoc.set_port(addr.port);
    return true;
}

bool UnSeriaDiskTxPos(const DiskTxPos& cProtoc, CDiskTxPos& pos)
{ 
    pos.m_nFile = cProtoc.nfile();
    pos.m_nBlockPos = cProtoc.nblockpos();
    pos.m_nTxPos    = cProtoc.ntxpos();
    return true;
}

bool SeriaDiskTxPos(const CDiskTxPos& pos, DiskTxPos& cProtoc)
{
    cProtoc.Clear();
    cProtoc.set_nfile(pos.m_nFile);
    cProtoc.set_nblockpos(pos.m_nBlockPos);
    cProtoc.set_ntxpos(pos.m_nTxPos);
    return true;
}
    
bool UnSeriaTxIndex(const TxIndex& cProtoc, CTxIndex& txindex)
{
    if (!Enze::UnSeriaDiskTxPos(cProtoc.cdiskpos(), txindex.m_cDiskPos))
        return false;
    

    foreach(auto it, cProtoc.vspent())
    {
        CDiskTxPos cData;
        if (Enze::UnSeriaDiskTxPos(it, cData))
            txindex.m_vSpent.push_back(cData);
        else 
            return false;
    }
      
    return true;
}

bool SeriaTxIndex(const CTxIndex& txindex, TxIndex& cProtoc)
{
    cProtoc.Clear();
    if (!Enze::SeriaDiskTxPos(txindex.m_cDiskPos, *cProtoc.mutable_cdiskpos() ))
        return false;
    
    for(int i=0;i <txindex.m_vSpent.size(); ++i)
    {
        if (!Enze::SeriaDiskTxPos(txindex.m_vSpent[i], *cProtoc.mutable_vspent(i)))
            return false;
    }
    
    return true;
}
    
bool UnSeriaDiskBlockIndex(const DiskBlockIndex& cProtocData, CDiskBlockIndex& blockindex)
{
    blockindex.m_NextHash.SetHex(cProtocData.hashnext());
    blockindex.m_PrevHash.SetHex(cProtocData.hashprev());
    blockindex.m_nFile = cProtocData.nfile();
    blockindex.m_nBlockPos = cProtocData.nblockpos();
    blockindex.m_uBits  = cProtocData.nbit();
    blockindex.m_uTime  = cProtocData.ntime();
    blockindex.m_uNonce = cProtocData.nnonce();
    blockindex.m_nCurHeight = cProtocData.nheight();
    blockindex.m_nCurVersion = cProtocData.nversion();
    blockindex.m_hashMerkleRoot.SetHex(cProtocData.hashmerkleroot());
    return true;
}

bool SeriaDiskBlockIndex(const CDiskBlockIndex& blockindex, DiskBlockIndex& cProtocData)
{
    cProtocData.Clear();
    cProtocData.set_hashnext(blockindex.m_NextHash.ToString());
    cProtocData.set_hashprev(blockindex.m_PrevHash.ToString());
    cProtocData.set_nfile(blockindex.m_nFile);
    cProtocData.set_nblockpos(blockindex.m_nBlockPos);
    cProtocData.set_nbit(blockindex.m_uBits);
    cProtocData.set_ntime(blockindex.m_uTime);
    cProtocData.set_nversion(blockindex.m_nCurVersion);
    cProtocData.set_nheight(blockindex.m_nCurHeight);
    cProtocData.set_nnonce(blockindex.m_uNonce);
    cProtocData.set_hashmerkleroot(blockindex.m_hashMerkleRoot.ToString());
    return true;
}

bool UnSeriaOutPoint(const OutPoint& cProtoc, COutPoint& cTargetData)
{
    cTargetData.m_nIndex = cProtoc.index();
    cTargetData.m_u256Hash.SetHex(cProtoc.u256hash());
    return true;
}

bool SeriaOutPoint(const COutPoint& cSrcData, OutPoint& cProtoc)
{
    cProtoc.Clear();
    cProtoc.set_index(cSrcData.m_nIndex);
    cProtoc.set_u256hash(cSrcData.m_u256Hash.ToString());
    return true;
}

bool UnSeriaTxIn(const TxIn& cProtoc, CTxIn& cTargetData)
{
   // cTargetData.m_cScriptSig;
    cTargetData.m_cScriptSig.clear();
    foreach(auto it, cProtoc.cscriptsig())
    {
        cTargetData.m_cScriptSig.push_back(it);
    }

    cTargetData.m_uSequence = cProtoc.usequence();
    return Enze::UnSeriaOutPoint(cProtoc.cprevout(), cTargetData.m_cPrevOut);

}

bool SeriaTxIn(const CTxIn& cSrcData, TxIn& cProtoc)
{
    cProtoc.Clear();
    cProtoc.set_usequence(cSrcData.m_uSequence);
    cProtoc.set_cscriptsig(cSrcData.m_cScriptSig.ToString());
    return Enze::SeriaOutPoint(cSrcData.m_cPrevOut, *cProtoc.mutable_cprevout());
}

bool UnSeriaTxOut(const TxOut& cProtoc, CTxOut& cTargetData)
{
    cTargetData.m_nValue = cProtoc.nvalue();
    cTargetData.m_cScriptPubKey.clear();
    foreach(auto it, cProtoc.cscriptpubkey())
    {
        cTargetData.m_cScriptPubKey.push_back(it);
    }

    return true;
}

bool SeriaTxOut(const CTxOut& cSrcData, TxOut& cProtoc)
{
    cProtoc.Clear();
    cProtoc.set_nvalue(cSrcData.m_nValue);
    cProtoc.set_cscriptpubkey(cSrcData.m_cScriptPubKey.ToString());
    return true;
}

bool UnSeriaTransaction(const Transaction& cProtoc, CTransaction& cTargetData)
{
    cTargetData.m_nCurVersion = cProtoc.ncurversion();
    cTargetData.m_nLockTime   = cProtoc.nlocktime();
    cTargetData.m_vTxIn.clear();
    cTargetData.m_vTxOut.clear();
    foreach(auto it, cProtoc.vtxin())
    {
        CTxIn txin;
        if (!Enze::UnSeriaTxIn(it, txin))
            return false;

        cTargetData.m_vTxIn.push_back(txin);
    }

    foreach(auto it, cProtoc.vtxout())
    {
        CTxOut txout;
        if(!Enze::UnSeriaTxOut(it, txout))
            return false;
        cTargetData.m_vTxOut.push_back(txout);
    }

    return  true;
}

bool SeriaTransaction(const CTransaction& cSrcData, Transaction& cProtoc)
{
    cProtoc.Clear();
    cProtoc.set_ncurversion(cSrcData.m_nCurVersion);
    cProtoc.set_nlocktime(cSrcData.m_nLockTime);
    foreach(auto it, cSrcData.m_vTxIn)
    {
        if(!Enze::SeriaTxIn(it,*cProtoc.add_vtxin()))
            return false;
    }
    foreach(auto it, cSrcData.m_vTxOut)
    {
        if(!Enze::SeriaTxOut(it, *cProtoc.add_vtxout()))
            return false;
    }

    return true;
}

bool UnSeriaMerkleTx(const MerkleTx& cProtoc, CMerkleTx& cTargetData)
{
    cTargetData.m_nCurVersion = cProtoc.ncurversion();
    cTargetData.m_nLockTime   = cProtoc.nlocktime();
    cTargetData.m_nIndex      = cProtoc.nindex();
    cTargetData.m_hashBlock.SetHex(cProtoc.hashblock());
    cTargetData.m_vTxIn.clear();
    cTargetData.m_vTxOut.clear();
    cTargetData.m_vMerkleBranch.clear();
    foreach(auto it, cProtoc.vtxin())
    {
        CTxIn txin;
        if (!Enze::UnSeriaTxIn(it, txin))
            return false;

        cTargetData.m_vTxIn.push_back(txin);
    }

    foreach(auto it, cProtoc.vtxout())
    {
        CTxOut txout;
        if(!Enze::UnSeriaTxOut(it, txout))
            return false;
        cTargetData.m_vTxOut.push_back(txout);
    }

    foreach(auto it, cProtoc.vmerklebranch())
    {
        uint256 merkHash(it);
        cTargetData.m_vMerkleBranch.push_back(merkHash);
    }

    return true;
}

bool SeriaMerkleTx(const CMerkleTx& cSrcData, MerkleTx& cProtoc)
{
    cProtoc.Clear();
    cProtoc.set_ncurversion(cSrcData.m_nCurVersion);
    cProtoc.set_nlocktime(cSrcData.m_nLockTime);
    cProtoc.set_nindex(cSrcData.m_nIndex);
    cProtoc.set_hashblock(cSrcData.m_hashBlock.ToString());
    foreach(auto it, cSrcData.m_vTxIn)
    {
        if(!Enze::SeriaTxIn(it,*cProtoc.add_vtxin()))
            return false;
    }
    foreach(auto it, cSrcData.m_vTxOut)
    {
        if(!Enze::SeriaTxOut(it, *cProtoc.add_vtxout()))
            return false;
    }

    foreach(auto it, cSrcData.m_vMerkleBranch)
    {
        cProtoc.add_vmerklebranch(it.ToString());
    }
}

bool UnSeriaOrderForm(const OrderForm& cProtoc, map<string, string>& cTargetData)
{
   // map<string, string>
    cTargetData.clear();
    cTargetData.insert(cProtoc.maporder().begin(), cProtoc.maporder().end());
    return true;
}

bool SeriaOrderForm(const map<string, string>& cSrcData, OrderForm& cProtoc)
{
    cProtoc.Clear();
    cProtoc.mutable_maporder()->insert(cSrcData.begin(), cSrcData.end());
    return true;
}

bool UnSeriaWalletTx(const WalletTx& cProtoc, CWalletTx& cTargetData)
{
    cTargetData.m_nCurVersion = cProtoc.ncurversion();
    cTargetData.m_nLockTime   = cProtoc.nlocktime();
    cTargetData.m_nIndex      = cProtoc.nindex();
    cTargetData.m_hashBlock.SetHex(cProtoc.hashblock());
    cTargetData.m_bFromMe = cProtoc.bfromme();
    cTargetData.m_bSpent  = cProtoc.bspent();
    cTargetData.m_uTimeReceived = cProtoc.utimereceived();
    cTargetData.m_bTimeReceivedIsTxTime = cProtoc.btimereceivedistxtime();

    cTargetData.m_vTxIn.clear();
    cTargetData.m_vTxOut.clear();
    cTargetData.m_vMerkleBranch.clear();
    cTargetData.m_vPrevTx.clear();
    cTargetData.m_vOrderForm.clear();
    cTargetData.m_mapValue.clear();

    cTargetData.m_mapValue.insert(cProtoc.mapvalue().begin(), cProtoc.mapvalue().end());


    foreach(auto it, cProtoc.vtxin())
    {
        CTxIn txin;
        if (!Enze::UnSeriaTxIn(it, txin))
            return false;

        cTargetData.m_vTxIn.push_back(txin);
    }

    foreach(auto it, cProtoc.vtxout())
    {
        CTxOut txout;
        if(!Enze::UnSeriaTxOut(it, txout))
            return false;
        cTargetData.m_vTxOut.push_back(txout);
    }

    foreach(auto it, cProtoc.vmerklebranch())
    {
        uint256 merkHash(it);
        cTargetData.m_vMerkleBranch.push_back(merkHash);
    }

    foreach(auto it, cProtoc.vprevtx())
    {
        CMerkleTx cMkTx;
        if(!Enze::UnSeriaMerkleTx(it, cMkTx))
            return false;
        cTargetData.m_vPrevTx.push_back(cMkTx);
    }

    return true;
//    cTargetData.m_vOrderForm.resize(cProtoc.orderlist_size());
//    for(int i = 0; i< cProtoc.orderlist_size(); ++i)
//   // foreach(auto it, cProtoc.orderlist())
//    {
//        const OrderForm& cProtocOder = cProtoc.orderlist(i);
//        cTargetData.m_vOrderForm[i] = pair<string,string>(cProtocOder.maporder());
//    }
}

bool SeriaWalletTx(const CWalletTx& wtx, WalletTx& cProtoData)
{
    cProtoData.Clear();

    cProtoData.set_ncurversion(wtx.m_nCurVersion);
    cProtoData.set_nindex(wtx.m_nIndex);
    cProtoData.set_bfromme(wtx.m_bFromMe);
    cProtoData.set_bspent(wtx.m_bSpent);
    cProtoData.set_nlocktime(wtx.m_nLockTime);
    cProtoData.set_utimereceived(wtx.m_uTimeReceived);
    cProtoData.set_btimereceivedistxtime(wtx.m_bTimeReceivedIsTxTime);
    cProtoData.set_hashblock(wtx.m_hashBlock.ToString());
    foreach(auto it, wtx.m_vTxIn)
    {
        if(!Enze::SeriaTxIn(it,*cProtoData.add_vtxin()))
            return false;
    }
    foreach(auto it, wtx.m_vTxOut)
    {
        if(!Enze::SeriaTxOut(it, *cProtoData.add_vtxout()))
            return false;
    }

    foreach(auto it, wtx.m_vMerkleBranch)
    {
        cProtoData.add_vmerklebranch(it.ToString());
    }

    foreach(auto it, wtx.m_vPrevTx)
    {
        if (!Enze::SeriaMerkleTx(it, *cProtoData.add_vprevtx()))
            return false;
    }

    cProtoData.mutable_mapvalue()->insert(wtx.m_mapValue.begin(), wtx.m_mapValue.end());
    return true;
    map<string, string> mapOrder;
    foreach(auto it, wtx.m_vOrderForm)
    {
        mapOrder.insert(it);
    }

    return Enze::SeriaOrderForm(mapOrder, *cProtoData.add_orderlist());
}
    
bool UnSeriaBlock(const Block& cProtoc, CBlock& cTargetData)
{
    cTargetData.m_nCurVersion = cProtoc.ncurversion();
    cTargetData.m_uBits = cProtoc.ubits();
    cTargetData.m_uNonce = cProtoc.unonce();
    cTargetData.m_uTime  = cProtoc.utime();
    cTargetData.m_hashMerkleRoot.SetHex(cProtoc.hashmerkleroot());
    cTargetData.m_hashPrevBlock.SetHex(cProtoc.hashprevblock());
    cTargetData.m_vTrans.clear();
    foreach(auto it, cProtoc.vtrans())
    {
        CTransaction tx;
        if (!UnSeriaTransaction(it, tx))
            return false;
        cTargetData.m_vTrans.push_back(tx);
    }
    return true;
}
    
bool SeriaBlock(const CBlock& cSrcData, Block& cProtoc)
{
    cProtoc.Clear();
    cProtoc.set_ncurversion(cSrcData.m_nCurVersion);
    cProtoc.set_ubits(cSrcData.m_uBits);
    cProtoc.set_unonce(cSrcData.m_uNonce);
    cProtoc.set_utime(cSrcData.m_uTime);
    cProtoc.set_hashmerkleroot(cSrcData.m_hashMerkleRoot.ToString());
    cProtoc.set_hashprevblock(cSrcData.m_hashPrevBlock.ToString());
    foreach(auto it, cSrcData.m_vTrans)
    {
        if(!SeriaTransaction(it,*cProtoc.add_vtrans()))
            return false;
    }
    printf("SeriaBlock---%d---%d\n", cSrcData.m_vTrans.size(), cProtoc.vtrans_size());
    return true;
}

    
}// end namespace
/* EOF */

