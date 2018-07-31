#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "curl/curl.h"
#include <string>
#include <set>

enum CustomResponseCodes
{
    RESPONSE_NOT_FOUND = 404,

    RESPONSE_ALREADY_EXISTS = -1,
};

struct ResponseData
{
    double ElapsedTime;
    uint64_t ResponseCode;
    std::string ResponseString;
    CURLcode CURLResponseCode;
};

class DiscordHook
{
public:
    DiscordHook() { }
    ~DiscordHook() { }

    void Initialize();
    void SendToDiscord(std::string title, std::string message);
    static DiscordHook* instance();
private:
    std::string _discordWebhook;
    std::string _user;
};

#define sDiscordHook DiscordHook::instance()

#endif