#include "Requester.h"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include "MySQLConnection.h"
#include "DiscordHook.h"

#define RIBBIT_VERSION "v1"

void Requester::SendCommand(RibbitRequestData& request)
{
    std::stringstream ss;
    ss << RIBBIT_VERSION << "/" << request.Command;
    if (request.Product != "")
        ss << "/" << request.Product;
    if (request.SubCommand != "")
        ss << "/" << request.SubCommand;
    ss << "\r\n";
    boost::asio::write(_socket, boost::asio::buffer(ss.str()));
}

Requester::~Requester()
{
    _socket.close();
    _ioservice.stop();
}

Requester::Requester(std::string downloadDir, std::string host, std::string port, RibbitRequestData* cmd) : _downloadDir(downloadDir), _host(host), _port(port), _currentCommand(cmd)
{
    boost::asio::ip::tcp::resolver::query qry(host, port);
    _resolver.async_resolve(qry, boost::bind(&Requester::ResolveHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::iterator));
    _ioservice.run();
}

uint32 Requester::ParseSummary(std::stringstream& dataStream)
{
    uint64 seqn = -1;
    int dataRowNum = -1;
    std::string row;
    int startRowOfData = 0;
    int whitespaceLines = 0;
    std::vector<std::string> colNames;
    for (int rowNum = 0; getline(dataStream, row, '\n'); rowNum++)
    {
        if (row.find("!STRING:") != std::string::npos)
            startRowOfData = rowNum;

        if (startRowOfData == 0)
            continue;

        if (row.size() < 2)
            break;

        if (row[0] != '#')
        {
            std::stringstream tokens(std::move(row));
            std::string token = "";

            // Parse header
            if (startRowOfData == rowNum)
            {
                for (int colNum = 0; getline(tokens, token, '|'); colNum++)
                    colNames.push_back(token.substr(0, token.find('!')));
            }
            else
            {
                // Parse data rows
                RibbitRequestData reqDat;
                for (int colNum = 0; getline(tokens, token, '|'); colNum++)
                {
                    token.erase(std::remove(token.begin(), token.end(), '\r'), token.end());
                    if (colNames[colNum] == "Product")
                        reqDat.Product = token;
                    else if (colNames[colNum] == "Seqn")
                        reqDat.Seqn = token;
                    else if (colNames[colNum] == "Flags")
                        reqDat.Flag = token;
                    else
                    {
                        std::cout << "Requester::ParseSummary: DETECTED NEW COLUMN TYPE: '" << colNames[colNum] << "' PLEASE IMPLEMENT ME ASAP **" << std::endl;
                        sDiscordHook->SendToDiscord("Ribbit ERROR", "**Requester::ParseSummary: DETECTED NEW COLUMN TYPE: '" + colNames[colNum] + "' PLEASE IMPLEMENT ME ASAP **");
                    }
                }
                sMySQLConnection->AddNewRibbitEntry(reqDat.Product, std::stoull(reqDat.Seqn), reqDat.Flag);
            }
        }

        // Get seqn
        // ## seqn = 32451
        if (row[0] == '#' && row[3] == 's')
        {
            seqn = std::stoull(row.c_str() + 2 + 1 + 4 + 1 + 1 + 1);
            dataRowNum = rowNum + 1;
        }
    }
    return seqn;
}

void Requester::ReadHandler(boost::system::error_code const& ec, std::size_t transferredBytes)
{
    // if (_currentCommand->Command == "summary")
    //     std::cout << "Received: " << transferredBytes << " bytes. " << std::endl;
    _currentPacket.write(_data.data(), transferredBytes);

    if (transferredBytes == 0 && _currentPacket.str().size() > 10)
    {
        std::string seqn = "";
        if (_currentCommand->Command == "summary")
            seqn = std::to_string(ParseSummary(_currentPacket));
        else
            seqn = _currentCommand->Seqn;

        std::stringstream pathStream;
        pathStream << _downloadDir << _currentCommand->Command;
        if (_currentCommand->Product != "")
            pathStream << "/" << _currentCommand->Product;
        pathStream << "/";

        boost::filesystem::path dir(pathStream.str());
        if (boost::filesystem::create_directories(pathStream.str()))
            std::cout << "Created directories: " << pathStream.str() << std::endl;

        if (_currentCommand->Flag != "")
            pathStream << _currentCommand->Flag << "-";

        if (_currentCommand->Product != "")
            pathStream << _currentCommand->Product;
        else
            pathStream << _currentCommand->Command << "-#";
        pathStream << "-" << seqn << ".bmime";

        std::ofstream ofs(pathStream.str(), std::ios::binary);
        ofs << _currentPacket.str();
        ofs.close();
        if (_currentCommand->Product != "")
            sMySQLConnection->MarkRibbitEntryDownloaded(_currentCommand->Product, std::stoull(_currentCommand->Seqn), _currentCommand->Flag);

        _socket.close();
    }
    if (ec)
        return;

    // proceed if there is moar data
    _socket.async_read_some(boost::asio::buffer(_data), boost::bind(&Requester::ReadHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Requester::ConnectHandler(boost::system::error_code const& ec)
{
    if (ec)
    {
        std::cout << "Requester::ConnectHandler: An error occured: " << ec << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "Requester::ConnectHandler: An error occured! Check console for more info!");
        return;
    }

    SendCommand(*_currentCommand);
    _socket.async_read_some(boost::asio::buffer(_data), boost::bind(&Requester::ReadHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Requester::ResolveHandler(boost::system::error_code const& ec, boost::asio::ip::tcp::resolver::iterator it)
{
    if (ec)
    {
        std::cout << "Requester::ResolveHandler: An error occured: " << ec << std::endl;
        sDiscordHook->SendToDiscord("Ribbit ERROR", "Requester::ResolveHandler: An error occured! Check console for more info!");
        return;
    }
    _socket.async_connect(*it, boost::bind(&Requester::ConnectHandler, this, boost::asio::placeholders::error));
}
