
#include <sstream>
#include <fstream>
#include <iostream>
#include <regex>
#include "DiscordHook.h"
#include "Defines.h"

DiscordHook* DiscordHook::instance()
{
    static DiscordHook instance;
    return &instance;
}

void DiscordHook::Initialize()
{
    std::ifstream ifs("./discord.cfg");
    if (ifs.is_open())
    {
        ifs >> _user;
        ifs >> _discordWebhook;
        ifs.close();
    }
}

void DiscordHook::SendToDiscord(std::string title, std::string message)
{
    if (_discordWebhook.size() < 2)
        return;

    CURL* curl;
    ResponseData response = ResponseData();
    curl = curl_easy_init();
    if (curl)
    {
        message = std::regex_replace(message, std::regex("\n"), "\\n");
        std::stringstream ss;
        ss << "{ ";
        ss << "\"username\": \"" << _user << "\", ";
        ss << "\"embeds\": [ { \"title\": \"" << title << "\", \"description\": \"" << message << "\" } ] }";
        std::string content = ss.str();

        curl_easy_setopt(curl, CURLOPT_URL, _discordWebhook.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.length());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

        curl_slist* hs = nullptr;
        hs = curl_slist_append(hs, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

        response.CURLResponseCode = curl_easy_perform(curl);
        if (response.CURLResponseCode != CURLE_OK)
            fprintf(stderr, "DiscordHook::SendToDiscord: curl_easy_perform() failed: %s\n", curl_easy_strerror(response.CURLResponseCode));

        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &response.ElapsedTime);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.ResponseCode);

        curl_easy_cleanup(curl);
    }
}
