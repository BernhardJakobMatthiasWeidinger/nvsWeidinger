#include <iostream>
#include <sstream>
#include <fstream>

#include "base64.h"
#include "pystring.h"
#include "restinio/all.hpp"
#include <restinio/helpers/multipart_body.hpp>
#include <restinio/helpers/file_upload.hpp>
#include "restinio/tls.hpp"

#include "materialverwaltung.h"

using namespace std;
using namespace restinio;
using namespace restinio::file_upload;
using router_t = router::express_router_t<>;

//vector of already logged in users, 
vector<string> signedUsers{};
//path, where all server files are located 
string path = "../src/server/csvFiles/";

//checks, if user is already logged in
bool checkAuth(request_handle_t req) {
    if (!(req->header().has_field("Authorization"))) {
        return false;
    }

    std::stringstream ss(req->header().get_field("Authorization"));
    std::string token;
    while (std::getline(ss, token, ':')) {}

    if (!count(signedUsers.begin(), signedUsers.end(), token)) {
        return false;
    }
    return true; 
}

//appends the user to the vector of logged in users
void login(request_handle_t req) {
    std::stringstream ss(req->header().get_field("Authorization"));
    std::string token;
    while (std::getline(ss, token, ':')) {}

    signedUsers.push_back(token);
}

//returns all lines of specified file if authorized
auto on_get(request_handle_t req, string filename) {
    if (!checkAuth(req)) {
        return req->create_response(status_unauthorized()).done();
    }

	return req->create_response()
        .set_body(restinio::sendfile(path + "server" + filename + ".csv"))
        .done();
}

//checks if file for posted Seilwaren are correct
bool on_post_seil(vector<string> tokens, string filename="") {
    if (tokens.size() != 7) {
        return false;
    }

    if (!(tokens[0] == "true" || tokens[0] == "false")) {
        return false;
    }

    if (!pystring::isdigit(tokens[4])) {
        return false;
    }

    if (!(tokens[5] == "Kletterseil" || 
        tokens[5] == "StatischesLastseil" || 
        tokens[5] == "Canyoningseil" || 
        tokens[5] == "Reepschnur" || 
        tokens[5] == "Bandmaterial")) {
        return false;
    }

    if (!(tokens[6] == "nie" || 
        tokens[6] == "selten" || 
        tokens[6] == "gelegentlich" || 
        tokens[6] == "regelmäßig" || 
        tokens[6] == "häufig" || 
        tokens[6] == "ständig")) {
        return false;
    }


    return true;
}

//checks if file for posted Hartwaren are correct
bool on_post_hart(vector<string> tokens, string filename="") {
    if (tokens.size() != 6) {
        return false;
    }

    if (!(tokens[0] == "true" || tokens[0] == "false")) {
        return false;
    }

    if (!pystring::isdigit(tokens[4])) {
        return false;
    }

    if (!(tokens[5] == "Stahlschraubkarabiner" || 
        tokens[5] == "Aluschraubkarabiner" || 
        tokens[5] == "HMSKarabiner" || 
        tokens[5] == "Stahlseil" || 
        tokens[5] == "Abseilachter" || 
        tokens[5] == "Tuber" || 
        tokens[5] == "Trage" || 
        tokens[5] == "ZubehörStahlseil" || 
        tokens[5] == "SeilwindeStahlseil" || 
        tokens[5] == "SeilwindePerlon" || 
        tokens[5] == "Funkgerät")) {
        return false;
    }

    return true;
}

//checks if file for posted Wartungsobjekte are correct
bool on_post_wartung(vector<string> tokens, string filename) {
    if (tokens.size() != 6) {
        return false;
    }

    if (!(tokens[0] == "true" || tokens[0] == "false")) {
        return false;
    }
    
    ifstream file;
    file.open(path + "server" + filename + ".csv");
    if (file.fail()) {
        return false;
    }

    string line{};
    while (getline(file, line)) {
        std::stringstream ss(line);
        string token{};
        int cnt{};

        while (getline(ss, token, ',')) {
            if (cnt == 3) {
                if (token == tokens[3]) {
                    return false;
                }
            }
            cnt++;
        }
    }

    if (!pystring::isdigit(tokens[4])) {
        return false;
    }

    return true;
}

