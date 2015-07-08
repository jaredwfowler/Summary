<!--
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
-->
<!--
   Created By: Jared Fowler
   April 22, 2015
   
   A general collection of php functions and constants which can be imported into other .php files
-->
<?php
   //Constants
   define("MAX_USER_LEN", 30, true);      //Max length of user id 
   define("MAX_PSWD_LEN", 50, true);      //Max length of password
   define("MIN_PSWD_LEN", 8, true);       //Min length of password
   define("DATABASE", "Comp484", true);   //Name of database
   define("SERVER", "localhost", true);   //Name of server
   define("SERVER_UN", "484Query", true); //Server user name
   define("SERVER_PD", "thatWasFun!", true); //Server password
   define("SALT_LEN", 40, true);          //Number of chars in salt
   define("HASH_TYPE", "sha512", true);   //Hash method
   define("MAX_MSG_LEN", 500, true);      //Max lengthof message
   define("MAX_EMAIL_LEN",50, true);      //Max email length
   
   //---------------------------------------------
   //Checks the type for no spaces
   //---------------------------------------------
   function noSpaces($str){
      for($i = 0; $i < strlen($str); $i++){
         if(substr($str,$i,1) == ' ')
            return false;
      }
      return true;
   }
   //---------------------------------------------
   //Determines if a string contains a special char
   //---------------------------------------------
   function containsSpecialChar($inputStr){
      for($i = 0; $i < strlen($inputStr); $i++){
         $temp = substr($inputStr,$i,1);
         if(!($temp >= 'a' && $temp <= 'z')
            && !($temp >= 'A' && $temp <= 'Z')
            && !($temp >= '0' && $temp <= '9'))
            return true;
      }
      return false;
   }
   //---------------------------------------------
   //Determines if a string contains a number
   //---------------------------------------------
   function containsNumber($inputStr){
      for($i = 0; $i < strlen($inputStr); $i++){
         $temp = substr($inputStr,$i,1);
         if($temp >= '0' && $temp <= '9')
            return true;
      }
      return false;
   }
   //---------------------------------------------
   //Determines if a string contains a letter
   //---------------------------------------------
   function containsLetter($inputStr){
      for($i = 0; $i < strlen($inputStr); $i++){
         $temp = substr($inputStr,$i,1);
         if(($temp >= 'a' && $temp <= 'z')
            || ($temp >= 'A' && $temp <= 'Z'))
            return true;
      }
      return false;
   }
   //---------------------------------------------
   //Validates the strength of the password
   //---------------------------------------------
   function validatePasswordStrength($inputStr){
      //Passwords are to have at least one letter,
      //one number, and one special char, with
      //a length greater than 8 and less than MAX
      if(containsLetter($inputStr) && 
         containsNumber($inputStr) && 
         containsSpecialChar($inputStr))
         return true;
      else
         return false;
   }
   //---------------------------------------------
   //Validates the format of a password
   //---------------------------------------------
   function validatePasswordFormat($inputStr){
      if(
         //Validate the strength
         !validatePasswordStrength($inputStr) ||
         //No spaces
         !noSpaces($inputStr) ||
         //Min and max length
         (strlen($inputStr) > MAX_PSWD_LEN) ||
         (strlen($inputStr) < MIN_PSWD_LEN))
         return false;
      else
         return true;
   }
   //---------------------------------------------
   //Validates the format of a userName
   //---------------------------------------------
   function validateUserNameFormat($inputStr){
      if(
         //No spaces
         !noSpaces($inputStr) ||
         //Min and max length
         (strlen($inputStr) > MAX_USER_LEN) ||
         ($inputStr == ""))
         return false;
      else
         return true;
   }
   //---------------------------------------------
   //Validates the format of an email address
   //---------------------------------------------
   function validateEmailFormat($inputStr){
      //Use a useful php function
      if(!filter_var($inputStr, FILTER_VALIDATE_EMAIL) ||
         (strlen($inputStr) > MAX_EMAIL_LEN))
         return false;
      else
         return true;
   }
   //---------------------------------------------
   //Validates the format of the message
   //---------------------------------------------
   function validateMessageFormat($inputStr){
      //Use a useful php function
      if(existSqlKeywords($inputStr) ||
          (strlen($inputStr) > MAX_MSG_LEN))
         return false;
      else
         return true;
   }
   //---------------------------------------------
   //Changes format of message to valid sql injection
   //---------------------------------------------
   function messageEncode($inputStr){
      //Simple way to avoid sql injection...
      return bin2hex($inputStr);
   }
   //---------------------------------------------
   //Changes format of message to valid sql injection
   //---------------------------------------------
   function messageDecode($inputStr){
      //Simple way to avoid sql injection...
      return hex2bin($inputStr);
   }
   //---------------------------------------------
   //Checks if entered password matches hashed-salted 
   //one. Takes in plain text password, and a hashed-
   //salted-password with the salt pre-pended
   //---------------------------------------------
   function comparePasswords($hsPassword, $password){
      //Parse out the salt from the hsPassword
      $salt = substr($hsPassword,0,SALT_LEN);
      $hsPassword = substr($hsPassword,SALT_LEN);
      //Get the hashed password
      $password = getHashedPassword($salt, $password);
      //Compare
      if($password == $hsPassword)
         return true;
      else
         return false;
   }
   //---------------------------------------------
   //Takes plain text password and salt to generate
   //a salted hashed password.
   //---------------------------------------------
   function getHashedPassword($salt, $pswd){
      return hash(HASH_TYPE, $salt . $pswd);
   }
   //---------------------------------------------
   //Generates a salt compose of SALT_LEN chars (hex values)
   //---------------------------------------------
   function generateSalt(){
      return bin2hex(openssl_random_pseudo_bytes(SALT_LEN / 2));
   }
   //---------------------------------------------
   //Connects to the sql database using defined values
   //If successful, returns a handle, else returns false.
   //---------------------------------------------
   function connectToDatabase(){
      return mysqli_connect(SERVER, SERVER_UN, SERVER_PD, DATABASE);
   }
   //---------------------------------------------
   //Checks for sql command words.. Returns true
   //if one or more are found.
   //---------------------------------------------
   function existSqlKeywords($inputStr){
      //Add space to position 0, and last pos
      $inputStr = '$ ' . $inputStr . ' '; //$ is required due to . properties.
      //Make all same case
      $inputStr = strtolower($inputStr);
      
      //INSERT
      $list = strpos_r($inputStr, " insert ");
      if($list !== false)
         if(count($list) != 0){
            return true;
         }
      //SELECT
      $list = strpos_r($inputStr, " select ");
      if($list !== false)
         if(count($list) != 0){
            return true;
         }
      //CREATE
      $list = strpos_r($inputStr, " create ");
      if($list !== false)
         if(count($list) != 0){
            return true;
         }
      //UPDATE
      $list = strpos_r($inputStr, " update ");
      if($list !== false)
         if(count($list) != 0){
            return true;
         }
      //DELETE
      $list = strpos_r($inputStr, " delete ");
      if($list !== false)
         if(count($list) != 0){
            return true;
         }
      //DROP
      $list = strpos_r($inputStr, " drop ");
      if($list !== false)
         if(count($list) != 0){
            return true;
         }
      //JOIN
      $list = strpos_r($inputStr, " join ");
      if($list !== false)
         if(count($list) != 0){
            return true;
         }  
      //GRANT
      $list = strpos_r($inputStr, " grant ");
      if($list !== false)
         if(count($list) != 0){
            return true;
         }
         
      return false;  
   }
   //---------------------------------------------
   //Gets all offsets of a particular substring
   //in a string (found at 
   //http://php.net/manual/en/function.strpos.php)
   //---------------------------------------------
   function strpos_r($haystack, $needle)
   {
    if(strlen($needle) > strlen($haystack))
       return false;

    $seeks = array();
    while($seek = strrpos($haystack, $needle))
    {
        array_push($seeks, $seek);
        $haystack = substr($haystack, 0, $seek);
    }
    return $seeks;
   }
?>