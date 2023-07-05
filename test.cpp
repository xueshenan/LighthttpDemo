#include <json/json.h>

#include <iostream>
#include <string>

int main() {
    std::string content_string = "{\n    \"username\": \"anxs\",\n    \"password\": \"111\"\n}";

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    std::string errors;
    if (!reader->parse(content_string.c_str(), content_string.c_str() + content_string.length(),
                       &root, &errors)) {
        std::cerr << "Failed to parse JSON: " << errors << std::endl;
        return -1;
    }
    std::cout << root["password"] << std::endl;
    return 0;
}
