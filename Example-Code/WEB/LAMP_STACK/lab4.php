<!DOCTYPE HTML>
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
   
   Project: Create a basic website using PHP and MySQL.
   Note: This project is built upon the assumption that a LAMP has been set up. This project will be
   tested on a Linux Ubuntu 32bit OS with LAMP set up on it.
   There will be a login page, a register page, and mail page. Once logged in, the user can
   send an email via the mail page. These emails will be sent out every 30 minutes via a "Cron" job.

   SQL Tables:
   user:
   CREATE TABLE user
   (
      userid int unsigned not null auto_increment primary key,
      username varchar(30),
      password varchar(168)  #Combination of SHA-512 (128 chars) hash and sha1($str) (40 chars) salt
   )
   
   message:
   CREATE TABLE message
   (
      msgid int unsigned not null auto_increment primary key,
      userid varchar(30),
      emailAddress varchar(50),
      message varchar(1000), #Message length 500 chars stored in hex
      timeStamp int(8) unsigned,  #Seconds (int value) since January 1, 1970
      sent BOOL
   )
   
   SQL User (Lower Privileges):
   
   GRANT SELECT, INSERT, UPDATE, FILE ON *.* TO '484Query'@'%' IDENTIFIED BY PASSWORD '*D55FE4B3A70589D434E25A8F99AEEBC10935E487';
   
   
   CRON JOB:
   -Create a .txt file
      -In a the first line write: 0-59/30 * * * * /usr/bin/php /var/wwww/html/mailer.php
      -Hit the enter key to go down to the second line
      -Save and Close
   -In linuxe, open terminal, navigate to file location
      -type the command: crontab <file>.txt
      -Verify with crontab -l
   -Done
   
-->
<!--#############################################################################################-->
<html>
   <!--PHP **************************************************************************************-->
   <?php
      //Begin a session and set variables
      session_start();
      
      //Invalid Arguments?
      $loginFail = false;
      if(isset($_SESSION['invalidArguments'])){
         if($_SESSION['invalidArguments'] == true){
            $loginFail = true;
            //Reset value
            $_SESSION['invalidArguments'] = false;
         }
      }
      //Is this user already logged in?
      //Is this user already logged in?
      if(isset($_SESSION['loggedIn'])){
         if($_SESSION['loggedIn'] == true){
            //Redirect to lobby
            header('Location: lobby.php');
            exit;
         }
      }
   ?>
   <!--END of PHP Constants and Variables *******************************************************-->
   <head>
      <!--Embedded CSS **************************************************************************-->
      <style type="text/css">
         html {
            background-color: black;
         }
         #_loginBox {
            background-color: black;
            position: absolute;
            top: 40%;
            left: 40%;
            width: 30%;
            height: 30%;
         }
         #_loginForm label {
            color: white;
            font-weight: bold;
         }
         #_input_submit {
            color: white;
            background-color: black;
            border-color: white;
            border-width: 1px;
         }
         #_error_report {
            color: red;
            font-weight: bold;
         }
         #_register {
            color: white;
         }
         .error {
            background-color: #FF6666;
         }
      </style>
      <!--END OF Embedded CSS *******************************************************************-->
      <!--Embedded JavaScript *******************************************************************-->
      <!--jQuery-->
      <script type="text/javascript"   
         src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.2/jquery.min.js">
      </script>
      <!--General javascript functions-->
      <script type="text/javascript"
         src="lab4_general_javascript_func.js">
      </script>
      <!--File specific javascript-->
      <script type="text/javascript">
         //---------------------------------------------
         //Handles submission event of user name and 
         //password. Checks are made on the input, 
         //including length and type, and submission is 
         //made.
         //---------------------------------------------
         function clickHandler_submit(e){
            //Local Variables
            var un = document.getElementById("_input_userName");
            var pd = document.getElementById("_input_password");
            
            //Remove previous submission errors
            $(pd).removeClass("error");
            $(un).removeClass("error");
            $("#_error_report").html("");
            
            //Is there input in each field?
            if(un.value == "" || pd.value == ""){
               if(un.value == "")
                  $(un).addClass("error");
               if(pd.value == "")
                  $(pd).addClass("error");
               e.preventDefault();
               return;
            }
            //Valid userName and password format?
            if(!validPasswordFormat(pd.value) || !validUserNameFormat(un.value)){
               un.value = "";
               pd.value = "";
               $("#_error_report").html("Invalid jared User/Password");
               e.preventDefault();
               return;
            }
            
            //Submit values to database checker.
         }
         //---------------------------------------------
         //Set handles for javascript
         //---------------------------------------------
         $(document).ready(function(){
            submitButton = document.getElementById("_input_submit");
            submitButton.addEventListener("click", clickHandler_submit, false);
         });
      
      </script>
      <!--END OF Embedded JavaScript ************************************************************-->
   </head>
   <!--##########################################################################################-->
   <body>
      <div id="_loginBox">
         <form id="_loginForm" method="post" action="loginHandler.php">
            <label id="_label_userName">User Name:</label><br>
            <input id="_input_userName" type="text" name="username"><br><br>
            <label id="_label_password">Password:</label><br>
            <input id="_input_password" type="password" name="password"><br><br>
            <input id="_input_submit" type="submit" value="Login"><br><br>
         </form>
         <!--Error Message-->
         <label id="_error_report"><?php if($loginFail){print("Invalid User/Password");} ?></label>
      
         <!--Registration link-->
         <p id="_register">Not Registered? Click <a href="registration.php">here</a></p>
      
      </div>
      
   </body>
   <!--##########################################################################################-->
</html>
