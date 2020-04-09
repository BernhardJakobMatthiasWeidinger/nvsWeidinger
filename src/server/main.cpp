#include <iostream>
#include <sstream>

#include "base64.h"
#include "restinio/all.hpp"
//#include "restinio/tls.hpp"

#include "materialverwaltung.h"

using namespace std;
using namespace restinio;

//vector of already logged in users, 
//example user: maxi:secret
vector<string> signedUsers{};


vector<Seilware> seilwaren{};
vector<Hartware> hartwaren{};
vector<Fahrzeug> fehrzeuge{};
vector<Immobilie> immobilien{};

using router_t = router::express_router_t<>;

//checks, if user is already logged in
bool checkAuth(request_handle_t req) {
    std::stringstream ss(req->header().get_field("Authorization"));
    std::string token;
    while (std::getline(ss, token, ':')) {}

    if (!count(signedUsers.begin(), signedUsers.end(), token)) {
        return false;
    }
    return true; 
}

//appends the user to the already logged in users
void login(request_handle_t req) {
    std::stringstream ss(req->header().get_field("Authorization"));
    std::string token;
    while (std::getline(ss, token, ':')) {}

    signedUsers.push_back(token);
}

auto server_handler() {
	auto router = std::make_unique< router_t >();

	//GET request for Homepage
	router->http_get("/", [](auto req, auto) {
        if (!checkAuth(req)) {
            return req->create_response(status_unauthorized()).done();
        }

		return req->create_response()
            .set_body("Successfully signed in!")
            .done();
	});

    //GET request for Homepage
	router->http_get("/login", [](auto req, auto) {
        login(req);

		return req->create_response()
            .set_body("Successfully logged in!")
            .done();
	});

    //get all seilwaren
    router->http_get("/seilware", [](auto req, auto) {
        if (!checkAuth(req)) {
            return req->create_response(status_unauthorized()).done();
        }

		return req->create_response()
            .set_body(seilwaren)
            .done();
	});

    //post new seilware
    router->http_post("/seilware", [](auto req, auto) {
        if (!checkAuth(req)) {
            return req->create_response(status_unauthorized()).done();
        }

		return req->create_response()
            .set_body("Successfully signed in!")
            .done();
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
			//restinio::single_thread_tls_traits_t<
            restinio::traits_t<
				asio_timer_manager_t,
				single_threaded_ostream_logger_t,
				router_t >;

        /*asio::ssl::context tls_context{ asio::ssl::context::sslv23 };
        tls_context.set_options(
        asio::ssl::context::default_workarounds |
        asio::ssl::context::no_sslv2 |
        asio::ssl::context::single_dh_use );

        string certs_dir{"."};

        tls_context.use_certificate_chain_file( certs_dir + "/server.pem" );
        tls_context.use_private_key_file(
            certs_dir + "/key.pem",
            asio::ssl::context::pem );
        tls_context.use_tmp_dh_file( certs_dir + "/dh2048.pem" );*/

        run(on_this_thread<traits_t>()
            .port(8080).address("localhost")
            .request_handler(server_handler()));
            //.tls_context( std::move(tls_context)));
            //.request_handler(handler));
    } catch( const std::exception & ex ) {
		cerr << "Error: " << ex.what() << endl;
		return 1;
	}
   
}