#include <iostream>

#include "base64.h"

#include "spdlog/spdlog.h"
#include "cpr/cpr.h"

using namespace std;

int main() {
    //spdlog::info("Hello");

    auto r = cpr::Get(cpr::Url{"localhost:8080"},
                      cpr::Header{{"Authorization", "Basic: HALLO"}});
                      //cpr::Authentication{"user", "pass"});//,
                      //cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    //r.status_code;                  // 200
    //r.header["content-type"];       // application/json; charset=utf-8
    //r.text;                         // JSON text string

    if (r.status_code == 200) {
        spdlog::info("Successful Request");
        cout << r.text << endl;
    } else if (r.status_code == 401) {
        spdlog::info("Not authorized, redirect to /login");
    } else if (r.status_code == 0) {
        spdlog::error("Server not reachable");
    } else {
        spdlog::error("Request failed with code: " + to_string(r.status_code));
    }
}