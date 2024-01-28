// Name: URLProxy.cpp
// Auth: Kareem T
// Date: 1/25/2024
// Desc: Library for cycling proxies onto a GET request until a successful connection is made
#include <iostream>             // File IO
#include <fstream>              // File IO
#include <string>               // Strings
#include <vector>               // Vectors
#include <unordered_map>        // Hashmap
#include <thread>               // Sleep
#include <chrono>               // Time
#include <random>               // Random
#include <json/json.h>          // JSON Parsing
#include <curl/curl.h>          // (CURL) Web get-requests
#include <pybind11/pybind11.h>  // Compile library for Python use
using namespace std;

static string filep = "credentials.json";
void set_credentials_fpath(string filePath){filep = filePath;} /* Setter for credentials.json filepath || Helper Function */

size_t WriteCallBack(void* contents, size_t size, size_t nmemb, string* output) {
/* Write function for libCURL library || Helper Function necessary by library (libCURL) */
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

struct NetInfo {
// Struct containing Socks5 proxies, browser headers, and Socks5 credentials || Helper Function
    vector<string> IP;
    vector<char*> headers;
    string creds;
    bool complete = false;
};

NetInfo socks_credentials(){
/* Retrieval function for proxy information || Helper Function */
    // Open the file for reading
    NetInfo result;
    ifstream file(filep);
    cout << filep << endl;
    if (!file.is_open()){
        cerr << "Error opening file." << endl; 
        return result;
    }
    // Declare JSON-parsing variables
    Json::CharReaderBuilder reader;
    Json::Value jsonData;
    string err;
    if (!Json::parseFromStream(reader, file, &jsonData, &err)){
        cerr << "Error parsing JSON: " << err << endl; 
        return result;
    }
    // Access JSON data
    for (Json::Value& proxy : jsonData["proxies"]){
        result.IP.push_back(proxy.asString());
    }
    for (Json::Value& header : jsonData["headers"]){
        result.headers.push_back(const_cast<char*>(header.asCString()));
    }
    for (Json::Value& cred : jsonData["credentials"]){
        result.creds = cred.asString();
    }
    result.complete = true;
    return result;
}

void setup_curl(CURL* curl, const char* url, char* header, const char* proxy, string* response){
/* Sets options (url, proxy, header, response data, writebackfn) for CURL object || Helper Function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallBack);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, header);  
    curl_easy_setopt(curl, CURLOPT_PROXY, proxy);      
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
}

const string force_connect(const char* url, const int tries){
/* Returns GET response using a random proxy, cycles upto 'tries' times per proxy until success */
    thread_local int fails = 0;
    static thread_local chrono::steady_clock::time_point start_time;
    static thread_local long long duration;
    start_time = chrono::steady_clock::now();
    // Random number generators (random proxy selection)
    static random_device rd;
    static mt19937 genP(rd()), genH(rd());
    // Retrieve proxy information
    const static NetInfo info = socks_credentials();
    if(!info.complete) return string("ERROR: Proxy credentials have not been properly loaded");
    // Create uniform distribution for generators (using proxy size)
    static uniform_int_distribution<> proxy_dis(0, info.IP.size() - 1), header_dis(0, info.headers.size() - 1);
    // Declare variables for maintaining proxy information
    static vector<string> IPS = info.IP;                              // GUIDE: Why use const?
    static thread_local const string* IP;                             // const string - access const strings (ip) at different ptr locations
    static thread_local string proxy;         
    // Declare variables for handling request response
    static thread_local string* const response_data = new string(""); // const ptr - access response data same location (var: response_data)
    static unordered_map<const string*, int> badIP;                   // const string - save const strings as different ptr locations
    static thread_local CURL *curl;
    static thread_local CURLcode res;
    // Initialize CURL (for connection)
    curl = curl_easy_init();
    IP = &(IPS[proxy_dis(genP)]);
    proxy.assign("socks5h://@:1080").insert(10, info.creds).insert(60, *IP); // Perform operations in-place (on pre-existing strings), rather than new ones
    (*response_data).clear();
    (*response_data).reserve(1024*1024*1024);
    if (curl) {
        // Perform the request
        setup_curl(curl, url, info.headers[header_dis(genH)], proxy.c_str(), response_data);
        res = curl_easy_perform(curl);
        while (res != 0) {
            /* CONNECTION FAILURE */
            if(badIP.count(IP) == 0) badIP.insert({IP, 0});     // Add IP to broken map, increase fail count (def: 5 fails)
            badIP[IP]++;                                       
            cerr << "Error (IP: " << *IP << ") - Request failed: " << curl_easy_strerror(res) << endl;
            // INDIVIDUAL PROXY FAILURE THRESH-HOLD REACHED
            if(badIP[IP] == tries){                      
                cout << "Deleting " << *IP << " from working IP dict" << endl;
                IPS.erase(remove(IPS.begin(), IPS.end(), *IP), IPS.end());     // Remove IP from working vector
                if(!IPS.size()){// ALL PROXIES TIMED OUT                         
                    cout << "All proxies have timed out, clearing Bad_IPs cache" << endl;
                    for (pair<const string* const,int> deletedIP : badIP){
                        IPS.push_back(*(deletedIP.first));        // Reset all IPs to working vector
                        deletedIP.second = 0;
                    }
                } 
                proxy_dis.param(uniform_int_distribution<int>::param_type(0, IPS.size() - 1)); // Update distribution parameters for new size
            }  
            // Refresh new proxy
            IP = &IPS[proxy_dis(genP)];
            proxy.assign("socks5h://@:1080").insert(10, info.creds).insert(60, *IP);
            fails++;
            this_thread::sleep_for(chrono::seconds(1));
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, info.headers[header_dis(genH)]);
            res = curl_easy_perform(curl);
        }
        // Calculate and print the duration
        duration = (chrono::steady_clock::now() - start_time).count();
        cout << "Success (IP: " << *IP <<  ")\tRuntime: " << duration << " sec.\tAdjusted (" << fails << " failures): "  << duration - fails <<  " sec.)" << endl;
        curl_easy_reset(curl);
        curl_easy_cleanup(curl);
    }
    return *response_data;
}

namespace py = pybind11; 
// Create a Pybind11 module
PYBIND11_MODULE(URLProxyC, m) {
    // Expose the add function to Python
    m.def("force_connect", &force_connect, "A function that cycles proxies on HTTP requests", py::return_value_policy::move);
    m.def("set_credentials_fpath", &set_credentials_fpath, "A function that edits proxy credentials' file path");
}
