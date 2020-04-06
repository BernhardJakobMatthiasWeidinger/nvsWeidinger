#include <iostream>

#include "spdlog/spdlog.h"
#include "cpr/cpr.h"

using namespace std;

int main() {
    //spdlog::info("Hello");

    auto r = cpr::Get(cpr::Url{"localhost:8080"},
                      cpr::Authentication{"user", "pass"},
                      cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    //r.status_code;                  // 200
    //r.header["content-type"];       // application/json; charset=utf-8
    //r.text;                         // JSON text string

    if (r.status_code == 200) {
        spdlog::info("Successful Request");
        cout << r.text << endl;
    } else {
        spdlog::error("Request failed");
        cerr << r.status_code << endl;
    }
}