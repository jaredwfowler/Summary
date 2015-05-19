/*
     _____                           ___  _____   ___   _           _         ___                    
    /  __ \                         /   ||  _  | /   | | |         | |       /   |                   
    | /  \/ ___  _ __ ___  _ __    / /| | \ V / / /| | | |     __ _| |__    / /| |                   
    | |    / _ \| '_ ` _ \| '_ \  / /_| | / _ \/ /_| | | |    / _` | '_ \  / /_| |                   
    | \__/\ (_) | | | | | | |_) | \___  || |_| \___  | | |___| (_| | |_) | \___  |                   
     \____/\___/|_| |_| |_| .__/      |_/\_____/   |_/ \_____/\__,_|_.__/      |_/                   
                          | |                                                                        
                          |_|                                                                        
______      _   _   _               _____ _      ___  _ _   _____                _   _               
| ___ \    | | | | (_)             |_   _| |    / _ \| | | |_   _|              | | | |              
| |_/ /   _| |_| |_ _ _ __   __ _    | | | |_  / /_\ \ | |   | | ___   __ _  ___| |_| |__   ___ _ __ 
|  __/ | | | __| __| | '_ \ / _` |   | | | __| |  _  | | |   | |/ _ \ / _` |/ _ \ __| '_ \ / _ \ '__|
| |  | |_| | |_| |_| | | | | (_| |  _| |_| |_  | | | | | |   | | (_) | (_| |  __/ |_| | | |  __/ |   
\_|   \__,_|\__|\__|_|_| |_|\__, |  \___/ \__| \_| |_/_|_|   \_/\___/ \__, |\___|\__|_| |_|\___|_|   
                             __/ |                                     __/ |                         
                            |___/                                     |___/                          
*/
/*
   Created by Jared Fowler
   April 26, 2015
   
   Collection of general use javascript functions and constants to be used in lab4
*/

//Constants
var MAX_USER_LEN = 30;
var MAX_PSWD_LEN = 50;
var MIN_PSWD_LEN = 8;
var MAX_MSG_LEN = 500;
var MAX_EMAIL_LEN = 50;

//Functions
//---------------------------------------------
//Returns true if no spaces are present in string
//---------------------------------------------
function noSpaces(inputStr){
   for(var i = 0; i < inputStr.length; i++){
      if(inputStr.charAt(i) == ' ')
         return false;
   }
   return true;
}
//---------------------------------------------
//Validates the strength of the password
//---------------------------------------------
function validatePasswordStrength(inputStr){
   //Passwords are to have at least one letter,
   //one number, and one special char, with
   //a length greater than 8 and less than MAX
   if(containsLetter(inputStr) && 
      containsNumber(inputStr) && 
      containsSpecialChar(inputStr))
      return true;
   else
      return false;
}
//---------------------------------------------
//Determines if a string contains a letter
//---------------------------------------------
function containsLetter(inputStr){
   for(var i = 0; i < inputStr.length; i++){
      if((inputStr.charAt(i) >= 'a' && inputStr.charAt(i) <= 'z')
         || (inputStr.charAt(i) >= 'A' && inputStr.charAt(i) <= 'Z'))
         return true;
   }
   return false;
}
//---------------------------------------------
//Determines if a string contains a number
//---------------------------------------------
function containsNumber(inputStr){
   for(var i = 0; i < inputStr.length; i++){
      if((inputStr.charAt(i) >= '0' && inputStr.charAt(i) <= '9'))
         return true;
   }
   return false;
}
//---------------------------------------------
//Determines if a string contains a special char
//---------------------------------------------
function containsSpecialChar(inputStr){
   for(var i = 0; i < inputStr.length; i++){
      if(!(inputStr.charAt(i) >= 'a' && inputStr.charAt(i) <= 'z')
         && !(inputStr.charAt(i) >= 'A' && inputStr.charAt(i) <= 'Z')
         && !(inputStr.charAt(i) >= '0' && inputStr.charAt(i) <= '9'))
         return true;
   }
   return false;
}
//---------------------------------------------
//Determines if a password is in valid format
//---------------------------------------------
function validPasswordFormat(inputStr){
   if(
      //Validate Strength
      !validatePasswordStrength(inputStr) ||
      //No spaces
      !noSpaces(inputStr) ||
      //Valid length?
      (inputStr.length < MIN_PSWD_LEN) ||
      (inputStr.length > MAX_PSWD_LEN))
      return false;
   else
      return true;
}
//---------------------------------------------
//Determines if a userName is of valid format
//---------------------------------------------
function validUserNameFormat(inputStr){
   if(
      //No spaces
      !noSpaces(inputStr) ||
      //Valid length?
      (inputStr.length == 0) ||
      (inputStr.length > MAX_USER_LEN))
      return false;
   else
      return true;
}
//---------------------------------------------
//Determines if a email is valid format
//Taken from www.w3resource.com
//---------------------------------------------
 function validEmailFormat(inputStr){
   var mailformat = /^\w+([\.-]?\w+)*@\w+([\.-]?\w+)*(\.\w{2,3})+$/;   
   if(!inputStr.match(mailformat))
      return false;
   //Valid length?
   if(inputStr.length == 0 || inputStr.length > MAX_EMAIL_LEN)
      return false;
   return true;
 }  