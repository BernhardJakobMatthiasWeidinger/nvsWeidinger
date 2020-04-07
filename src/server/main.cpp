#include <iostream>
#include <sstream>

#include "base64.h"
#include "restinio/all.hpp"

using namespace std;
using namespace restinio;

//vector of already logged in users, 
//example user: maxi:secret
vector<string> signedUsers = {"bWF4aTpzZWNyZXQ="};

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

auto server_handler() {
	auto router = std::make_unique< router_t >();

	//GET request for Homepage
	router->http_get("/", []( auto req, auto) {
        if (!checkAuth(req)) {
            return req->create_response(status_unauthorized()).done();
        }

		return req->create_response()
            .set_body("Successfully signed in!")
            .done();
	});
    
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
			traits_t<
				asio_timer_manager_t,
				single_threaded_ostream_logger_t,
				router_t >;

        run(on_this_thread<traits_t>()
            .port(8080).address("localhost")
            .request_handler(server_handler()));
            //.request_handler(handler));
    } catch( const std::exception & ex ) {
		cerr << "Error: " << ex.what() << endl;
		return 1;
	}
   
}