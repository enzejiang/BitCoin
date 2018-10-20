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

#ifndef EZ_BT_CREVIEWDB_H
#define EZ_BT_CREVIEWDB_H
#include "DAO/CDB.h"
#include "CommonBase/uint256.h"
#include "CommonBase/market.h"
namespace Enze
{
    class CUser;
    class CReview;
    
    class CReviewDB : public CDB
    {
    public:
        CReviewDB(DbEnv*pDbenv = NULL, const char* pszMode="crw+", bool fTxn=false) : CDB(pDbenv, "reviews.dat", pszMode, fTxn) { }
        
        bool ReadUser(const uint256& hash, CUser& user)
        {
            printf("%s---%d\n", __FILE__, __LINE__);
            //return Read(make_pair(string("user"), hash), user);
        }

        bool WriteUser(const uint256& hash, const CUser& user)
        {
            printf("%s---%d\n", __FILE__, __LINE__); 
            //return Write(make_pair(string("user"), hash), user);
        }

        bool ReadReviews(const uint256& hash, vector<CReview>& vReviews);
        bool WriteReviews(const uint256& hash, const vector<CReview>& vReviews);

    private:
        CReviewDB(const CReviewDB&);
        CReviewDB operator=(const CReviewDB&);
    };


}

#endif /* EZ_BT_CREVIEWDB_H */
/* EOF */

