#include <fcgi_config.h>
#include <fcgiapp.h>
#include <json/json.h>

#include <chrono>
#include <sstream>
#include <string>

#include "authenticate.h"

static const std::string k_url_prefix = "/api/v1/";

static void send_message(FCGX_Stream *out, const std::string &message);
static void quit_with_message(FCGX_Stream *out, int code, const char *message);

int main(int argc, const char *argv[]) {
    Authenticate authenticate;

    FCGX_Stream *in = NULL;
    FCGX_Stream *out = NULL;
    FCGX_Stream *err = NULL;
    FCGX_ParamArray envp;
    while (FCGX_Accept(&in, &out, &err, &envp) >= 0) {
        //1 check content type only support application/json
        char *p_content_type = FCGX_GetParam("CONTENT_TYPE", envp);
        if (p_content_type == NULL) {
            quit_with_message(out, 403, "Empty content type");
            continue;
        }
        std::string content_type(p_content_type);
        if (content_type != "application/json") {
            quit_with_message(out, 403, "Wrong content type only support json");
            continue;
        }

        //2 check request method
        char *p_method = FCGX_GetParam("REQUEST_METHOD", envp);
        if (p_method == NULL) {
            quit_with_message(out, 403, "Empty request method");
            continue;
        }
        std::string method(p_method);
        if (method != "POST") {
            quit_with_message(out, 405, "Only support post method");
            continue;
        }

        //3 get content data
        char *p_content_length = FCGX_GetParam("CONTENT_LENGTH", envp);
        int content_length = 0;
        if (p_content_length != NULL) {
            content_length = strtol(p_content_length, NULL, 10);
        }
        std::stringstream content_string_stream;
        for (int i = 0; i < content_length; i++) {
            char ch = 0;
            if ((ch = FCGX_GetChar(in)) < 0) {
                quit_with_message(out, 400, "Error: Not enough bytes received");
                continue;
            }
            content_string_stream << ch;
        }

        std::string content_string = content_string_stream.str();

        //4 check request uri
        char *p_request_uri = FCGX_GetParam("REQUEST_URI", envp);
        if (p_request_uri == NULL) {
            quit_with_message(out, 400, "Empty URI");
            continue;
        }
        std::string request_uri(p_request_uri);
        if (request_uri == k_url_prefix + "info") {
            Json::Value root;
            root["version"] = "0.1.0";
            root["copyright"] = "copyright goertek 2023";
            Json::StyledWriter styled_writer;
            send_message(out, styled_writer.write(root));
        } else if (request_uri == k_url_prefix + "login") {
            Json::Value root;
            Json::CharReaderBuilder builder;
            std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            std::string errors;
            if (!reader->parse(content_string.c_str(),
                               content_string.c_str() + content_string.length(), &root, &errors)) {
                std::string build_string = "Failed parse json " + errors;
                quit_with_message(out, 400, build_string.c_str());
                continue;
            }

            Json::Value result;
            if (authenticate.login(root["username"].asString(), root["password"].asString())) {
                result["result"] = "OK";
                result["token"] = authenticate.generate_token();
            } else {
                quit_with_message(out, 403, "Authenticate failed");
                continue;
            }
            Json::StyledWriter styled_writer;
            send_message(out, styled_writer.write(result));
        } else {  // operation
            if (request_uri == k_url_prefix + "change_wifi_password") {
            } else {
                std::string build_string = "Invalid request " + request_uri;
                quit_with_message(out, 400, build_string.c_str());
                continue;
            }

            //check authenticate first
            Json::Value root;
            Json::CharReaderBuilder builder;
            std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
            std::string errors;
            if (!reader->parse(content_string.c_str(),
                               content_string.c_str() + content_string.length(), &root, &errors)) {
                std::string build_string = "Failed parse json " + errors;
                quit_with_message(out, 400, build_string.c_str());
                continue;
            }

            if (!authenticate.check_token(root["token"].asString())) {
                quit_with_message(out, 400, "Invalid token please login again");
                continue;
            }

            Json::Value result;
            result["result"] = "OK";
            Json::StyledWriter styled_writer;
            send_message(out, styled_writer.write(result));
        }
    }

    return EXIT_SUCCESS;
}

static void quit_with_message(FCGX_Stream *out, int code, const char *message) {
    FCGX_FPrintF(out, "Status: %d\r\n", code);
    FCGX_FPrintF(out, "Content-type: application/json\r\n\r\n");
    Json::Value root;
    root["message"] = message;
    Json::StyledWriter styled_writer;
    FCGX_FPrintF(out, "%s\r\n", styled_writer.write(root).c_str());
}

static void send_message(FCGX_Stream *out, const std::string &message) {
    FCGX_FPrintF(out, "Status: 200 OK\r\n");
    FCGX_FPrintF(out, "Content-type: application/json\r\n\r\n");

    FCGX_FPrintF(out, "%s\r\n", message.c_str());
}