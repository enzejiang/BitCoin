/*
 * =====================================================================================
 *
 *       Filename:  CDB.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/02/2018 07:37:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  enze (), 767201935@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef EZ_BT_CDB_H
#define EZ_BT_CDB_H
#include <db_cxx.h>
#include <google/protobuf/message.h>
#include "CommonBase/headers.h"
#include "CommonBase/CommFunctionDef.h"
namespace Enze 
{

    class CDB
    {
        protected:
            Db* m_pDb;
            explicit CDB(DbEnv*pDbenv, const char* pszFile, const char* pszMode="crw+", bool fTxn=false);
            virtual ~CDB();// { Close(); }
        public:
            void Close();
        private:
            CDB(const CDB&);
            CDB operator=(const CDB&);
            static bool m_bDbEnvInit;
            static map<string, int> mapFileUseCount;
        public:
            bool Erase(DbTxn*txn, const string& key);
            bool Exists(const string& key);
            bool ReadVersion(int& nVersion);
            bool WriteVersion(DbTxn* txn,int nVersion);
        protected:
            bool Read(const string& key, ::google::protobuf::Message& cProtoData);
            bool Read(const string& key, string& value);
            bool Write(DbTxn*txn, const string& key, const ::google::protobuf::Message& value, bool fOverwrite=true);
            bool Write(DbTxn* txn, const string&key, const string& value, bool fOverwrite = true);
            int ReadAtCursor(Dbc* pcursor, Dbt& datKey, Dbt& datValue, unsigned int fFlags=DB_NEXT );
            Dbc* GetCursor();
    };


}
#endif /* EZ_BT_CDB_H */
/* EOF */

