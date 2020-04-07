#include <iostream>

#include "base64.h"

#include "spdlog/spdlog.h"
#include "cpr/cpr.h"

using namespace std;

int main() {
    //spdlog::info("Hello");

    string encAuth{""};

    while (true) {
        auto r = cpr::Get(cpr::Url{"localhost:8080"},
                 cpr::Header{{"Authorization", "Basic:" + encAuth}});
                      //cpr::Authentication{"user", "pass"});//,
                      //cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    //r.status_code;                  // 200
    //r.header["content-type"];       // application/json; charset=utf-8
    //r.text;                         // JSON text string

        if (r.status_code == 200) {
            spdlog::info("Successful Request");
            cout << r.text << endl;
            return 0;
        } else if (r.status_code == 401) {
            spdlog::warn("Not authorized, redirect to login field");
            cout << "You are not logged in yet!" << endl;

            string username;
            string password;

            cout << "Username: ";
            cin >> username;
            cout << endl;

            cout << "Password: ";
            cin >> password;
            cout << endl;

            const string auth{username + ':' + password};

            encAuth = base64_encode(reinterpret_cast<const unsigned char*>(
                auth.c_str()), auth.length());
        } else if (r.status_code == 404) {
            spdlog::error("Resource / not found");
        } else if (r.status_code == 0) {
            spdlog::error("Server not reachable");
        } else {
            spdlog::error("Request failed with code: " + to_string(r.status_code));
        }
    }
}