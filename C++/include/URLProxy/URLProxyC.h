#pragma once
#include <iostream>
#include <string>
#include <curl/curl.h>
/* Setter for credentials.json filepath || Helper Function */
void set_credentials_fpath(string filePath); 

/* Write function for libCURL library || Helper Function necessary by library (libCURL) */
size_t WriteCallBack(void* contents, size_t size, size_t nmemb, string* output);

// Struct containing Socks5 proxies, browser headers, and Socks5 credentials || Helper Function
struct NetInfo;

/* Retrieval function for proxy information || Helper Function */
NetInfo socks_credentials();

/* Sets options (url, proxy, header, response data, writebackfn) for CURL object || Helper Function */
void setup_curl(CURL* curl, const char* url, char* header, const char* proxy, string* response);

/* Returns GET response using a random proxy, cycles upto 'tries' times per proxy until success */
const string force_connect(const char* url, const int tries);