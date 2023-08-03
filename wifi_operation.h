#pragma once
#include <string>

struct WifiInfo {
    std::string ssid;
    std::string password;
    int channel;
};

class WifiOperation final {
public:
    enum class Result {
        ResultOk,
        ResultFileNotExit,
        ResultPasswordLength,
        ResultInvalidChannel,
    };
public:
    WifiOperation::Result get_wifi_info(WifiInfo &wifi_info);
    WifiOperation::Result set_wifi_ssid(std::string ssid);
    WifiOperation::Result set_wifi_password(std::string password);
    WifiOperation::Result set_wifi_channel(int channel);
    std::string get_last_error();
public:
    WifiOperation();
    ~WifiOperation();
private:
    void change_file_content(std::string &key_pair, std::string &new_content);
private:
    Result _last_result;
};