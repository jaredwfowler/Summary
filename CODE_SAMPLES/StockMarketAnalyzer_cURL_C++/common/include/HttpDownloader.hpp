
#ifndef HTTP_DOWNLOADER_HPP
#define HTTP_DOWNLOADER_HPP

/***************************************************************************************************
************************         |\/| o  _ ._ _  |  _   _   _.  _           ************************
************************         |  | | (_ | (_) | (/_ (_| (_| (_ \/        ************************
************************                                _|        /         ************************
************************                      Estd. 2015                    ************************
****************************************************************************************************
****************************************************************************************************
**** The use, disclosure, reproduction, modification, transfer, or transmittal of this work for ****
**** any purpose in any form or by any means without the written permission of Microlegacy is   ****
**** strictly prohibited.                                                                       ****
****                                                                                            ****
**** Confidential, Unpublished Property of Microlegacy.                                         ****
**** Use and Distribution Limited Solely to Authorized Personnel.                               ****
****                                                                                            ****
**** All Rights Reserved                                                                        ****
****************************************************************************************************
CREATOR ********************************************************************************************
****************************************************************************************************
****  Name:  Jared Fowler
**** Title:  Co-Founder of Microlegacy
****  Date:  December 7, 2015
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.0
** Version Date: December 7, 2015
** Revision Personnel: JFowler,
****************************************************************************************************
ABOUT **********************************************************************************************
****************************************************************************************************
A simple C++ wrapper for the libcurl easy API.
Written by Uli K�hler (techoverflow.net)
Published under CC0 1.0 Universal (public domain)
Modified by Jared Fowler
****************************************************************************************************
***************************************************************************************************/
//Dependencies======================================================================================
//==================================================================================================
#include <string>
#include <curl/curl.h>

//Definition========================================================================================
//==================================================================================================
class HttpDownloader
{
public: //##########################################################################################
   //Constructor(s)=================================================================================
   //===============================================================================================
   HttpDownloader();

   //Destructors====================================================================================
   //===============================================================================================
   ~HttpDownloader();

   //Do=============================================================================================
   //===============================================================================================
   std::string download(const std::string& _url);
   static size_t write_data(void *_ptr, size_t _size, size_t _nmemb, void *_stream);

protected: //#######################################################################################
   //Vars,typedefs,etc.=============================================================================
   //===============================================================================================
   void* curl;

   //Do=============================================================================================
   //===============================================================================================
   void destroyCurl();

};
//==================================================================================================
//End Definition====================================================================================

#endif
