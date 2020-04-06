#include <iostream>

#include "restinio/all.hpp"

using namespace std;

int main() {
    restinio::run(
        restinio::on_this_thread()
            .port(8080)
            .address("localhost")
            .request_handler([](auto req) {
                return req->create_response().set_body("Hello, World!").done();
    }));
}