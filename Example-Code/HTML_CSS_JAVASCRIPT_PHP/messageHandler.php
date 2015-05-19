<!--
     _____                           ___  _____   ___   _           _         ___                    
    /  __ \                         /   ||  _  | /   | | |         | |       /   |                   
    | /  \/ ___  _ __ ___  _ __    / /| | \ V / / /| | | |     __ _| |__    / /| |                   
    | |    / _ \| '_ ` _ \| '_ \  / /_| | / _ \/ /_| | | |    / _` | '_ \  / /_| |                   
    | \__/\ [_] | | | | | | |_] | \___  || |_| \___  | | |___| [_| | |_] | \___  |                   
     \____/\___/|_| |_| |_| .__/      |_/\_____/   |_/ \_____/\__,_|_.__/      |_/                   
                          | |                                                                        
                          |_|                                                                        
______      _   _   _               _____ _      ___  _ _   _____                _   _               
| ___ \    | | | | [_]             |_   _| |    / _ \| | | |_   _|              | | | |              
| |_/ /   _| |_| |_ _ _ __   __ _    | | | |_  / /_\ \ | |   | | ___   __ _  ___| |_| |__   ___ _ __ 
|  __/ | | | __| __| | '_ \ / _` |   | | | __| |  _  | | |   | |/ _ \ / _` |/ _ \ __| '_ \ / _ \ '__|
| |  | |_| | |_| |_| | | | | [_| |  _| |_| |_  | | | | | |   | | [_] | [_| |  __/ |_| | | |  __/ |   
\_|   \__,_|\__|\__|_|_| |_|\__, |  \___/ \__| \_| |_/_|_|   \_/\___/ \__, |\___|\__|_| |_|\___|_|   
                             __/ |                                     __/ |                         
                            |___/                                     |___/                          
-->
<!-- Created By: Jared Fowler, April 22, 2015-->

<?php
   //Include general use php file
   include 'lab4_general_php_func.php';

   session_start();
   //Are we here legitimately?
   if($_SESSION['loggedIn'] == false){
      //Redirect to login page
         header('Location: lab4.php');
         exit;
   }
   
   //Re-set some session flags
   $_SESSION['msgSent'] = false;
   $_SESSION['msgError'] = false;
   
   //Get some session info
   $userId = $_SESSION['userid'];

   //Get passed in values
   $email = $_POST["email"];
   $message = $_POST["message"];
   $minute = intval($_POST["minute"]);
   $hour = intval($_POST["hour"]);
   $day = intval($_POST["day"]);
   $month = intval($_POST["month"]);
   $year = intval($_POST["year"]);
   $offset = intval($_POST["tzOffset"]);
   
   //Check Validity of email address
   if(!validateEmailFormat($email))
      fail();
   
   //Encode message format to avoid sql injection, yet
   //give the user free range of words.
   $message = messageEncode($message);
   
   //Valid message?
   if(!validateMessageFormat($message))
      fail();
   
   //Time stuff
   //Validation
   if(($year < 2015 || $year > 2017) ||
      ($month < 1 || $month > 12) ||
      ($day < 1 || $day > 31) ||
      ($hour < 0 || $hour > 23) ||
      ($minute < 0 || $minute > 59))
      fail();
   //Get the unix time stamp for the send time
   $sendTime = mktime($hour, $minute, 0, $month, $day, $year);
   
   //Valid offset from GMT time?
   if($offset > 720 || $offset < -720)
      $offset = 0;
   //Update the send time
   //$sendTime = $sendTime + ($offset * 60);
   
   //Connect to the database
   if(!($sql = connectToDatabase())){
      fail();
   }
   
   //Add row to the database
   $query = "INSERT INTO message (userid, emailAddress, message, timeStamp, sent) VALUES ('$userId', '$email', '$message', '$sendTime', FALSE)";
   //Run query
   if(!($queryResult = mysqli_query($sql, $query))){
      fail();
   }
   
   //Exit Database and return to lobby
   $_SESSION['msgSent'] = true;
   mysqli_close($sql);
   header('Location: lab4.php');
   exit;
   
   //---------------------------------------------
   //On failure, navigate back to the lobby
   //---------------------------------------------
   function fail(){
      mysqli_close($sql);
      $_SESSION['msgError'] = true;
      header('Location: lab4.php');
      exit;
   }
?>
