#ifndef __REQUESTER_H
#define __REQUESTER_H

#include <queue>
#include <boost/asio.hpp>
#include "RibbitRequestData.h"
#include "Defines.h"

class Requester
{
public:
    Requester(std::string downloadDir, std::string host, std::string port, RibbitRequestData* cmd);
    ~Requester();

    uint64 GetSeqn(std::stringstream& dataStream);
    void ReadHandler(boost::system::error_code const& ec, std::size_t transferredBytes);
    void ConnectHandler(boost::system::error_code const& ec);
    void ResolveHandler(boost::system::error_code const& ec, boost::asio::ip::tcp::resolver::iterator it);
    void SendCommand(RibbitRequestData& request);
    uint64 ParseSummary(std::stringstream& dataStream);
private:
    boost::asio::io_service _ioservice;
    boost::asio::ip::tcp::resolver _resolver{ _ioservice };
    boost::asio::ip::tcp::socket _socket{ _ioservice };
    std::array<char, 4096> _data;
    RibbitRequestData* _currentCommand;

    std::stringstream _currentPacket;
    std::string _host;
    std::string _port;
    std::string _downloadDir;
};

#endif