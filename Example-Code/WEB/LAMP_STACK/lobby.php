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
      $messageFail = false;
      $messageSent = false;
      
      //Session Information
      session_start();
      //Are we here legally?
      if($_SESSION['loggedIn'] == false){
         //Redirect to login page
         header('Location: lab4.php');
         exit;
      }
      //De we just send a message?
      if($_SESSION['msgSent'] == true){
         $messageSent = true;
         $_SESSION['msgSent'] = false;
      }
      //Did we just fail to send a message?
      if($_SESSION['msgError'] == true){
         $messageFail = true;
         $_SESSION['msgError'] = false;
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
         #_mailerBox {
            background-color: black;
            position: absolute;
            top: 5%;
            left: 5%;
            bottom: 5%;
            right: 50%;
         }
         #_mailerForm label {
            color: white;
            font-weight: bold;
         }
         #_input_message {
            width: 75%;
         }
         #_input_submit, #log_out {
            color: white;
            background-color: black;
            border-color: white;
            border-width: 1px;
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
      <!--General javascript functions-->
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
            
            //Local Handlers
            var email = document.getElementById("_input_email");
            var message = document.getElementById("_input_message");
            var error = document.getElementById("_error_report");
            var tzOffset = document.getElementById("_input_tzOffset");
            
            //Remove previous submission errors
            $(email).removeClass("error");
            $(message).removeClass("error");
            $(error).html("");
            
            //Empty fields?
            if(email.value == "" || message.value == ""){
               if(email.value == "")
                  $(email).addClass("error");
               if(message.value == "")
                  $(message).addClass("error");
               e.preventDefault();
               return;
            }
            
            //Valid email address?
            if(!validEmailFormat(email.value)){
               $(error).html("Invalid Email");
               e.preventDefault();
               return;
            }
            
            //Valid message length?
            if(message.length > MAX_MSG_LEN){
               $(error).html("Message Exceeds 500 Char Limit");
               e.preventDefault();
               return;
            }
            
            //Get the timezone offset and write value
            var offset = new Date().getTimezoneOffset();
            $(tzOffset).val(offset);
            
            //Submit values to database checker.
         }
         //---------------------------------------------
         //Set handles for javascript
         //---------------------------------------------
         $(document).ready(function(){
            var submitButton = document.getElementById("_input_submit");
            submitButton.addEventListener("click", clickHandler_submit, false);
            
            var logoutButton = document.getElementById("log_out");
            logoutButton.addEventListener("click", function(e){
               window.location = 'logoutHandler.php'
            }, false);
         });
      
      </script>
      <!--END OF Embedded JavaScript ************************************************************-->
   </head>
   <!--##########################################################################################-->
   <body>
      <div id="_mailerBox">
         <form id="_mailerForm" method="post" action="messageHandler.php">
            <label id="_label_email">email: (50 Chars Max)</label><br>
            <input id="_input_email" type="text" name="email"><br><br>
            <label id="_label_date_time">Date/Time:</label><br>
            
            <select id="_input_date_day" name="day">
               <option value="1">1</option>
               <option value="2">2</option>
               <option value="3">3</option>
               <option value="4">4</option>
               <option value="5">5</option>
               <option value="6">6</option>
               <option value="7">7</option>
               <option value="8">8</option>
               <option value="9">9</option>
               <option value="10">10</option>
               <option value="11">11</option>
               <option value="12">12</option>
               <option value="13">13</option>
               <option value="14">14</option>
               <option value="15">15</option>
               <option value="16">16</option>
               <option value="17">17</option>
               <option value="18">18</option>
               <option value="19">19</option>
               <option value="20">20</option>
               <option value="21">21</option>
               <option value="22">22</option>
               <option value="23">23</option>
               <option value="24">24</option>
               <option value="25">25</option>
               <option value="26">26</option>
               <option value="27">27</option>
               <option value="28">28</option>
               <option value="29">29</option>
               <option value="30">30</option>
               <option value="31">31</option>
            </select>
            <select id="_input_date_month" name="month">
               <option value="1">January</option>
               <option value="2">February</option>
               <option value="3">March</option>
               <option value="4">April</option>
               <option value="5">May</option>
               <option value="6">June</option>
               <option value="7">July</option>
               <option value="8">August</option>
               <option value="9">September</option>
               <option value="10">October</option>
               <option value="11">November</option>
               <option value="12">December</option>
            </select>
            <select id="_input_date_year" name="year">
               <option value="2015">2015</option>
               <option value="2016">2016</option>
               <option value="2017">2017</option>
            </select>
            <br><br>
            <label> @ </label>
            <select id="_input_time_hour" name="hour">
               <option value="0">00</option>
               <option value="1">01</option>
               <option value="2">02</option>
               <option value="3">03</option>
               <option value="4">04</option>
               <option value="5">05</option>
               <option value="6">06</option>
               <option value="7">07</option>
               <option value="8">08</option>
               <option value="9">09</option>
               <option value="10">10</option>
               <option value="11">11</option>
               <option value="12">12</option>
               <option value="13">13</option>
               <option value="14">14</option>
               <option value="15">15</option>
               <option value="16">16</option>
               <option value="17">17</option>
               <option value="18">18</option>
               <option value="19">19</option>
               <option value="20">20</option>
               <option value="21">21</option>
               <option value="22">22</option>
               <option value="23">23</option>
            </select>
            <label>:</label>
            <select id="_input_time_minute" name="minute">
               <option value="0">00</option>
               <option value="30">30</option>
            </select>
            <br><br>
            <label id="_label_message">message: (500 Chars Max)</label><br>
            <input id="_input_message" type="text" name="message"><br><br>
            
            <input id="_input_tzOffset" type="hidden" name="tzOffset" value="0">
            
            <input id="_input_submit" type="submit" value="Save"><br><br>
         </form>
         
         <!--Error Message-->
         <label id="_error_report">
         <?php 
            if($messageFail){print("Failed to Send.");} 
            else if($messageSent){print("SENT!");}
         ?></label>
         <br><br>
         <!--Log Out Option-->
         <button id="log_out">Log Out</button>
      </div>
      
   </body>
   <!--##########################################################################################-->
</html>







