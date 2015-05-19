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
      //Include general php functions
      include 'lab4_general_php_func.php';
      
      session_start();
      $_SESSION['userNameTaken'] = false;
      $_SESSION['sqlFail'] = false;
      
      //Get user inputs
      $userName = $_POST['username'];
      $password = $_POST['password'];
      
      //UserName
      if(!validateUserNameFormat($userName)){
         invalidArguments();
      }
      
      //Password
      if(!validatePasswordFormat($password)){
         invalidArguments();
      }
      
      //The input formats are good. Does the userName already exist?
      //Connect to the database
      if(!($sql = connectToDatabase())){
         sqlFail();
      }
      //Does the user exist?
      $query = "SELECT * FROM user WHERE username='$userName'";
      //Run query
      if(!($queryResult = mysqli_query($sql, $query))){
         sqlFail();
      }
      if(mysqli_num_rows($queryResult) != 0){ 
         //User Name is already taken
         userNameUsed();
      }
      
      //Generate a salt, get a salted-hashed password appended to salt
      $salt = generateSalt();
      $password = getHashedPassword($salt, $password);
      $password = $salt . $password;
         
      //Add userName and password to database
      $query = "INSERT INTO user (username, password) VALUES ('$userName', '$password')";
      //Run query
      if(!($queryResult = mysqli_query($sql, $query))){
         sqlFail();
      }
      
      //Disconnect from the database
      mysqli_close($sql);
      
      //---------------------------------------------
      //Called if user name already taken
      //---------------------------------------------
      function userNameUsed(){
         mysqli_close($sql);
         $_SESSION['userNameTaken'] = true;
         header('Location: registration.php');
         exit;
      }
      //---------------------------------------------
      //Called if sql login fail
      //---------------------------------------------
      function sqlFail(){
         mysqli_close($sql);
         $_SESSION['sqlFail'] = true;
         header('Location: registration.php');
         exit;
      }
      //---------------------------------------------
      //Called if invalid userName or password
      //---------------------------------------------
      function invalidArguments(){
         header('Location: lab4.php');
         exit;
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
         #_register {
            color: white;
         }
         #_reg_result {
            font-weight: bold;
            color: green;
         }
      </style>
      <!--END OF Embedded CSS *******************************************************************-->
      <!--Embedded JavaScript *******************************************************************-->
      <script type="text/javascript" >
         //Void
      </script>
      <!--END OF Embedded JavaScript ************************************************************-->
   </head>
   <!--##########################################################################################-->
   <body>
      <!--Return to login link-->
      <p id="_reg_result">Thank you!</p>
      <p id="_register">Click <a href="lab4.php">here</a> to return to login page.</p>
      
   </body>
   <!--##########################################################################################-->
</html>
