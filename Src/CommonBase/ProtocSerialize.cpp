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
#include "NetWorkService/CInv.h"
#include "BlockEngine/CDiskTxPos.h"
#include "BlockEngine/CDiskBlockIndex.h"
#include "BlockEngine/CBlock.h"
#include "BlockEngine/CBlockLocator.h"
#include "WalletService/CTxIndex.h"
#include "WalletService/CTxIn.h"
#include "WalletService/CTxOut.h"
#include "WalletService/CTransaction.h"
#include "WalletService/CWalletTx.h"
#include "CommonBase/market.h"
#include "ProtocSrc/Address.pb.h"
#include "ProtocSrc/DiskTxPos.pb.h"
#include "ProtocSrc/DiskBlockIndex.pb.h"
#include "ProtocSrc/TxIndex.pb.h"
#include "ProtocSrc/TxIn.pb.h"
#include "ProtocSrc/TxOut.pb.h"
#include "ProtocSrc/Transaction.pb.h"
#include "ProtocSrc/WalletTx.pb.h"
#include "ProtocSrc/Block.pb.h"
#include "ProtocSrc/Inventory.pb.h"
#include "ProtocSrc/Review.pb.h"
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
        if (!Enze::SeriaDiskTxPos(txindex.m_vSpent[i], *cProtoc.add_vspent()))
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

bool UnSeriaTxIn(TxIn& cProtoc, CTxIn& cTargetData)
{
   // cTargetData.m_cScriptSig;
    cTargetData.m_cScriptSig.clear();
   // CScript strScritp(cProtoc.cscriptsig().begin(), cProtoc.cscriptsig().end());
   // cTargetData.m_cScriptSig = strScritp;
#if 1
    foreach(char it, cProtoc.cscriptsig())
    {
        cTargetData.m_cScriptSig.push_back(it);
    }
#endif
//    printf("unSeriaTxIn--protoc_cScriptSig_size[%d], target scriptsz[%d]\n", cProtoc.cscriptsig().length(), cTargetData.m_cScriptSig.size());
    cTargetData.m_uSequence = cProtoc.usequence();
    return Enze::UnSeriaOutPoint(cProtoc.cprevout(), cTargetData.m_cPrevOut);

}

bool SeriaTxIn(const CTxIn& cSrcData, TxIn& cProtoc)
{
  //  printf("SeriaTxIn--start[%d]\n", cSrcData.m_cScriptSig.size());
    cProtoc.Clear();
    cProtoc.set_usequence(cSrcData.m_uSequence);

//    cSrcData.m_cScriptSig.PrintHex();
//    string strScript = HexStr(cSrcData.m_cScriptSig.begin(), cSrcData.m_cScriptSig.end(), false);
    string strScript(cSrcData.m_cScriptSig.begin(), cSrcData.m_cScriptSig.end());
    cProtoc.set_cscriptsig(strScript);
  //  printf("SeriaTxIn--cScriptSigSz[%d]--strScriptSz[%d]\n", cProtoc.cscriptsig().length(), strScript.length());
    return Enze::SeriaOutPoint(cSrcData.m_cPrevOut, *cProtoc.mutable_cprevout());
}

bool UnSeriaTxOut(TxOut& cProtoc, CTxOut& cTargetData)
{
    cTargetData.m_nValue = cProtoc.nvalue();
    cTargetData.m_cScriptPubKey.clear();
    //CScript strScritp(cProtoc.cscriptpubkey().begin(), cProtoc.cscriptpubkey().end());
    //cTargetData.m_cScriptPubKey = strScritp;
    foreach(char it, cProtoc.cscriptpubkey())
    {
        cTargetData.m_cScriptPubKey.push_back(it);
    }

//    printf("unSeriaTxOut--protoc_cScriptSig[%s] \n\n", cProtoc.cscriptpubkey().c_str());
//    cTargetData.m_cScriptPubKey.PrintHex();

    return true;
}

