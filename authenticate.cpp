#include "authenticate.h"

#include <jwt-cpp/jwt.h>

static const std::string db_username = "anxs";
static const std::string db_password = "111";

static const std::string jwt_issuer = "auth0";  //签发人
static const jwt::date jwt_date;
static const std::string jwt_secret_key = "94htqQcszgwtiarZ";

bool Authenticate::login(std::string username, std::string password) {
    if (username == db_username && password == db_password) {
        _already_login = true;
        return true;
    }
    return false;
}

std::string Authenticate::generate_token() {
    if (!_already_login) {
        return "";
    }
    auto token = jwt::create()
                     .set_issuer(jwt_issuer)
                     .set_type("JWT")
                     .set_issued_at(std::chrono::system_clock::now())
                     .set_payload_claim("username", jwt::claim(db_username))
                     .sign(jwt::algorithm::hs256{jwt_secret_key});
    return token;
}

bool Authenticate::check_token(std::string token) {
    if (!_already_login) {
        return false;
    }

    try {
        auto decoded_jwt = jwt::decode(token);

        auto issuer = decoded_jwt.get_issuer();
        auto jwt_date = decoded_jwt.get_issued_at();
        auto payload_claim = decoded_jwt.get_payload_claim("username");

        // if (!issuer.empty()) {
        //     auto verifier = jwt::verify()
        //                         .allow_algorithm(jwt::algorithm::rs256(jwt_secret_key, "", "", ""))
        //                         .with_issuer(jwt_issuer);

        //     verifier.verify(decoded_jwt);
        // }
        if (issuer == jwt_issuer && payload_claim.as_string() == db_username) {
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

bool Authenticate::change_password(std::string username, std::string origin_password,
                                   std::string new_password) {
    return false;
}

bool Authenticate::logout(std::string username) {
    _already_login = false;
    return true;
}