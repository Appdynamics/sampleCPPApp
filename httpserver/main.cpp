#include <boost/network/protocol/http/server.hpp>
#include "/opt/appdynamics-cpp-sdk/include/appdynamics.h"
#include <iostream>
#include <curl/curl.h>
#include <cstdlib>
// #include <string.h>

const char APP_NAME1[] = "cppsampleapp4";
const char TIER_NAME1[] = "tier2";
const char NODE_NAME1[] = "cppnode2";
const char *CONTROLLER_HOST="master-saas-controller.e2e.appd-test.com";
const int CONTROLLER_PORT = 8090;
const char CONTROLLER_ACCOUNT[] = "e2e-customer";
// const char CONTROLLER_ACCESS_KEY[] = "7a15fef0-c685-47ed-9bc3-3db885018363";
const int CONTROLLER_USE_SSL = 0;

namespace http = boost::network::http;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void doSomethingElse(appd_bt_handle bt, const char* corr) {
    appd_frame_handle fh=NULL;
    if(appd_bt_is_snapshotting(bt)) {
	    fh = appd_frame_begin(bt, APPD_FRAME_TYPE_CPP, NULL, "subBranchOf1", "tdiwate/main/doSomethingElse", 132);
    }
    usleep(1000);
	if (fh != NULL){
		appd_frame_end(bt, fh);
	}
}

void doSomething(appd_bt_handle bt, const char* backendOne, const char* corr) {
    appd_frame_handle fh=NULL;
    if(appd_bt_is_snapshotting(bt)) {
	    fh = appd_frame_begin(bt, APPD_FRAME_TYPE_CPP, NULL, "branch1", "tdiwate/main/doSomething", 54);
    }
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();

    if(curl) {
        appd_exitcall_handle ecHandle = appd_exitcall_begin(bt, backendOne);

        int rc = appd_exitcall_set_details(ecHandle, "jsonplaceholder_req");
		if (rc) {
   			std::cout << "Error: exitcall details " << std::endl;
		}
        curl_easy_setopt(curl, CURLOPT_URL, "http://jsonplaceholder.typicode.com/todos/1");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        appd_exitcall_end(ecHandle);
    }

    doSomethingElse(bt, corr);

	if (fh != NULL){
		std::cout << "framehandle is not null for doSomething call" << std::endl;
		appd_frame_end(bt, fh);
	}
}

struct hello_world;
typedef http::server<hello_world> server;

struct hello_world {
    void operator()(server::request const &request, server::response &response) {
        std::string corrhdr;
        for (const auto& header : request.headers) {
            if (header.name == "APPD_CORRELATION_HEADER_NAME") corrhdr = header.value;
            if (!corrhdr.empty()) break;
        }
        const char backendOne[] = "jsonplaceholder_server";
        const char * c = corrhdr.c_str();
        appd_bt_handle btHandle = appd_bt_begin("catchAll", c);

        server::string_type ip = source(request);
        unsigned int port = request.source_port;
        std::ostringstream data;
        doSomething(btHandle, backendOne, c);
        data << "Hello, " << ip << ':' << port << '!';
        response = server::response::stock_reply(server::response::ok, data.str());
        appd_bt_end(btHandle);
    }
    void log(const server::string_type& message) {
        std::cerr << "ERROR: " << message << std::endl;
    }
};

int main(int argc, char *argv[]) {
    const char* CONTROLLER_ACCESS_KEY;
    if(CONTROLLER_ACCESS_KEY = std::getenv("CONTROLLER_ACCESS_KEY"))
        std::cout << CONTROLLER_ACCESS_KEY << std::endl;
    int initRC;
    appd_config *cfg = appd_config_init();
	appd_config_set_app_name(cfg, APP_NAME1);
	appd_config_set_tier_name(cfg, TIER_NAME1);
	appd_config_set_node_name(cfg, NODE_NAME1);
	appd_config_set_controller_host(cfg, CONTROLLER_HOST);
	appd_config_set_controller_port(cfg, CONTROLLER_PORT);
	appd_config_set_controller_account(cfg, CONTROLLER_ACCOUNT);
	appd_config_set_controller_access_key(cfg, CONTROLLER_ACCESS_KEY);
	appd_config_set_controller_use_ssl(cfg, CONTROLLER_USE_SSL);
	appd_config_getenv(cfg, NULL);
	// set log level
	appd_config_set_logging_min_level(cfg, APPD_LOG_LEVEL_TRACE);
	appd_config_set_init_timeout_ms(cfg,60000);

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " address port" << std::endl;
        return 1;
    }
    std::cout << "starting server" << std::endl;
    initRC = appd_sdk_init(cfg);
    const char backendOne[] = "jsonplaceholder_server";
    appd_backend_declare(APPD_BACKEND_HTTP,  backendOne);
    int rc = appd_backend_set_identifying_property(backendOne, "HOST", "jsonplaceholder.typicode.com");
    if (rc)
    {
        std::cout << "Backend identifying properties could not be set" << std::endl;
        return 1;
    }
    rc = appd_backend_add(backendOne);
    if (rc)
    {
        std::cout << "Backend could not be added\n" << std::endl;
        return 1;
    }
    std::cout << "Backend added successfully\n" << std::endl;
    try {
        hello_world handler;
        server::options options(handler);
        server server_(options.address(argv[1]).port(argv[2]));
        server_.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
