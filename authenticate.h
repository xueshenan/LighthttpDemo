#pragma once

#include <string>

// only support single user for now

class Authenticate final {
public:
    bool login(std::string username, std::string password);
    std::string generate_token();
    bool check_token(std::string token);
    bool change_password(std::string username, std::string origin_password,
                         std::string new_password);
    bool logout(std::string username);
public:
    Authenticate() = default;
    ~Authenticate() = default;
    Authenticate &operator=(const Authenticate &) = delete;
    Authenticate(const Authenticate &) = delete;
private:
    bool _already_login{false};
};
