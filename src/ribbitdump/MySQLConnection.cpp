#include <iostream>
#include <sstream>
#include "MySQLConnection.h"
#include "DiscordHook.h"

MySQLConnection* MySQLConnection::instance()
{
    static MySQLConnection instance;
    return &instance;
}

void MySQLConnection::Open(const char* host, unsigned int port, const char* user, const char* pass, const char* db)
{
    _con = mysql_init(nullptr);

    if (_con == nullptr)
    {
        std::cout << "MySQLConnection::Open|mysql_init: Error occured: " << mysql_error(_con) << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "MySQLConnection::Open|mysql_init: An error occured! Check console for more info!");
        exit(1);
    }

    if (mysql_real_connect(_con, host, user, pass, db, port, nullptr, 0) == nullptr)
    {
        std::cout << "MySQLConnection::Open|mysql_real_connect: Error occured: " << mysql_error(_con) << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "MySQLConnection::Open|mysql_real_connect: An error occured! Check console for more info!");
        mysql_close(_con);
        exit(1);
    }
}

void MySQLConnection::Close()
{
    mysql_close(_con);
}

bool MySQLConnection::RibbitEntryExists(std::string name, uint64 seqn, std::string flag)
{
    std::stringstream builder;
    builder << "SELECT * FROM `ribbit` WHERE"
        << " `Name`='" << name << "' AND"
        << " `Seqn`=" << seqn << " AND"
        << " `Flag`='" << flag << "'";

    if (mysql_query(_con, builder.str().c_str()))
    {
        std::cout << "MySQLConnection::RibbitEntryExists|mysql_query: Error occured: " << mysql_error(_con) << std::endl;
        std::cout << "    Used Query: " << builder.str() << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "MySQLConnection::RibbitEntryExists|mysql_query: An error occured! Check console for more info!");
    }

    MYSQL_RES* result = mysql_store_result(_con);
    if (result == nullptr)
    {
        std::cout << "MySQLConnection::RibbitEntryExists|mysql_store_result: Error occured: " << mysql_error(_con) << std::endl;
        std::cout << "    Used Query: " << builder.str() << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "MySQLConnection::RibbitEntryExists|mysql_store_result: An error occured! Check console for more info!");
    }

    return mysql_num_rows(result) != 0;
}

void MySQLConnection::AddNewRibbitEntry(std::string name, uint64 seqn, std::string flag)
{
    if (RibbitEntryExists(name, seqn, flag))
    {
        // std::cout << "MySQLConnection::AddNewRibbitEntry|RibbitEntryExists: " << name << ", " << seqn << ", " << flag << std::endl;
        return;
    }

    std::stringstream builder;
    builder << "INSERT INTO `ribbit` (`Name`, `Seqn`, `Flag`) VALUES "
            << "('" << name << "'," << seqn << ",'" << flag << "')";
    if (mysql_query(_con, builder.str().c_str()))
    {
        std::cout << "MySQLConnection::AddNewRibbitEntry|mysql_query: Error occured: " << mysql_error(_con) << std::endl;
        std::cout << "    Used Query: " << builder.str() << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "MySQLConnection::AddNewRibbitEntry|mysql_query: An error occured! Check console for more info!");
    }
}

void MySQLConnection::MarkRibbitEntryDownloaded(std::string name, uint64 seqn, std::string flag)
{
    std::stringstream builder;
    builder << "UPDATE `ribbit` SET "
        << "`Downloaded`=1, `TimeDownloaded`=CURRENT_TIMESTAMP()"
        << " WHERE"
        << " `Name`='" << name << "' AND"
        << " `Seqn`=" << seqn << " AND"
        << " `Flag`='" << (flag == "version" ? "" : flag) << "'";

    std::stringstream discText;
    discText << "**" << name << "**\nSeqn: " << seqn << "\n" << flag;

    if (mysql_query(_con, builder.str().c_str()))
    {
        std::cout << "MySQLConnection::MarkRibbitEntryDownloaded|mysql_query: Error occured: " << mysql_error(_con) << std::endl;
        std::cout << "    Used Query: " << builder.str() << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "MySQLConnection::MarkRibbitEntryDownloaded|mysql_query: An error occured! Check console for more info!");
    }
    sDiscordHook->SendToDiscord("Ribbit Update", discText.str());
}

std::queue<RibbitRequestData> MySQLConnection::GetUndownloadedList()
{
    std::queue<RibbitRequestData> q;
    std::stringstream builder;
    builder << "SELECT `Name`, `Seqn`, `Flag` FROM `ribbit` WHERE `Downloaded`=0 AND `Seqn`<>0";

    if (mysql_query(_con, builder.str().c_str()))
    {
        std::cout << "MySQLConnection::GetUndownloadedList|mysql_query: Error occured: " << mysql_error(_con) << std::endl;
        std::cout << "    Used Query: " << builder.str() << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "MySQLConnection::GetUndownloadedList|mysql_query: An error occured! Check console for more info!");
    }

    MYSQL_RES* result = mysql_store_result(_con);
    if (result == nullptr)
    {
        std::cout << "MySQLConnection::GetUndownloadedList|mysql_query: Error occured: " << mysql_error(_con) << std::endl;
        std::cout << "    Used Query: " << builder.str() << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "MySQLConnection::GetUndownloadedList|mysql_query: An error occured! Check console for more info!");
    }

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result)))
    {
        RibbitRequestData d;
        for (int i = 0; i < num_fields; i++)
        {
            switch (i)
            {
                case 0:
                    d.Product = row[i];
                    break;
                case 1:
                    d.Seqn = row[i];
                    break;
                case 2:
                    d.Flag = row[i];
                    break;
                default:
                    break;
            }
        }

        d.Command = "products";
        if (d.Flag == "")
        {
            d.Flag = "version";
            d.SubCommand = "versions";
        }
        else if (d.Flag == "cdn")
        {
            d.SubCommand = "cdns";
        }
        else if (d.Flag == "bgdl")
        {
            d.SubCommand = d.Flag;
        }
        q.push(d);
    }
    return q;
}