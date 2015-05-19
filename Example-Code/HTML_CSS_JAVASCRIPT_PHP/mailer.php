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
   /*This file was created to handle the sending of existing messages in the database.
   This file is called via a CRON job, that is, an event called by unix at a specified
   time. This will be called every 30 minutes, at which time we will connect to the DB,
   check message send times, and send anything where the time has past.
   
   http://stackoverflow.com/questions/712392/send-email-using-the-gmail-smtp-server-from-a-php-page
   --Using swift mailer
   */
     
   //Include general use php file
   include 'lab4_general_php_func.php';
   require_once 'swift/lib/swift_required.php';
   
   //Connect to the database
   if(($sql = connectToDatabase()) == false){
      fail();
   }
   
   //Get the current epoch time
   $curTime = time();
   
   //Request from the DB a list of entries where time to send is < curTime
   $query = "SELECT * FROM message WHERE timeStamp <'$curTime' AND sent = 'false'";
   
   //Run query
   if(!($queryResult = mysqli_query($sql, $query))){
      fail();
   }
   $rows = mysqli_num_rows($queryResult);
   if($rows == 0){ 
      fail();
   }
   
   //We have work to do...
   for($i = 0; $i < $rows; $i++){
      
      //Get the information for the next row
      $msgInfo = mysqli_fetch_assoc($queryResult);
      
      //Prepare to send the message
      $_sendTo = $msgInfo['emailAddress'];
      $_msg = $msgInfo['message'];
      //Decode the message
      $_msg = messageDecode($_msg);
      //Get the message id (key)
      $_msgID = $msgInfo['msgid'];
      
      $transport = Swift_SmtpTransport::newInstance('smtp.gmail.com', 465, 'ssl')
         //->setUsername('jared.fowler.publicsender@gmail.com')
         //->setPassword('thatWasFun!');
         ->setUsername('comp424group1@gmail.com')
         ->setPassword('space!!space!!');

      $mailer = Swift_Mailer::newInstance($transport);

      $message = Swift_Message::newInstance('484_EMAIL')
         //->setFrom(array('jared.fowler.publicsender@gmail.com' => 'jared'))
         ->setFrom(array('comp424group1@gmail.com'=>'Sender'))
         ->setTo(array($_sendTo))
         ->setBody($_msg);

      $result = $mailer->send($message);
      
      //Update DB sent field
      $query = "UPDATE message SET sent='1' WHERE msgid='$_msgID'";
   
      //Run query -- If it fails, oh well...
      mysqli_query($sql, $query);
   }
   
   //Disconnect from the database
   mysqli_close($sql);
   exit;
   
   //---------------------------------------------
   //Returns direction of pages to login page
   //---------------------------------------------
   function fail(){
      //Close the sql connection 
      mysqli_close($sql);
      exit;
   }
   
?>