/***************************************************************************************************
************************         |\/| o  _ ._ _  |  _   _   _.  _           ************************
************************         |  | | (_ | (_) | (/_ (_| (_| (_ \/        ************************
************************                                _|        /         ************************
************************                      Estd. 2015                    ************************
****************************************************************************************************
****************************************************************************************************
**** The use, disclosure, reproduction, modification, transfer, or transmittal of this work for ****
**** any purpose in any form or by any means without the written permission of Microlegacy is   ****
**** strictly prohibited.                                                                       ****
****                                                                                            ****
**** Confidential, Unpublished Property of Microlegacy.                                         ****
**** Use and Distribution Limited Solely to Authorized Personnel.                               ****
****                                                                                            ****
**** All Rights Reserved                                                                        ****
****************************************************************************************************
CREATOR ********************************************************************************************
****************************************************************************************************
****  Name:  Jared Fowler
**** Title:  Co-Founder of Microlegacy
****  Date:  August 8, 2017
**** Email:  jaredwfowler@hotmail.com
****************************************************************************************************
VERSION ********************************************************************************************
****************************************************************************************************
** Version #: 1.0
** Version Date: February 1, 2018
** Revision Personel: JFowler,
***************************************************************************************************/

#ifndef STREAM_STATE_MACHINE_HPP
#define STREAM_STATE_MACHINE_HPP

// ------------------------------------------------------------------------------------------------

// Add a simplified define for windows versus unix system. Also add different behaviors here.

#if defined(_MSC_VER) || defined(__WINDOWS__)

#define WINDOWS_OS

#define SHARED_MEMORY                                                   0
#define HTTPS_SUPPORT                                                   0

#define GET_PID()                                                       (GetCurrentProcessId())
#define SLEEP_SECONDS(A)                                                (Sleep(A))
#define GET_CHAR()                                                      (_getch())

#else

#define UNIX_OS

// Linux will make use of shared memory regions to help speed things up

#define SHARED_MEMORY                                                   1
#define HTTPS_SUPPORT                                                   1

#define GET_PID()                                                       (getpid())
#define SLEEP_SECONDS(A)                                                (usleep(A * 1000))
#define GET_CHAR()                                                      (getchar())

#endif

// ------------------------------------------------------------------------------------------------

// Define MACROS for file acces in shared memeory verse regular file system so that we don't have
// to put a whole bunch of preprocessor defines throughout the code

#if (SHARED_MEMORY)

#define F_FILE                                                          int
#define F_OPEN(A, B, C, D)                                              (shm_open(A, C, D))
#define F_SEEK(A, B, C)                                                 (lseek(A, B, C))
#define F_WRITE(A, B, C, D, E)                                          (write(D, A, E))
#define F_READ(A, B, C, D, E)                                           (read(D, A, E))
#define F_FLUSH(A)                                                      ((int)A)      // Do Nothing
#define F_CLOSE(A)                                                      (close(A))

#define FD_NULL                                                         0
#define FD_FAIL                                                         -1

#else

#define F_FILE                                                          FILE*
#define F_OPEN(A, B, C, D)                                              (fopen(A, B))
#define F_SEEK(A, B, C)                                                 (fseek(A, B, C))
#define F_WRITE(A, B, C, D, E)                                          (fwrite(A, B, C, D))
#define F_READ(A, B, C, D, E)                                           (fread(A, B, C, D))
#define F_FLUSH(A)                                                      (fflush(A))
#define F_CLOSE(A)                                                      (fclose(A))

#define FD_NULL                                                         NULL
#define FD_FAIL                                                         NULL

#endif

// ------------------------------------------------------------------------------------------------

// STL list uses a linked list as its internal structure

#include <list>

// ------------------------------------------------------------------------------------------------

// DEBUG
// Enable debug output to stdout (console)

#define DEBUG                                                           1

// LOG
// Enable logging to stderr (output file)

