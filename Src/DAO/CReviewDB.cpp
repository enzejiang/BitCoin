/*
 * =====================================================================================
 *
 *       Filename:  CReviewDB.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:47:44 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "key.h"
#include "market.h"
#include "CommonBase/uint256.h"
#include "DAO/CReviewDB.h"
//
// CReviewDB
//

namespace Enze
{
    bool CReviewDB::ReadReviews(const uint256& hash, vector<CReview>& vReviews)
    {
        printf("%s---%d\n", __FILE__, __LINE__);
        vReviews.size(); // msvc workaround, just need to do anything with vReviews
        //return Read(make_pair(string("reviews"), hash), vReviews);
    }

    bool CReviewDB::WriteReviews(const uint256&  hash, const vector<CReview>& vReviews)
    {
        printf("%s---%d\n", __FILE__, __LINE__);
        //return Write(make_pair(string("reviews"), hash), vReviews);
    }


}
/* EOF */

