#include "wifi_operation.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>

static std::string kHostapdConfigFile =
    "/Users/tbago/work/goertek/fcgi_server/build/hostapd.wlan0.conf";
static std::string kTempConfigFile = "/Users/tbago/work/goertek/fcgi_server/build/temp.conf";

static std::unordered_set<int> channel_set({
    1,    //* 2412 MHz [1] (20.0 dBm)
    2,    //* 2417 MHz [2] (20.0 dBm)
    3,    //* 2422 MHz [3] (20.0 dBm)
    4,    //* 2427 MHz [4] (20.0 dBm)
    5,    //* 2432 MHz [5] (20.0 dBm)
    6,    //* 2437 MHz [6] (20.0 dBm)
    7,    //* 2442 MHz [7] (20.0 dBm)
    8,    //* 2447 MHz [8] (20.0 dBm)
    9,    //* 2452 MHz [9] (20.0 dBm)
    10,   //* 2457 MHz [10] (20.0 dBm)
    11,   //* 2462 MHz [11] (20.0 dBm)
    12,   //* 2467 MHz [12] (20.0 dBm)
    13,   //* 2472 MHz [13] (20.0 dBm)
    36,   //* 5180 MHz [36] (20.0 dBm)
    40,   //* 5200 MHz [40] (20.0 dBm)
    44,   //* 5220 MHz [44] (20.0 dBm)
    48,   //* 5240 MHz [48] (20.0 dBm)
    52,   //* 5260 MHz [52] (20.0 dBm)
    56,   //* 5280 MHz [56] (20.0 dBm)
    60,   //* 5300 MHz [60] (20.0 dBm)
    64,   //* 5320 MHz [64] (20.0 dBm)
    100,  //* 5500 MHz [100] (20.0 dBm)
    104,  //* 5520 MHz [104] (20.0 dBm)
    108,  //* 5540 MHz [108] (20.0 dBm)
    112,  //* 5560 MHz [112] (20.0 dBm)
    116,  //* 5580 MHz [116] (20.0 dBm)
    120,  //* 5600 MHz [120] (20.0 dBm)
    124,  //* 5620 MHz [124] (20.0 dBm)
    128,  //* 5640 MHz [128] (20.0 dBm)
    132,  //* 5660 MHz [132] (20.0 dBm)
    136,  //* 5680 MHz [136] (20.0 dBm)
    140,  //* 5700 MHz [140] (20.0 dBm)
    144,  //* 5720 MHz [144] (20.0 dBm)
    149,  //* 5745 MHz [149] (20.0 dBm)
    153,  //* 5765 MHz [153] (20.0 dBm)
    157,  //* 5785 MHz [157] (20.0 dBm)
    161,  //* 5805 MHz [161] (20.0 dBm)
    165,  //* 5825 MHz [165] (20.0 dBm)
    169,  //* 5845 MHz [169] (20.0 dBm)
    173,  //* 5865 MHz [173] (20.0 dBm)
    177,  //* 5885 MHz [177] (20.0 dBm)
});

WifiOperation::Result WifiOperation::get_wifi_info(WifiInfo &wifi_info) {
    std::ifstream infile(kHostapdConfigFile);
    if (infile.fail()) {
        //File does not exist
        _last_result = Result::ResultFileNotExit;
        return _last_result;
    }

    std::string line;
    while (std::getline(infile, line)) {
        if (line.find("ssid=") == 0) {
            wifi_info.ssid = line.substr(std::string("ssid=").size());
        } else if (line.find("channel=") == 0) {
            wifi_info.channel = std::stoi(line.substr(std::string("channel=").size()));
        } else if (line.find("wpa_passphrase=") == 0) {
            wifi_info.password = line.substr(std::string("wpa_passphrase=").size());
        }
    }
    _last_result = Result::ResultOk;
    return _last_result;
}

WifiOperation::Result WifiOperation::set_wifi_ssid(std::string ssid) {
    std::stringstream ss;
    std::string key_pair = "ssid=";
    ss << key_pair << ssid;
    std::string content = ss.str();
    change_file_content(key_pair, content);
    _last_result = Result::ResultOk;
    return _last_result;
}

WifiOperation::Result WifiOperation::set_wifi_password(std::string password) {
    if (password.length() < 8) {
        _last_result = Result::ResultPasswordLength;
        return _last_result;
    }

    std::stringstream ss;
    std::string key_pair = "wpa_passphrase=";
    ss << key_pair << password;
    std::string content = ss.str();
    change_file_content(key_pair, content);
    _last_result = Result::ResultOk;
    return _last_result;
}

WifiOperation::Result WifiOperation::set_wifi_channel(int channel) {
    if (channel_set.count(channel) == 0) {
        _last_result = Result::ResultInvalidChannel;
        return _last_result;
    }

    std::stringstream ss;
    std::string key_pair = "channel=";
    ss << key_pair << channel;
    std::string content = ss.str();
    change_file_content(key_pair, content);

    _last_result = Result::ResultOk;
    return _last_result;
}

std::string WifiOperation::get_last_error() {
    switch (_last_result) {
        case Result::ResultOk:
            return "OK";
            break;
        case Result::ResultFileNotExit:
            return "File not exits";
            break;
        case Result::ResultPasswordLength:
            return "password length should be equal or greater than 8 character";
            break;
        case Result::ResultInvalidChannel:
            return "Invalid channel";
            break;
    }
}

WifiOperation::WifiOperation() {}

WifiOperation::~WifiOperation() {}

void WifiOperation::change_file_content(std::string &key_pair, std::string &new_content) {
    std::ifstream infile(kHostapdConfigFile);  // input file stream
    std::ofstream outfile(kTempConfigFile);    // output file stream
    std::string line;                          // string to store each line

    while (getline(infile, line))  // read each line
    {
        if (line.find(key_pair) == 0) {
            outfile << new_content << std::endl;
        } else {
            outfile << line << std::endl;  // write original line to output file
        }
    }

    infile.close();   // close input file
    outfile.close();  // close output file

    remove(kHostapdConfigFile.c_str());  // delete original file
    rename(kTempConfigFile.c_str(),
           kHostapdConfigFile.c_str());  // rename output file as original file
}
