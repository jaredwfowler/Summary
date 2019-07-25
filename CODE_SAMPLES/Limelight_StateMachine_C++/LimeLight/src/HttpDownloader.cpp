//Dependencies======================================================================================
//==================================================================================================
#include "HttpDownloader.hpp"
#include <curl/easy.h>
#include <curl/curlbuild.h>
#include <sstream>
#include <iostream>

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// HttpDownloader
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Default Constructor
//
//--------------------------------------------------------------------------------------------------
HttpDownloader::HttpDownloader() 
{
    curl_global_init(CURL_GLOBAL_ALL);
    this->curl = curl_easy_init();
}

//--------------------------------------------------------------------------------------------------
//Name:
// ~HttpDownloader
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Destructor
//
//--------------------------------------------------------------------------------------------------
HttpDownloader::~HttpDownloader() 
{
    this->destroyCurl();
}

//--------------------------------------------------------------------------------------------------
//Name:
// destroyCurl
//
//Arguments:
// void
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Cleans curl up and sets to NULL. This method was added so that derived classes can force curl
// to be cleaned up in order to let go of different handles, like file decrypts on the cookies.
//
//--------------------------------------------------------------------------------------------------
void HttpDownloader::destroyCurl() 
{
    if (this->curl != NULL) 
    {
        curl_easy_cleanup(this->curl);
        this->curl = NULL;
    }

    curl_global_cleanup();
}

//--------------------------------------------------------------------------------------------------
//Name:
// download
//
//Arguments:
// _url -> The url to download
// _args -> Additional parameters. Should be written like:
//          GET/POST: arg1=val1&arg2=val2
// _rt -> Request type, post, get, etc..
//
//Output:
// void
//
//Returns:
// void
//
//About:
// Downloads a provided url and returns the content as a string. Empty string returned on fail.
//
//--------------------------------------------------------------------------------------------------
std::string HttpDownloader::download(const char* _url, const char * _args, request_type_t _rt)
{
    std::string readBuffer;

    if (_rt == GET)
    {
        sprintf(requestBuilder, "%s?%s", _url, _args);
        curl_easy_setopt(curl, CURLOPT_URL, requestBuilder);
    }
    else if (_rt == POST)
    {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _args);
        curl_easy_setopt(curl, CURLOPT_URL, _url);
    }

    // Skip server certificate stuff -- Be Smart.
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // Have CURL write to our string, not stdout
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) 
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        return "";
    }

    return readBuffer;
}

//--------------------------------------------------------------------------------------------------
//Name:
// WriteCallback
//
//Arguments:
//
//
//Output:
// void
//
//Returns:
// void
//
//About:
// CURL requires a function handler if we want output to go somewhere other than stdout. 
//
//--------------------------------------------------------------------------------------------------
size_t HttpDownloader::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