#define LOG                                                             1

// LOG_ENTRIES_MAX
// Maximum log entries before resetting log file. 0 indicates infinite.

#define LOG_ENTRIES_MAX                                                100000

// SERVER_COMM
// Enable the state machine to send HTTP(S) requests to the server

#define SERVER_COMM                                                     1

// UPDATE_PARAMS
// Enable the state machine to check param file for updates

#define PARAM_UPDATE                                                    0

// The largest request will be that which includes a list of users to remove
// from the invitation list or queue. Give ample room for this.

#define HTPP_REQUEST_BUFFER_SIZE                                        4096

// Buffer used for creating print out messages

#define MESSAGE_BUFFER_SIZE                                             512

// Buffer used for creating the input file name

#define FILE_NAME_BUFFER_SIZE                                           256

// Values direcly correspond to constant values within limelight server

#define USER_NAME_LENGTH_MAX                                            128
#define SERVER_PASSKEY_LENGTH_MAX                                       128

#define FILE_INSTRUCTION_COUNT_SIZE                                     500000
#define FILE_PIPE_SIZE                                                  (FILE_INSTRUCTION_COUNT_SIZE * sizeof(stream_instruction_t))

#define VOLATILE_INSTRUCTION_COUNT_SIZE                                 (FILE_INSTRUCTION_COUNT_SIZE / 2)
#define VOLATILE_INSTRUCTION_SIZE                                       (VOLATILE_INSTRUCTION_COUNT_SIZE * sizeof(stream_instruction_t))

#define VOLATILE_SOCKET_BUFFER_SIZE                                     8192

// Number of input files. Widens the flow of input instructions which
// is at a bottleneck with only 1 file available.

#define INPUT_FILE_COUNT                                                16


// Instruction file offsets. Server input and State Machine output

#define INSTRUCTION_IN_OFFSET                                           0
#define INSTRUCTION_IN_VERIFY_OFFSET                                    4
#define INSTRUCTION_META_OFFSET                                         8
#define INSTRUCTION_META2_OFFSET                                        12
#define INSTRUCTION_START_OFFSET                                        16

#if SHARED_MEMORY

#define INPUT_FILE                                                      "/ll_in_sm"
#define OUTPUT_FILE                                                     "/ll_out_sm"
#define PARAM_FILE                                                      "./FileBuffers/streamStateParams"
#define LOG_FILE                                                        "./FileBuffers/streamStateLog.txt"

#elif defined (UNIX_OS)

#define INPUT_FILE                                                      "./FileBuffers/streamStateIn"
#define OUTPUT_FILE                                                     "./FileBuffers/streamStateOut"
#define PARAM_FILE                                                      "./FileBuffers/streamStateParams"
#define LOG_FILE                                                        "./FileBuffers/streamStateLog"

#else

#define INPUT_FILE                                                      "../../LimeLight/FileBuffers/streamStateIn"
#define OUTPUT_FILE                                                     "../../LimeLight/FileBuffers/streamStateOut"
#define PARAM_FILE                                                      "../../LimeLight/FileBuffers/streamStateParams"
#define LOG_FILE                                                        "../../LimeLight/FileBuffers/streamStateLog"

#endif

// LINK INFO ...

// WAMP style links - Defined in Visual Studio Project Properties, thus, automatically not defined
// in LINUX build. :-)

#ifdef WAMP

#define LINK_PROTOCOL                                                   "http://"
#define LINK_ROOT                                                       "localhost/"
#define LINK_PATH                                                       "limelight_streaming/public/"

#else

#if HTTPS_SUPPORT

#define LINK_PROTOCOL                                                   "https://"

#else

#define LINK_PROTOCOL                                                   "http://"

#endif

#define LINK_ROOT                                                       "limelightstreaming.com/"
#define LINK_PATH                                                       ""

#endif

