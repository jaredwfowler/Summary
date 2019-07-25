================================================================================
About: A general setup guide for limelight
================================================================================

Architecture:

                                          (cURL Queries)
                             -----------------------------------------
                            |                                         |
                            v                                         |
                      -------------        -------------        -------------
           ---------> |  Laravel  | <----> |   File    | <----> |   State   |
          |           |   Server  | <--    |  Buffers  |        |  Machine  |
          |           -------------   |    -------------        -------------
          |                 |         \
          |                 |          \
          v                 v           \
     ------------     -------------      \   -------------
     |Andr./IOS |     |   Redis   |       |  |   MySQL   |
     |   App    |     |  Database |       -->|  Databse  |
     ------------     -------------          -------------
          ^                 |
          |                 |
          |                 v
          |           -------------
          |           |  NodeJS   |
           ---------> |  Socket   |
                      |  Server   |
                      -------------


Update (April 4, 2018)
I've added a handy batch command which will start three servers: State Machine,
Redis, and NodeJS. Located in the root folder of the project, but cmd line to
run. (_RunServers.bat)

--------------------------------------------------------------------------------
State Machine:
--------------------------------------------------------------------------------
About:
Laravel is a PHP server. It is unable to run an ongoing algorithm to analyze
the current invited users, those in queue, and other ongoing tasks. This is
where the state machine comes into play. The state machine will handle all the
logic pertaining to the streaming states and tasks, such as determining how many
users should be invited, stream time left, etc., etc.. The state machine will
talk to the Server via file buffers and direct queries.

Setup:
The state machine is written in C++ and works on both windows and unix
platforms. The machine has a dependency upon CURL and the existence of the
file buffers. For windows, the appropriate CURL libraries have been included in
the project. For linux, you'll have to install the curl developer libraries.

    sudo apt-get install libcurl4-openssl-dev

The file buffers are conveniently located inside the project's directory. You'll
have to possibly re-position the file buffers, dependent upon where you execute
the binary.

I've been using Visual Studio to make the project on windows. I've created a
make file to build it on unix.

Check the following line in Limelight/include/StreamStateMachine.h :

    #define LINK_ROOT                xxx.xxx.xxx.xxx

, and ensure that the ip address or domain name is properly set. Change it if
necessary, then rebuild the solution.

For Linux, we are going to set the State Machine up so it runs reliably and by
itself when the system starts up. Setup pm2:

    sudo npm install -g pm2

Go to the scripts location (private/app_data/StreamStateMachine/Limelight)
and use:

    pm2 start ll_sm

Create script and install it (copy/paste/enter generated script):

    pm2 startup

Save it:

    pm2 save

Run:
On Windows use command prompt to start executable.

If setup correctly on Linux, it should run automatically on system startup.

--------------------------------------------------------------------------------
File Buffers:
--------------------------------------------------------------------------------
About:
Communication between state machine and server primarily occurs via file
buffers. There are two main files of interest. One is an input instruction file,
which the server writes to and the state machine reads. These instructions
inform the state machine of activities, such as adding an upvote or downvote.
The second file is an output file from the state machine. This is the stream
info which we'll want to send out over the socket to the user's application.

Setup:
You may have to modify the write permissions to these buffers using chmod and
chown.

--------------------------------------------------------------------------------
MySQL Database:
--------------------------------------------------------------------------------
About:
Basic database information, such as user information, vote tables, etc..

Setup:
Automatically setup with vestacp. You'll need to manually setup the DB,
username, and password, and use Laravel's migration tool to create the tables.
The user table has been pre-populated with 'bots' in order to assist in
debugging efforts.

Run:
Should run automatically on system startup, or WAMP startup for windows.

--------------------------------------------------------------------------------
Laravel Server:
--------------------------------------------------------------------------------
About:

Setup:
Most setup will occur in the .env file. Most of the default values are in
.env.example, including the setting to use redis for broadcasting. The fields
which will need to be filled out pertain to Database authentication, app key,
and public server path.

See furhter documentation below for server setup...

Run:
Should run automatically on system startup, or WAMP startup for windows.

--------------------------------------------------------------------------------
Redis:
--------------------------------------------------------------------------------
About:
Redis is a noSQL database. Redis is the bridge used between Laravel server and
NodeJS server. Very simply, redis is a basic key-value type database.

Setup:
Redis has ongoing support for unix, but its support for windows has dwindled.
I've included in the project folder under StreamSocket/RedisForWindows and
StreamSocket/RedisForLinux binary executables which can be ran via command line.

For Linux, we are going to set redis up so it runs reliably and by itself when
the system starts up. Navigate to the StreamSocket/RedisForLinux folder and do
the following commands:

    sudo cp redis-server /usr/local/bin
    sudo cp redis-cli /usr/local/bin

    sudo mkdir /etc/redis
    sudo mkdir /var/redis
    sudo cp redis_init_script /etc/init.d/redis_6379
    sudo cp redis.conf /etc/redis/6379.conf
    sudo mkdir /var/redis/6379

    Edit the config file:

    sudo nano /etc/redis/6379.conf

      - Set daemonize to yes (by default it is set to no).
      - Set the pidfile to /var/run/redis_6379.pid (modify the port if needed).
      - Change the port accordingly. In our example it is not needed as the
        default port is already 6379.
      - Set your preferred loglevel.
      - Set the logfile to /var/log/redis_6379.log
      - Set the dir to /var/redis/6379 (very important step!)

    sudo update-rc.d redis_6379 defaults
    sudo /etc/init.d/redis_6379 start

For both Windows and Linux, we'll need to make sure predis is installed in
Laravel.

    composer require predis/predis

Also in Laravel, ensure that the "BROADCAST_DRIVER" value in the .env file is
set to redis.

Depending on the server, remember to allow a firewall rule for socket 3000 on
both vestacp and server provider rules.

Run:
On Windows, run the stand-alone executable. If setup correctly on Linux, it
should run automatically on system startup.

--------------------------------------------------------------------------------
NodeJS Socket Server:
--------------------------------------------------------------------------------
About:
NodeJS handles socket connection with the application.

Setup:
Install NodeJS, along with npm, on Windows using a downloaded installer. Install
version 8.x or later. On Linux, use the following commands:

    sudo curl -sL https://deb.nodesource.com/setup_8.x | sudo -E bash -
    sudo apt-get install -y nodejs

Now, install the various npm packages when inside private/app_data

    sudo npm install express ioredis socket.io --save

For Linux, we are going to set nodeJS up so it runs reliably and by itself when
the system starts up. Setup pm2:

    sudo npm install -g pm2

Go to the scripts location (private/app_data/StreamSocket/) and use:

    pm2 start socket.js

Create script and install it (copy/paste/enter generated script):

    pm2 startup

Save it:

    pm2 save

Depending on the server, remember to allow a firewall rule for socket 3000 on
both vestacp and server provider rules.

Run:
On Windows, run a node server with:

    node socket.js

If setup correctly on Linux, it should run automatically on system startup.


================================================================================
Server Instance Setup
================================================================================

In setting up the server on VPSDime which hosts multiple domains, I discovered
these various gotchas:

Ensure that shared memory is both on or off in both state machine and in app/constants.php

Remember that only users that have recently pinged will be selected for invite... duh..

Use command to upgrade UNIX user to sudo level
usermod -aG sudo username

Make sure that the state machine executible is owned by limelightstreaming, not
root, and run it without sudo. Run it on PM2 under limelightstreaming.
The umask value has been updated to hopefully fix permission issues. Ensure that
permissions in the files, probably /dev/shm/ in linux, are set correctly.

Run node server under root in PM2, as it needs special access to certificate files.

