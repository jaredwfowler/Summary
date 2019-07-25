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
   if (this->curl != NULL) {
      curl_easy_cleanup(this->curl);
      this->curl = NULL;
   }
}

//--------------------------------------------------------------------------------------------------
//Name:
// download
//
//Arguments:
// _url -> The url to download
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
std::string HttpDownloader::download(const std::string& _url) 
{
   curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
   /* example.com is redirected, so we tell libcurl to follow redirection */
   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
   curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //Prevent "longjmp causes uninitialized stack frame" bug
   curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
   std::stringstream out;
   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpDownloader::write_data);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
   /* Perform the request, res will get the return code */
   //JWF - Attempt request up to 5 times before giving up
   for (unsigned int j = 0; j < 5; j++)
   {
      CURLcode res = curl_easy_perform(curl);
      /* Check for errors */
      if (res != CURLE_OK) 
      {
         fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
      }
      else
      {
         return out.str();
      }
   }
   fprintf(stderr, "HttpDownloader::download failed attempts\n");
   return "";
}

//--------------------------------------------------------------------------------------------------
//Name:
// write_data
//
//Arguments:
// ...
//
//Output:
// void
//
//Returns:
// void
//
//About:
// ...
//
//--------------------------------------------------------------------------------------------------
size_t HttpDownloader::write_data(void *_ptr, size_t _size, size_t _nmemb, void *_stream) 
{
   std::string data((const char*)_ptr, (size_t)_size * _nmemb);
   *((std::stringstream*)_stream) << data;
   return _size * _nmemb;
}

/***************************************************************************************************
UPDATE LOG *****************************************************************************************
****************************************************************************************************

****************************************************************************************************
***************************************************************************************************/
