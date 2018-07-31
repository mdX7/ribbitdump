#include <string>
#include <iostream>
#include <thread>
#include <queue>
#include <fstream>
#include "Requester.h"
#include "MySQLConnection.h"
#include "DiscordHook.h"

int main()
{
    std::string host = "localhost";
    unsigned int port = 3306;
    std::string user = "root";
    std::string pass = "toor";
    std::string db = "blizzcdn";
    std::string downloadDir = "./";

    // Primitive config load lul
    std::ifstream ifs("./config.cfg");
    if (!ifs.is_open())
    {
        std::cout << "Could not open file './config.cfg'" << std::endl;
        return 1;
    }
    ifs >> host;
    ifs >> port;
    ifs >> user;
    ifs >> pass;
    ifs >> db;
    ifs >> downloadDir;
    ifs.close();
    MySQLConnection::instance()->Open(host.c_str(), port, user.c_str(), pass.c_str(), db.c_str());

    sDiscordHook->Initialize();
    while (true)
    {
        RibbitRequestData* cmd = new RibbitRequestData("summary", "", "", "", "");
        Requester req(downloadDir, "us.version.battle.net", "1119", cmd);
        delete cmd;

        std::queue<RibbitRequestData> queue = sMySQLConnection->GetUndownloadedList();
        while (queue.size() != 0)
        {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            cmd = &queue.front();
            cmd = new RibbitRequestData(cmd->Command, cmd->SubCommand, cmd->Product, cmd->Seqn, cmd->Flag);
            queue.pop();
            Requester r(downloadDir, "us.version.battle.net", "1119", cmd);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