//appends all lines from client file to server file if callback returns true
auto on_post(request_handle_t req, string filename, string objectName,
             function<bool(vector<string>, string)> callback, string_view_t cnt) {
    if (!checkAuth(req)) {
        return req->create_response(status_unauthorized()).done();
    }

    bool valid{true};
    vector<string> lines;

    //checks are receives lines from client
    const auto result = enumerate_parts_with_files(*req,
        [&](part_description_t part) {
        pystring::split(string(part.body), lines, "\n");

        for (string line : lines) {
            vector<string> tokens;
            pystring::split(line, tokens, ",");

            valid = callback(tokens, filename);
            
            if (!valid) {
                break;
            }
        }

        return handling_result_t::continue_enumeration;
    });

    //if client file is valid add to server
    if (valid) {
        ofstream file;
        file.open(path + "server" + filename + ".csv", ios_base::app);

        for (uint i{}; i < lines.size(); ++i) {
            if (cnt != "0" && i == stoul(string(cnt))) {
                break;
            }
            file << "\n" << lines[i];
        }
        file.close();

        //Send response with code 200 back
        return req->create_response()
            .set_body("Successfully posted " + objectName + "!")
            .done();
    } else {
        return req->create_response(status_conflict()).done();
    }
}

//express server, which handles the routes
auto server_handler() {
	auto router = std::make_unique< router_t >();

    //GET request for Homepage
	router->http_get("/login", [](auto req, auto) {
        login(req);

		return req->create_response().done();
	});

    //get all seilwaren
    router->http_get("/seilware", [](auto req, auto) {
        return on_get(req, "Seil");
	});

    //post new seilware
    router->http_post("/seilware", [](auto req, auto) {
        return on_post(req, "Seil", "Seilware(n)", on_post_seil, "0");
	});

    //post new seilware
    router->http_post("/seilware/:count", [](auto req, auto params) {
        return on_post(req, "Seil", "Seilware(n)", on_post_seil, params["count"]);
	});

    //get all hartwaren
    router->http_get("/hartware", [](auto req, auto) {
        return on_get(req, "Hart");
	});

    //post new hartwaren
    router->http_post("/hartware", [](auto req, auto) {
        return on_post(req, "Hart", "Hartware(n)", on_post_hart, "0");
	});

    //post new hartwaren
    router->http_post("/hartware/:count", [](auto req, auto params) {
        return on_post(req, "Hart", "Hartware(n)", on_post_hart, params["count"]);
	});

    //get all fahrzeuge
    router->http_get("/fahrzeug", [](auto req, auto) {
        return on_get(req, "Fahr");
	});

    //post new fahrzeuge
    router->http_post("/fahrzeug", [](auto req, auto) {
        return on_post(req, "Fahr", "Fahrzeug(e)", on_post_wartung, "0");
	});

    //post new fahrzeuge
    router->http_post("/fahrzeug/:count", [](auto req, auto params) {
        return on_post(req, "Fahr", "Fahrzeug(e)", on_post_wartung, params["count"]);
	});

    //get all immobilien
    router->http_get("/immobilie", [](auto req, auto) {
        return on_get(req, "Immobilie");
	});

    //post new immobilien
    router->http_post("/immobilie", [](auto req, auto) {
        return on_post(req, "Immobilie", "Immobilie(n)", on_post_wartung, "0");
	});

    //post new immobilien
    router->http_post("/immobilie/:count", [](auto req, auto params) {
        return on_post(req, "Immobilie", "Immobilie(n)", on_post_wartung, params["count"]);
	});

    
    //will be called, if route doesn't match exisitng ones
	router->non_matched_request_handler(
		[]( auto req ){
			return
				req->create_response(status_not_found())
					.append_header_date_field()
					.connection_close()
					.done();
		});

	return router;
}

int main() {
    try {
        using traits_t =
			restinio::tls_traits_t<
				asio_timer_manager_t,
				single_threaded_ostream_logger_t,
				router_t >;

        asio::ssl::context tls_context{ asio::ssl::context::sslv23 };
        tls_context.set_options(
        asio::ssl::context::default_workarounds |
        asio::ssl::context::no_sslv2 |
        asio::ssl::context::single_dh_use );

        string certs_dir{"../src/server/certs"};

        tls_context.use_certificate_chain_file(certs_dir + "/server.pem" );
        tls_context.use_private_key_file(
            certs_dir + "/key.pem",
            asio::ssl::context::pem );
        tls_context.use_tmp_dh_file(certs_dir + "/dh2048.pem" );

        run(on_this_thread<traits_t>()
            .port(1234).address("localhost")
            .request_handler(server_handler())
            .read_next_http_message_timelimit( 10s )
				.write_http_response_timelimit( 1s )
				.handle_request_timeout( 1s )
            .tls_context( std::move(tls_context)));
    } catch( const std::exception & ex ) {
		cerr << "Error: " << ex.what() << endl;
		return 1;
	}
   
}