#define LINK_REQUEST_SERVER_KEY                                         "sm/request_server_key"
#define LINK_REQUEST_SERVER_KEY_POST                                    " "
#define LINK_REQUEST_INVITE_FILL                                        "sm/request_invite_list_fill"
#define LINK_REQUEST_INVITE_FILL_POST                                   "passkey=%s&number=%u"
#define LINK_REQUEST_SOCKET_EVENT                                       "sm/request_socket_event"
#define LINK_REQUEST_SOCKET_EVENT_POST                                  "passkey=%s"
#define LINK_REQUEST_STREAM_CONFIRM                                     "sm/request_stream_confirm"
#define LINK_REQUEST_STREAM_CONFIRM_POST                                "passkey=%s&userID=%u&state=%u"
#define LINK_REQUEST_USER_REMOVAL                                       "sm/request_stream_user_removal"
#define LINK_REQUEST_USER_REMOVAL_POST                                  "passkey=%s&users=%s&clear_all=%u"

// ------------------------------------------------------------------------------------------------

// Debugging Output Options

typedef enum
{
    DEBUG_0, // Random Print Outs
    DEBUG_1, // Asserts and Events
    DEBUG_2, // Invitation Event
    DEBUG_3,
    DEBUG_4,
    DEBUG_5, // Errors
    DEBUG_6, // User Interaction
    DEBUG_7
}
debug_output_t;

// Which of the outputs should we be printing out? (0x00 - NONE , 0xFF - ALL)

#define DEBUG_OUT_OPTIONS                                               0b11111111

// ------------------------------------------------------------------------------------------------

// Limelight server will receive requests via CURL from the state machine. Part of these requests
// will include the passing of parameters. To be consistent with the Server, list here the
// arguments in enum fashion

typedef enum
{
    SM_START_CONFIRM            = 0,
    SM_TERMINATE_CONFIRM        = 1
}
server_parameters_t;

// ------------------------------------------------------------------------------------------------

// Limelight server will communicate with the state machine via a file pipe. This will be handled in
// a a queue fashion with a location pointer in the file for both the server side and state machine
// side. Communication will take the form in stream instructions.

typedef enum
{
    ACTION_ADD_UP_VOTE          = 0,
    ACTION_REMOVE_UP_VOTE       = 1,
    ACTION_ADD_DOWN_VOTE        = 2,
    ACTION_REMOVE_DOWN_VOTE     = 3,
    ACTION_ADD_NULL_VOTE        = 4,
    ACTION_ADD_TO_LIST          = 5,
    ACTION_ADD_TO_QUEUE         = 6,
    ACTION_REMOVE_FROM_LIST     = 7,
    ACTION_REMOVE_FROM_QUEUE    = 8,
    ACTION_REMOVE_FROM_ALL      = 9,

    ACTION_START_CONFIRM        = 50,
    ACTION_TERMINATE_CONFIRM    = 51,
    ACTION_SOCKET_CONFIRM       = 52,
    ACTION_INVITE_CONFIRM       = 53,

    ACTION_ASSEMBLE_ALIAS       = 80,
    ACTION_ASSEMBLE_PASSKEY     = 81,

    ACTION_RESTART_SM           = 128,
    ACTION_REMOVE_STREAMER      = 129,
    ACTION_LOG_INFO             = 130,

    ACTION_ERROR                = 0xFFFFFFFF  // Make this enum into 4 bytes
}
stream_action_t;

typedef struct
{
    union
    {
        stream_action_t action;
        struct
        {
            short int mf_action;
            short int mf_offset;
        };
    };

    union
    {
        unsigned int userID;
        unsigned int argument;
        char characters[4];
    };
}
stream_instruction_t;

// ------------------------------------------------------------------------------------------------

// Parameters will largely determine how the state machine will behave. We will allow manipulation
// of the state machine parameters during run-time via file changes. If the parameter file holds a
// different Major/Minor version, then  the parameters in volatile memory will be changed to match
// those in the file.

