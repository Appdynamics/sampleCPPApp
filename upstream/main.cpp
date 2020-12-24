#include "/opt/appdynamics-cpp-sdk/include/appdynamics.h"
#include <iostream>
#include <unistd.h>
#include <curl/curl.h>
#include <cstring>

const char APP_NAME1[] = "cppsampleapp4";
const char TIER_NAME1[] = "tier1";
const char NODE_NAME1[] = "cppnode1";
const char *CONTROLLER_HOST="master-saas-controller.e2e.appd-test.com";
const int CONTROLLER_PORT = 8090;
const char CONTROLLER_ACCOUNT[] = "e2e-customer";
// const char CONTROLLER_ACCESS_KEY[] = "7a15fef0-c685-47ed-9bc3-3db885018363";
const int CONTROLLER_USE_SSL = 0;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void exitCallToDownstream(appd_bt_handle btHandle, const char* backendOne) {
	appd_frame_handle fh = appd_frame_begin(btHandle, APPD_FRAME_TYPE_CPP, NULL, "exitCallToDownstream", "tdiwate/main", 22);

    appd_exitcall_handle ecHandle = appd_exitcall_begin(btHandle, backendOne);

    const char* corrHeader1 = appd_exitcall_get_correlation_header(ecHandle);
    // std::cout << " corrheader: "<< corrHeader1 <<std::endl;
    int rc = appd_exitcall_set_details(ecHandle, "cpp_downstream_req");
    if (rc) {
        std::cout << "Error: exitcall details " << std::endl;
    }
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        char result[1000];   // array to hold the result.
        std::strcpy(result,"APPD_CORRELATION_HEADER_NAME: "); // copy string one into the result.
        std::strcat(result,corrHeader1); 

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, result);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, "http://host.docker.internal:8000/api/hello");
        // curl_easy_setopt(curl, CURLOPT_URL, "http://cpphttp:8000/api/hello");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        // std::cout << readBuffer << std::endl;
    }
    appd_exitcall_end(ecHandle);


	if (fh != NULL) {
		std::cout << "framehandle in doSomethingElse is not null" << std::endl;
		appd_frame_end(btHandle, fh);
	}
}

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
    initRC = appd_sdk_init(cfg);

    // declare/add backend
	const char backendOne[] = "first_backend";
	appd_backend_declare(APPD_BACKEND_HTTP,  backendOne);
	int rc = appd_backend_set_identifying_property(backendOne, "HOST", "cpp_downstream");
	if (rc)
	{
		std::cout << "Backend identifying properties could not be set";
		return 1;
	}
	rc = appd_backend_add(backendOne);
	if (rc)
	{
		std::cout << "Backend could not be added" ;
		return 1;
	}

    while(true)
	{  
        appd_bt_handle btHandle = appd_bt_begin("api/call", NULL);
        exitCallToDownstream(btHandle, backendOne);
        appd_bt_end(btHandle);
        usleep(30000);
    }

    return 0;
}
