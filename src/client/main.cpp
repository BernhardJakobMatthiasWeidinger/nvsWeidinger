#include <iostream>

#include "spdlog/spdlog.h"
#include "cpr/cpr.h"

using namespace std;

int main() {
    //spdlog::info("Hello");

    auto r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                      cpr::Authentication{"user", "pass"},
                      cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
}