typedef struct
{
    unsigned int versionMajor;
    unsigned int versionMinor;

    unsigned int stateMachineTimeout;   // ms
    unsigned int instructEventTimeout;  // state machine cycles
    unsigned int socketEventTimeout;    // state machine cycles
    unsigned int inviteEventTimeout;    // state machine cycles
    unsigned int inviteRespEventTimeout;// state machine cycles
    unsigned int paramUpdateTimeout;    // state machine cylces
    unsigned int serverConfirmTimeout;  // state machine cycles
    unsigned int socketConfirmTimeout;  // socketEventTimeout hit count
    unsigned int inviteConfirmTimeout;  // inviteEventTimeout hit count

    unsigned int streamQueueLengthMax;  // Invite List + Queue Length
    unsigned int streamHailMaryChance;  // N / 1000
    unsigned int maxOpsPerIteration;    // Maximum operations to perform for single iteration

    unsigned int inviteResponseTimeout; // ms
    unsigned int startingStreamTime;    // ms
    unsigned int minimumStreamTime;     // ms
    unsigned int maximumStreamTime;     // ms
    unsigned int deltaTimePerVote;      // ms

    unsigned int warmupTimeout;         // ms
    unsigned int cooldownTimeout;       // ms
}
stream_param_t;

typedef enum
{
    PARAM_DEFAULT,
    PARAM_FILE_SYNC,
    PARAM_FORCE_SYNC
}
update_param_option_t;

// ------------------------------------------------------------------------------------------------

typedef enum name
{
    STREAM_STATUS_NONE,
    STREAM_STATUS_INVITED,
    STREAM_STATUS_QUEUED
}
user_stream_status_t;

typedef struct
{
    unsigned int userID;
    unsigned long long int timeStamp;
}
stream_ticket_t;

#define STREAM_ITERATOR_CONST                           std::list<stream_ticket_t>::const_iterator
#define STREAM_ITERATOR                                 std::list<stream_ticket_t>::iterator

// ------------------------------------------------------------------------------------------------

typedef struct
{
    unsigned int upVotes;
    unsigned int downVotes;
    unsigned int nullVotes;
}
stream_votes_t;

typedef struct
{
    unsigned long long int startTime;
    unsigned long long int expectedEndTime;
}
stream_time_t;

typedef struct
{
    unsigned int id;
    char alias[USER_NAME_LENGTH_MAX];
}
streamer_info_t;

typedef enum
{
    STREAM_STATE_IDLE,
    STREAM_STATE_WAIT_FOR_CONFRIMATION,
    STREAM_STATE_WARMUP,
    STREAM_STATE_INITIALIZING,
    STREAM_STATE_STREAMING,
    STREAM_STATE_TERMINATING,
    STREAM_STATE_COOLDOWN,
    STREAM_STATE_ERROR
}
stream_state_t;

typedef union
{
    unsigned char all;
    struct
    {
        unsigned char startConfirm       : 1;
        unsigned char terminationConfirm : 1;
        unsigned char inviteConfirm      : 1;
        unsigned char socketConfirm      : 1;
        unsigned char debugThread        : 1;
        unsigned char resetSateMachine   : 1;
        unsigned char terminateStream    : 1;
        unsigned char reserved           : 1;
    };
}
stream_flags_t;

typedef enum
{
    STREAM_TIMER_ZERO,
    STREAM_TIMER_STREAM,
    STREAM_TIMER_WARMUP,
    STREAM_TIMER_COOLDOWN
}
stream_timer_t;

typedef struct
{
    stream_state_t state;
    stream_flags_t flags;

    streamer_info_t streamer;
    stream_time_t time;
    stream_votes_t votes;

    std::list<stream_ticket_t> invitationList;
    std::list<stream_ticket_t> queue;
}
stream_info_t;

// ------------------------------------------------------------------------------------------------

extern int StreamStateMachine();

#endif
