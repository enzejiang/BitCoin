/*
 * =====================================================================================
 *
 *       Filename:  CommFunctionDef.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 04:04:54 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef CXX_BT_COMMON_FUNCTION_DEFINE_H
#define CXX_BT_COMMON_FUNCTION_DEFINE_H

class CTransaction;
class CScript;
class uint256;
bool EvalScript(const CScript& script, const CTransaction& txTo, unsigned int nIn, int nHashType=0,
                vector<vector<unsigned char> >* pvStackRet=NULL);
uint256 SignatureHash(CScript scriptCode, const CTransaction& txTo, unsigned int nIn, int nHashType);
bool IsMine(const CScript& scriptPubKey);
bool ExtractPubKey(const CScript& scriptPubKey, bool fMineOnly, vector<unsigned char>& vchPubKeyRet);
bool ExtractHash160(const CScript& scriptPubKey, uint160& hash160Ret);
bool SignSignature(const CTransaction& txFrom, CTransaction& txTo, unsigned int nIn, int nHashType=SIGHASH_ALL, CScript scriptPrereq=CScript());
bool VerifySignature(const CTransaction& txFrom, const CTransaction& txTo, unsigned int nIn, int nHashType=0);

#endif  /* CXX_BT_COMMON_FUNCTION_DEFINE_H */
/* EOF */

