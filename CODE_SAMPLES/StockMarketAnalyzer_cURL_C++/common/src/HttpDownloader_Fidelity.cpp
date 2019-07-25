//Dependencies======================================================================================
//==================================================================================================
#include "HttpDownloader_Fidelity.hpp"
#include <curl/easy.h>
#include <curl/curlbuild.h>
#include <sstream>
#include <iostream>

//Method Definitions================================================================================
//==================================================================================================

//--------------------------------------------------------------------------------------------------
//Name:
// HttpDownloader_Fidelity
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
// Constructor
//
//--------------------------------------------------------------------------------------------------
HttpDownloader_Fidelity::HttpDownloader_Fidelity(const std::string _user,
                                                 const std::string _password,
                                                 const std::string _cookieFileName) 
{
   // Store a copy of the information
   this->cookieFileName = _cookieFileName;
   this->userName = _user;
   this->password = _password;
}

//--------------------------------------------------------------------------------------------------
//Name:
// ~HttpDownloader_Fidelity
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
// Handles cleanup.
//
//--------------------------------------------------------------------------------------------------
HttpDownloader_Fidelity::~HttpDownloader_Fidelity() 
{
   // We first need to release the file decrypts
   destroyCurl();

   std::remove(this->cookieFileName.c_str());
}

//--------------------------------------------------------------------------------------------------
//Name:
// loginToFidelityAndSetCookie
//
//Arguments:
// _user -> Fidelity user name
// _password -> Fidelity user password
// _cookieFileName -> Name of file to create for cookie storage
//
//Output:
// void
//
//Returns:
// -1 on failure
//
//About:
// Logs into Fidelity.com with the provided user arguments. Writes associated cookies to maintain
// the session.
//
//--------------------------------------------------------------------------------------------------
int HttpDownloader_Fidelity::loginToFidelityAndSetCookie() 
{
   int returnVal = -1;

   // These 2 options are needed to store and use cookies. For this project, to store login information for fidelity
   curl_easy_setopt(curl, CURLOPT_COOKIEJAR, this->cookieFileName.c_str());
   curl_easy_setopt(curl, CURLOPT_COOKIEFILE, this->cookieFileName.c_str());

   // Prepare https request
   char query[512] = { 0 };

   #if defined(_MSC_VER) || defined(__WINDOWS__)

   sprintf_s(query, "https://login.fidelity.com/ftgw/Fas/Fidelity/RtlCust/Login/Response?SSN=%s&PIN=%s",
             this->userName.c_str(), this->password.c_str());

   #else

   snprintf(query, 512, "https://login.fidelity.com/ftgw/Fas/Fidelity/RtlCust/Login/Response?SSN=%s&PIN=%s",
            this->userName.c_str(), this->password.c_str());

   #endif

   if ("" != download(query)) 
   {
      returnVal = 0;
   }

   return returnVal;
}
