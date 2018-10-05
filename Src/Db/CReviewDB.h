/*
 * =====================================================================================
 *
 *       Filename:  CReviewDB.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:45:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CXX_BT_CREVIEWDB_H
#define CXX_BT_CREVIEWDB_H
#include "Db/CDB.h"
class CUser;
class uint256;
class CReview;
class CReviewDB : public CDB
{
public:
    CReviewDB(const char* pszMode="r+", bool fTxn=false) : CDB("reviews.dat", pszMode, fTxn) { }
    
    bool ReadUser(const uint256& hash, CUser& user)
    {
        return Read(make_pair(string("user"), hash), user);
    }

    bool WriteUser(const uint256& hash, const CUser& user)
    {
        return Write(make_pair(string("user"), hash), user);
    }

    bool ReadReviews(const uint256& hash, vector<CReview>& vReviews);
    bool WriteReviews(const uint256& hash, const vector<CReview>& vReviews);

private:
    CReviewDB(const CReviewDB&);
    void operator=(const CReviewDB&);
};


#endif /* CXX_BT_CREVIEWDB_H */
/* EOF */