bool SeriaTxOut(const CTxOut& cSrcData, TxOut& cProtoc)
{
//    printf("SeriaTxOut--start\n");
    cProtoc.Clear();
    cProtoc.set_nvalue(cSrcData.m_nValue);
    string strScript(cSrcData.m_cScriptPubKey.begin(), cSrcData.m_cScriptPubKey.end());
    cProtoc.set_cscriptpubkey(strScript);
  //  printf("SeriaTxOut--cScriptSig[%s]--strScript[%s]\n", cProtoc.cscriptpubkey().c_str(), strScript.c_str());
  //  printf("SeriaTxOut--cScriptSig[%s]\n\n\n", cSrcData.m_cScriptPubKey.ToString().c_str());
 //   cSrcData.m_cScriptPubKey.PrintHex();

    return true;
}

bool UnSeriaTransaction(const Transaction& cProtoc, CTransaction& cTargetData)
{
    cTargetData.SetNull();
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
 //   printf("UnSeriaTransaction--cScriptSigsz[%d]\n\n\n", cTargetData.m_vTxIn[0].m_cScriptSig.size());
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
    printf("UnSeriaBlock--start\n");
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
    cTargetData.BuildMerkleTree();
    return true;
}
    
bool SeriaBlock(const CBlock& cSrcData, Block& cProtoc)
{
    printf("SeriaBlock-m_hashPrevBlock-[%s], m_hashMerkleRoot[%s]\n", cSrcData.m_hashPrevBlock.ToString().c_str(), cSrcData.m_hashMerkleRoot.ToString().c_str());
    cSrcData.BuildMerkleTree();
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


    printf("SeriaBlock-hashmerkleroot-[%s]\n", cProtoc.hashmerkleroot().c_str());

    printf("SeriaBlock---%d---%d\n", cSrcData.m_vTrans.size(), cProtoc.vtrans_size());
    return true;
}

bool UnSeriaInv(const Inventory& pbInv, CInv& cInv)
{
    cInv.type = pbInv.type();
    cInv.hash.SetHex(pbInv.hash());
    return true;
}

bool SeriaInv(const CInv&cInv, Inventory& pbInv)
{
    pbInv.set_type(cInv.type);
    pbInv.set_hash(cInv.hash.GetHex());
}
    
bool UnSeriaLoctor(const GetBlocks_BlockLocator& pbLoctor,CBlockLocator& bkLocator)
{
    bkLocator.vHave.clear();
    bkLocator.vHave.resize(pbLoctor.vhave_size());
    foreach (auto it, pbLoctor.vhave()) {
        bkLocator.vHave.push_back(uint256(it));
    }
    return true;
}

bool SeriaLoctor(const CBlockLocator&bkLocator, GetBlocks_BlockLocator& pbLoctor)
{
    pbLoctor.Clear();
    for (int i = 0; i < bkLocator.vHave.size(); ++i)
    {
        pbLoctor.set_vhave(i, bkLocator.vHave[i].GetHex());
    }
    return true;
}


bool UnSeriaReview(const Review& pbReview, CReview& cReview)
{
    cReview.nVersion = pbReview.nversion();
    cReview.nTime    = pbReview.ntime();
    cReview.nAtoms   = pbReview.natoms();
    cReview.hashTo.SetHex(pbReview.hashto());

    cReview.mapValue.clear();
    foreach(auto it, pbReview.mapvalue())
        cReview.mapValue.insert(it);

    cReview.vchPubKeyFrom.clear();
    foreach(auto it, pbReview.vchpubkeyfrom())
        cReview.vchPubKeyFrom.push_back(it);

    cReview.vchSig.clear();
    foreach(auto it, pbReview.vchsig())
        cReview.vchSig.push_back(it);

    return true;
}

bool SeriaReview(const CReview& cReview, Review& pbReview)
{
    pbReview.Clear();
    pbReview.set_nversion(cReview.nVersion);
    pbReview.set_natoms(cReview.nAtoms);
    pbReview.set_ntime(cReview.nTime);
    pbReview.set_hashto(cReview.hashTo.GetHex());
    pbReview.set_vchpubkeyfrom(string(cReview.vchPubKeyFrom.begin(), cReview.vchPubKeyFrom.end()));
    pbReview.set_vchsig(string(cReview.vchSig.begin(), cReview.vchSig.end()));

    ::google::protobuf::Map< ::std::string, ::std::string >* pMapValue = pbReview.mutable_mapvalue();
    foreach(auto it, cReview.mapValue)
        pMapValue->insert(google::protobuf::MapPair<string, string>(it.first, it.second));

    return true;
}
}// end namespace
/* EOF */

