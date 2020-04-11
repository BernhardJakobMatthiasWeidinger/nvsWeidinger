#include <iostream>

#include "base64.h"

#include "spdlog/spdlog.h"
#include "cpr/cpr.h"
#include "CLI11.hpp"

using namespace std;

struct Client {
    static string file;
    static string url;
    static bool g_flag;

    //Credentials
    static string username;
    static string password;
    static string encAuth;
    
    static void processResponse(cpr::Response r) {
        if (r.status_code == 200) {
            //if user called login
            if (r.url.find("/login") != string::npos) {
                spdlog::info("Successfully logged in");

                cpr::Session session;
                session.SetVerifySsl(false);
                session.SetOption(cpr::Url{"https://localhost:1234" + Client::url});
                session.SetOption(cpr::Header{{"Authorization", "Basic:" + Client::encAuth}});

                //if user called a GET request
                if (Client::g_flag) {
                    processResponse(session.Get());
                } else {
                    session.SetOption(cpr::Multipart{{"File", cpr::File{Client::file}}});
                    processResponse(session.Post());
                }
            } else {
                //if user called a GET request
                if (Client::g_flag) {
                    spdlog::info("Successfully received lines from server");
                    //print the lines of the server File
                    cout << r.text << endl;
                } else {
                    spdlog::info(r.text);
                }
            }
        } else if (r.status_code == 401) {
            spdlog::warn("Not authorized, redirect to /login");
            spdlog::info("User " + Client::username + " called GET /login");

            cpr::Session session;
            session.SetVerifySsl(false);
            session.SetOption(cpr::Url{"https://localhost:1234/login"});
            session.SetOption(cpr::Header{{"Authorization", "Basic:" + Client::encAuth}});
            processResponse(session.Get());
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
};

string Client::file{"../src/client/csvFiles/clientSeil.csv"};
string Client::url{};
bool Client::g_flag{};

string Client::username{};
string Client::password{};
string Client::encAuth{};

int main(int argc, char* argv[]) {
    CLI::App app{"CPR client for the Bergrettungsbeispiel"};

    app.add_option("--username", Client::username, "Username to log in")->required();
    app.add_option("--password", Client::password, "Password to log in")->required();

    //File for post requests
    app.add_option("--file", Client::file, "File, which will be processed at the server");

    //Method, which will be used
    bool p_flag{};
    app.add_option_group("method", "HTTP Methode")->require_option(-1);
    app.get_option_group("method")
        ->add_flag("-g,--get", Client::g_flag, "Use GET method (default)");
    app.get_option_group("method")
        ->add_flag("-p,--post", p_flag, "Use POST method");


    //Object, which will be processed
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

    //Number of objects, which will be posted
    int cnt{};
    app.add_option("--cnt", cnt, "Count of Inventarobjekte (default = all)");

    CLI11_PARSE(app, argc, argv);
    
    //Set Credentials for HTTP Basic Authentication
    string auth{Client::username + ':' + Client::password};
    Client::encAuth = base64_encode(reinterpret_cast<const unsigned char*>(
        auth.c_str()), auth.length());

    //Building the session
    cpr::Session session;
    session.SetVerifySsl(false);
    session.SetOption(cpr::Header{{"Authorization", "Basic:" + Client::encAuth}});

    //Building the url
    if (h_flag) {
        Client::url = "/hartware";
    } else if (f_flag) {
        Client::url = "/fahrzeug";
    } else if (i_flag) {
        Client::url = "/immobilie";
    } else {
        Client::url = "/seilware";
    }

    if (p_flag) {
        //if user only wants to post a specific count of objects to server
        if (cnt != 0) {
            Client::url += "/" + to_string(cnt);
        }

        spdlog::info("User " + Client::username + 
            " called POST " + Client::url);
        session.SetOption(cpr::Multipart{{"File", cpr::File{Client::file}}});
        session.SetOption(cpr::Url{"https://localhost:1234" + Client::url});
        auto r = session.Post();
        Client::processResponse(r);
    } else {
        spdlog::info("User " + Client::username + 
            " called GET " + Client::url);
        session.SetOption(cpr::Url{"https://localhost:1234" + Client::url});
        auto r = session.Get();
        Client::processResponse(r);
    } 
}