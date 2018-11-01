/*
 * =====================================================================================
 *
 *       Filename:  ProtocSerialize.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/14/2018 09:34:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EN_BT_PROTOC_SERIAL_H
#define EN_BT_PROTOC_SERIAL_H
#include <map>
#include "ProtocSrc/GetBlocks.pb.h"
using namespace std;
namespace Enze
{
    class CAddress;
    class Address;
    class CDiskTxPos;
    class DiskTxPos;
    class CTxIndex;
    class TxIndex;
    class CDiskBlockIndex;
    class DiskBlockIndex;
    class OutPoint;
    class COutPoint;
    class TxIn;
    class CTxIn;
    class TxOut;
    class CTxOut;
    class Transaction;
    class CTransaction;
    class MerkleTx;
    class CMerkleTx;
    class WalletTx;
    class CWalletTx;
    class OrderForm;
    class Block;
    class CBlock;
    class CInv;
    class Inventory;
    class CBlockLocator;
    class CReview;
    class Review;
    bool UnSeriaAddress(const Address& cProtoc, CAddress& addr);
    bool SeriaAddress(const CAddress& addr, Address& cProtoc);
    
    bool UnSeriaDiskTxPos(const DiskTxPos& cProtoc, CDiskTxPos& pos);
    bool SeriaDiskTxPos(const CDiskTxPos& pos, DiskTxPos& cProtoc);
    
    bool UnSeriaTxIndex(const TxIndex& cProtoc, CTxIndex& txindex);
    bool SeriaTxIndex(const CTxIndex& txIndex, TxIndex& cProtoc);
    
    bool UnSeriaDiskBlockIndex(const DiskBlockIndex& cProtoc, CDiskBlockIndex& bindex);
    bool SeriaDiskBlockIndex(const CDiskBlockIndex& bIndex, DiskBlockIndex& cProtoc);

    bool UnSeriaOutPoint(const OutPoint& cProtoc, COutPoint& cTargetData);
    bool SeriaOutPoint(const COutPoint& cSrcData, OutPoint& cProtoc);
    bool UnSeriaTxIn( TxIn& cProtoc, CTxIn& cTargetData);
    bool SeriaTxIn(const CTxIn& cSrcData, TxIn& cProtoc);

    bool UnSeriaTxOut( TxOut& cProtoc, CTxOut& cTargetData);
    bool SeriaTxOut(const CTxOut& cSrcData, TxOut& cProtoc);

    bool UnSeriaTransaction(const Transaction& cProtoc, CTransaction& cTargetData);
    bool SeriaTransaction(const CTransaction& cSrcData, Transaction& cProtoc);

    bool UnSeriaMerkleTx(const MerkleTx& cProtoc, CMerkleTx& cTargetData);
    bool SeriaMerkleTx(const CMerkleTx& cSrcData, MerkleTx& cProtoc);
    
    bool UnSeriaWalletTx(const WalletTx& cProtoc, CWalletTx& cTargetData);
    bool SeriaWalletTx(const CWalletTx& cSrcData, WalletTx& cProtoc);

    bool UnSeriaOrderForm(const OrderForm& cProtoc, map<string, string>& cTargetData);
    bool SeriaOrderForm(const map<string, string>& cSrcData, OrderForm& cProtoc);
    
    bool UnSeriaBlock(const Block& cProtoc, CBlock& cTargetData);
    bool SeriaBlock(const CBlock& cSrcData, Block& cProtoc);

    bool UnSeriaInv(const Inventory& pbInv, CInv& cInv);
    bool SeriaInv(const CInv&cInv, Inventory& pbInv);

    bool UnSeriaLoctor(const GetBlocks_BlockLocator& pbLoctor,CBlockLocator& bkLoctor);
    bool SeriaLoctor(const CBlockLocator&bkLockor, GetBlocks_BlockLocator& pbLoctor);

    bool UnSeriaReview(const Review& pbReview, CReview& cReview);
    bool SeriaReview(const CReview& cReview, Review& pbReview);
}



#endif 
/* EOF */

