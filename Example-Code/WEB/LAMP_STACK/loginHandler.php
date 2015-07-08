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
<!-- Created By: Jared Fowler, April 22, 2015-->

<?php
   /*This file is called to handle the inputs of user name and password from the main log-in
     page. We will do a series of checks on the data in order to avoid overflows and SQL injection.
     We will then determine if the values are valid. This will be done by first grabbing information 
     from the database, doing the proper hashing, and comparison. If successful, we update any 
     necessary info on the database and move onto the next page, else, we return to the login page.*/
     
   //Include general use php file
   include 'lab4_general_php_func.php';
   
   //Start Session and set variables
   session_start();
   $_SESSION['invalidArguments'] = false;
   $_SESSION['loggedIn'] = false;
   
   //Get the passed in userName and password
   $userName = $_POST['username'];
   $pswd = $_POST['password'];
   
   //Format validity checks
   if(!validatePasswordFormat($pswd) || !validateUserNameFormat($userName)){
      loginFail();
   }
   
   //Connect to the database
   if(($sql = connectToDatabase()) == false){
      loginFail();
   }
   
   //Does the user exist?
   $query = "SELECT * FROM user WHERE username='$userName'";
   //Run query
   if(!($queryResult = mysqli_query($sql, $query))){
      loginFail();
   }
   if(mysqli_num_rows($queryResult) != 1){ 
      loginFail();
   }
   $userInfo = mysqli_fetch_assoc($queryResult); 
   
   //We now have the user info.. check password to verify
   if(!comparePasswords($userInfo["password"], $pswd)){
      loginFail();
   }
   
   //Copy the userId into a session variable
   $_SESSION["userid"] = $userInfo["userid"];
   
   //Success! Login in to the lobby.
   loginSuccess();
     
   //---------------------------------------------
   //Returns direction of pages to login page
   //---------------------------------------------
   function loginFail(){
      //Close the sql connection 
      mysqli_close($sql);
      //Update session info
      $_SESSION['invalidArguments'] = true;
      //Sleep a little bit to offset brute-force attacks
      sleep(1);
      //Navigate back to login page
      header('Location: lab4.php');
      exit;
   }
   //---------------------------------------------
   //Directs user to the lobby
   //---------------------------------------------
   function loginSuccess(){
      //Close the sql connection 
      mysqli_close($sql);
      //Update session info
      $_SESSION['loggedIn'] = true;
      //Navigate back to lobby
      header('Location: lobby.php');
      exit;
   }
?>
