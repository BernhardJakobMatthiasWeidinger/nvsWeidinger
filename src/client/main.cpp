#include <iostream>

#include "base64.h"

#include "spdlog/spdlog.h"
#include "cpr/cpr.h"
#include "CLI11.hpp"

using namespace std;

void processResponse(cpr::Response r, string encAuth="") {
    if (r.status_code == 200) {
        spdlog::info("Successful Request");
        cout << r.text << endl;
    } else if (r.status_code == 401) {
        spdlog::warn("Not authorized, redirect to /login");

        r = cpr::Get(cpr::Url{"localhost:8080/login"},
            cpr::Header{{"Authorization", "Basic:" + encAuth}});

        processResponse(r, encAuth);
    } else if (r.status_code == 404) {
        spdlog::error("Resource / not found");
    } else if (r.status_code == 0) {
        spdlog::error("Server not reachable");
    } else {
        spdlog::error("Request failed with code: " + to_string(r.status_code));
    }
}

int main(int argc, char* argv[]) {
    CLI::App app{"CPR client for the Bergrettungsbeispiel"};

    //Credentials
    string username{};
    string password{};

    app.add_option("--username", username, "Username to log in")->required();
    app.add_option("--password", password, "Password to log in")->required();

    //Method, which will be used
    app.add_option_group("method", "HTTP Methode")->require_option(-1);

    bool g_flag{};
    app.get_option_group("method")
        ->add_flag("-g,--get", g_flag, "Use GET method (default)");
    bool p_flag{};
    app.get_option_group("method")
        ->add_flag("-p,--post", p_flag, "Use POST method");


    //Object, which will be used
    app.add_option_group("object", "Inventarobjekt")->require_option(-1);

    bool s_flag{};
    app.get_option_group("object")
        ->add_flag("-s,--seilware", s_flag, "Seilware (default)");
    bool h_flag{};
    app.get_option_group("object")
        ->add_flag("-w,--hartware", h_flag, "Hartware");
    bool f_flag{};
    app.get_option_group("object")
        ->add_flag("-f,--fahrzeug", f_flag, "Fahrzeug");
    bool i_flag{};
    app.get_option_group("object")
        ->add_flag("-i,--immobilie", i_flag, "Immobilie");

    CLI11_PARSE(app, argc, argv);

    string url{""};
    string encAuth{""};

    if (g_flag) {
        if (s_flag) {
            url = "/seilware";
        } else if (h_flag) {
            url = "/hartware";
        } else if (f_flag) {
            url = "/fahrzeug";
        } else if (i_flag) {
            url = "/immobilie";
        } else {
            spdlog::error("Invalid object chosen");
            exit(1);
        }

        const string auth{username + ':' + password};

        string encAuth = base64_encode(reinterpret_cast<const unsigned char*>(
            auth.c_str()), auth.length());

        auto r = cpr::Get(cpr::Url{"localhost:8080" + url},
                 cpr::Header{{"Authorization", "Basic:" + encAuth}});

        spdlog::info("User called GET " + url);
        processResponse(r, encAuth);
    } else if (p_flag) {
        //POST SHIT
    } else {
        spdlog::error("Invalid method chosen");
    }
}