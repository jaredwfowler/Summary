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
-->

<html>
   <!--PHP Constants and Variables **************************************************************-->
   <?php
      //Flags
      $unt = false;
      $sqlFail = false;
   
      session_start();
      //Set-up, or reset error variables
      if(isset($_SESSION['userNameTaken'])){
         if($_SESSION['userNameTaken'] == true){
            $_SESSION['userNameTaken'] = false;
            $unt = true;
         }
      }
      if(isset($_SESSION['sqlFail'])){
         if($_SESSION['sqlFail'] == true){
            $_SESSION['sqlFail'] = false;
            $sqlFail = true;
         }
      }
   ?>
   <!--END of PHP Constants and Variables *******************************************************-->
   <!--##########################################################################################-->
   <head>
      <!--Embedded CSS **************************************************************************-->
      <style type="text/css">
         html {
            background-color: black;
         }
         #_registerBox {
            background-color: black;
            position: absolute;
            top: 40%;
            left: 40%;
            width: 30%;
            height: 30%;
         }
         #_registerForm label {
            color: white;
            font-weight: bold;
         }
         #_input_submit {
            color: white;
            background-color: black;
            border-color: white;
            border-width: 1px;
         }
         #_infoBox {
            background-color: black;
            position: absolute;
            top: 70%;
            left: 40%;
            width: 30%;
         }
         #_info {
            color: white;
         }
         #_error_report {
            color: red;
            font-weight: bold;
         }
         .error {
            background-color: #FF6666;
         }
      </style>
      <!--END OF Embedded CSS *******************************************************************-->
      <!--Embedded JavaScript *******************************************************************-->
      <script type="text/javascript" 
         src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.2/jquery.min.js">
      </script>
      <script type="text/javascript"
         src="lab4_general_javascript_func.js">
      </script>
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
            
            //Valid userName?
            if(!validUserNameFormat(un.value)){
               $("#_error_report").html("Invalid User Name Format");
               e.preventDefault();
               return;
            }
            
            //Valid password?
            if(!validPasswordFormat(pd.value)){
               $("#_error_report").html("Invalid Password Format");
               e.preventDefault();
               return;
            }
            
            //Submit values to database checker.
         }
         //---------------------------------------------
         //Set handles for javascript
         //---------------------------------------------
         $(document).ready(function(){
            var submitButton = document.getElementById("_input_submit");
            submitButton.addEventListener("click", clickHandler_submit, false);
            
            //Focus Display information
            $("#_input_userName").focus(function(){
               $("#_info").html("Maximum of 30 characters, no spaces.");
            });
            $("#_input_password").focus(function(){
               $("#_info").html("8-50 characters long. Must have at least 1 letter, 1 number, and 1 special character.");
            });
            //Blur Event - Hide information
            $("#_input_userName").blur(function(){
               $("#_info").html("");
            });
            $("#_input_password").blur(function(){
               $("#_info").html("");
            });
         });
      
      </script>
      <!--END OF Embedded JavaScript ************************************************************-->
   </head>
   <!--##########################################################################################-->
   <body>
      <div id="_registerBox">
         <form id="_registerForm" method="post" action="registrationHandler.php">
            <label id="_label_userName">User Name:</label><br>
            <input id="_input_userName" type="text" name="username"><br><br>
            <label id="_label_password">Password:</label><br>
            <input id="_input_password" type="password" name="password"><br><br>
            <input id="_input_submit" type="submit" value="Register"><br><br>
         </form>
         <!--Error Message-->
         <?php
            if($sqlFail)
               print('<label id="_error_report" >Connection Error...</label>');
            else if($unt)
               print('<label id="_error_report" >User Name Already Taken.</label>');
            else
               print('<label id="_error_report" ></label>');
         ?>
      </div>
      <div id="_infoBox">
         <p id="_info"></p>
      </div>
      
   </body>
   <!--##########################################################################################-->
</html>