#ifndef __MYSQL_CONNECTION_H
#define __MYSQL_CONNECTION_H

#ifdef _WIN32
#include <winsock2.h>
#endif

#include <mysql.h>
#include <queue>
#include "RibbitRequestData.h"
#include "Defines.h"

class MySQLConnection
{
public:
    static MySQLConnection* instance();

    void Open(const char* host, unsigned int port, const char* user, const char* pass, const char* db);
    void Close();

    bool RibbitEntryExists(std::string name, uint64 seqn, std::string flag);
    void AddNewRibbitEntry(std::string name, uint64 seqn, std::string flag);
    void UpdateRibbitSeqn(std::string name, uint64 oldSeqn, uint64 newSeqn, std::string flag);
    void MarkRibbitEntryDownloaded(std::string name, uint64 seqn, std::string flag);
    std::queue<RibbitRequestData> GetUndownloadedList();
private:
    MYSQL* _con;
};

#define sMySQLConnection MySQLConnection::instance()

#endif