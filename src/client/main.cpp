#include <iostream>

#include "base64.h"

#include "spdlog/spdlog.h"
#include "cpr/cpr.h"
#include "CLI11.hpp"

using namespace std;

string file{"../src/client/csvFiles/clientSeil.csv"};
string url{""};
bool p_flag{};

void processResponse(cpr::Response r, string encAuth="") {
    if (r.status_code == 200) {
        //if user called login
        if (r.url.find("/login") != string::npos) {
            spdlog::info("Successfully logged in");

            cpr::Session session;
            session.SetVerifySsl(false);
            session.SetOption(cpr::Url{"https://localhost:1234" + url});
            session.SetOption(cpr::Header{{"Authorization", "Basic:" + encAuth}});

            //if user called a POST request
            if (p_flag) {
                session.SetOption(cpr::Multipart{{"File", cpr::File{file}}});
                processResponse(session.Post(), encAuth);
            } else {
                processResponse(session.Get(), encAuth);
            }
        } else {
            //if user called a GET request
            if (r.text.find(",") != string::npos) {
                spdlog::info("Successfully received lines from server");
                //print the lines of the server File
                cout << r.text << endl;
            } else {
                spdlog::info(r.text);
            }
        }
    } else if (r.status_code == 401) {
        spdlog::warn("Not authorized, redirect to /login");

        cpr::Session session;
        session.SetVerifySsl(false);
        session.SetOption(cpr::Url{"https://localhost:1234/login"});
        session.SetOption(cpr::Header{{"Authorization", "Basic:" + encAuth}});

        processResponse(session.Get(), encAuth);
    } else if (r.status_code == 404) {
        spdlog::error("Resource / not found");
    } else if (r.status_code == 409) {
        spdlog::error("Given file has errors (too many or incorrect values)");
    } else if (r.status_code == 0) {
        spdlog::error("Unknown error: (status_code = 0)!");
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

    //File for post requests
    app.add_option("--file", file, "File, which will be processed at the server");

    //Method, which will be used
    app.add_option_group("method", "HTTP Methode")->require_option(-1);

    bool g_flag{};
    app.get_option_group("method")
        ->add_flag("-g,--get", g_flag, "Use GET method (default)");
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

    int cnt{};
    app.add_option("--cnt", cnt, "Count of Inventarobjekte (default = all)");

    CLI11_PARSE(app, argc, argv);
    
    string auth{username + ':' + password};
    string encAuth = base64_encode(reinterpret_cast<const unsigned char*>(
        auth.c_str()), auth.length());

    cpr::Session session;
    session.SetVerifySsl(false);
    session.SetOption(cpr::Header{{"Authorization", "Basic:" + encAuth}});

    if (p_flag) {
        if (h_flag) {
            url = "/hartware";
        } else if (f_flag) {
            url = "/fahrzeug";
        } else if (i_flag) {
            url = "/immobilie";
        } else {
            url = "/seilware";
        }

        if (cnt != 0) {
            url += "/" + to_string(cnt);
        }

        spdlog::info("User called POST " + url);
        session.SetOption(cpr::Multipart{{"File", cpr::File{file}}});
        session.SetOption(cpr::Url{"https://localhost:1234" + url});
        auto r = session.Post();
        processResponse(r, encAuth);
    } else {
        if (h_flag) {
            url = "/hartware";
        } else if (f_flag) {
            url = "/fahrzeug";
        } else if (i_flag) {
            url = "/immobilie";
        } else {
            url = "/seilware";
        }

        spdlog::info("User called GET " + url);
        session.SetOption(cpr::Url{"https://localhost:1234" + url});
        auto r = session.Get();
        processResponse(r, encAuth);
    } 
}