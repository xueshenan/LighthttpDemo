#include <fcgi_config.h>
#include <fcgiapp.h>
#include <json/json.h>

#include <string>

static const std::string k_url_prefix = "/api/v1/";

static void send_message(FCGX_Stream *out, const std::string &message);
static void quit_with_message(FCGX_Stream *out, int code, const char *message);

int main(int argc, const char *argv[]) {
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

        //3 check request uri
        char *p_request_uri = FCGX_GetParam("REQUEST_URI", envp);
        if (p_request_uri == NULL) {
            quit_with_message(out, 400, "Empty URI");
            continue;
        }
        std::string request_uri(p_request_uri);
        if (request_uri == k_url_prefix + "info") {
            Json::Value root;
            root["version"] = "0.1.0";
            root["copyright"] = "copyright xx";
            Json::StyledWriter styled_writer;
            send_message(out, styled_writer.write(root));
        } else if (request_uri == k_url_prefix + "login") {
            Json::Value root;
            Json::StyledWriter styled_writer;
            send_message(out, styled_writer.write(root));
        } else {
            quit_with_message(out, 400, request_uri.c_str());
            continue;
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