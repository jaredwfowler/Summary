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
** Version #: 2.0
** Version Date: February 1, 2018
** Revision Personel: JFowler,
***************************************************************************************************/

#include "StreamStateMachine.hpp"
#include "HttpDownloader.hpp"

#include <stdio.h>
#include <ctime>
#include <string.h>
#include <thread>
#include <stdlib.h>

#if defined(WINDOWS_OS)

#include <Windows.h>
#include <conio.h>

#else

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#endif

// ------------------------------------------------------------------------------------------------

// I fully understand that globals are not reccommended in best programming  practices, however,
// for the simplicity of this application and since I am the only one modifying it, I don't see
// it as that big of a deal.

static stream_info_t streamInfo;
static stream_param_t streamParam;

static char messageAry[MESSAGE_BUFFER_SIZE] = { 0 };
static char fileNameAry[FILE_NAME_BUFFER_SIZE] = { 0 };

// Instead of writing multiple times to a file, or reading multiple times, it's a lot faster to
// store the information in volatile memory and do the file read/write once. These buffers are
// for the purpose of holding volatile file information.

static unsigned char InputInstructionBuffer[VOLATILE_INSTRUCTION_SIZE] = { 0 };
static unsigned char OutputSocketBuffer[VOLATILE_SOCKET_BUFFER_SIZE] = { 0 };

// A special key is needed to make special requests to the server, which should only be made by
// this state machine. The state machine will ping the server and request the passkey upon
// startup.

static char ServerPasskey[SERVER_PASSKEY_LENGTH_MAX] = { 0 };
static HttpDownloader httpDownloader;
static char httpRequestAry[2][HTPP_REQUEST_BUFFER_SIZE] = { 0 };

// It is required that the state machine offset is global because we need to reset it in the
// case of a state machine reset.

static unsigned int sm_offset[INPUT_FILE_COUNT] = { INSTRUCTION_START_OFFSET };

// Other Debugging Helpers

static unsigned char PrintDebug = 0;
static unsigned int LastServerOffset[INPUT_FILE_COUNT];
static unsigned int LastSMOffset[INPUT_FILE_COUNT];
static unsigned int InputFileIndex = 0;

// ------------------------------------------------------------------------------------------------

// Deafult param values. This will automatically be copied into the param structure on startup. The
// param structure will then be overwritten by the param file, if it exists

const stream_param_t STREAM_PARAM_DEFAULTS =
{
    0,      // versionMajor
    1,      // versionMinor

    20,     // stateMachineTimeout
    1,      // instructEventTimout
    45,     // socketEventTimeout
    235,    // inviteEventTimeout
    75,     // inviteRespEventTimeout
    500,    // paramUpdateTimeout
    200,    // serverConfirmTimeout
    10,     // socketConfirmTimeout
    10,     // inviteConfirmTimeout

    20,     // streamQueueLengthMax
    100,    // streamHailMaryChance
    2000,   // maxOpsPerIteration

    10000,  // inviteResponseTimeout
    60000,  // startingStreamTime
    10000,  // minimumStreamTime
    300000, // maximumStreamTime
    5000,   // deltaTimePerVote

    10000,  // warmupTimeout
    10000,  // cooldownTimeout
};

// ------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//Name:
// AddUserToInvitationList
//GF
//Arguments:
// userID             -  User ID
//
//Returns:
// void
//
//About:
// Adds provided user to invitation list. Ensures that user isn't already in the streaming queue
// and not already in the invitation list
//
//--------------------------------------------------------------------------------------------------

unsigned long long int TimeStampNowMS(void)
{
    return (1000 * (unsigned long long int)time(0));
}

//--------------------------------------------------------------------------------------------------
//Name:
// EventLog
//
//Arguments:
// Char pointer to output string
//
//Returns:
// void
//
//About:
// Convenient wrapper for log messages. Outputs a timestamp and the mesage. Flushes the stream.
//
//--------------------------------------------------------------------------------------------------

void EventLog(const char* message, unsigned char priority)
{
    static unsigned int startTime = 0xFFFFFFFF;
    static unsigned int process_id = 0;
    static unsigned int logEntries = 0;

    if (startTime == 0xFFFFFFFF)
    {
        startTime = (unsigned int)time(0);
        process_id = GET_PID();
    }

    #if LOG

    if (LOG_ENTRIES_MAX && (logEntries >= LOG_ENTRIES_MAX))
    {
        freopen(LOG_FILE, "w", stderr);
        logEntries = 0;
    }

    logEntries++;
    fprintf(stderr, "PID: %5u | TS: %8u | %s\r\n", process_id, ((unsigned int)time(0) - startTime), message);
    fflush(stderr);

    #endif

    #if DEBUG

    if ((PrintDebug) && ((1 << priority) & DEBUG_OUT_OPTIONS))
    {
        fprintf(stdout, "PID: %5u | TS: %8u | %s\r\n", process_id, ((unsigned int)time(0) - startTime), message);
        fflush(stdout);
    }

    #endif
}

//--------------------------------------------------------------------------------------------------
//Name:
// TotalStreamTime
//
//Arguments:
// void
//
//Returns:
// Total stream time
//
//About:
// Simple arithmatic which calculates the total stream time from start to expected end
//
//--------------------------------------------------------------------------------------------------

int TotalStreamTime(void)
{
    return (int)(streamInfo.time.expectedEndTime - streamInfo.time.startTime);
}

//--------------------------------------------------------------------------------------------------
//Name:
// StreamTimeLeft
//
//Arguments:
// void
//
//Returns:
// Stream time left in ms
//
//About:
// Simple arithmatic which determines the stream time left
//
//--------------------------------------------------------------------------------------------------

int StreamTimeLeft(void)
{
    int timeLeft = (int)(streamInfo.time.expectedEndTime - TimeStampNowMS());

    if (timeLeft < 0)
    {
        timeLeft = 0;
    }

    return timeLeft;
}

//--------------------------------------------------------------------------------------------------
//Name:
// AddStreamTime
//
//Arguments:
// time               -  Ammount of time (ms) to add to the current stream time
//
//Returns:
// void
//
//About:
// Adds given ms to stream time. Does bounds check to make sure stream time doesn't go over maximum
//
//--------------------------------------------------------------------------------------------------

void AddStreamTime(unsigned int time)
{
    // Only allow addition/subtraction of stream time when in Streaming mode
    if (streamInfo.state == STREAM_STATE_STREAMING)
    {
        if (((streamInfo.time.expectedEndTime - streamInfo.time.startTime) + time) > streamParam.maximumStreamTime)
        {
            streamInfo.time.expectedEndTime = streamParam.maximumStreamTime + streamInfo.time.startTime;
        }
        else
        {
            streamInfo.time.expectedEndTime += time;
        }
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// SubtractStreamTime
//
//Arguments:
// time               -  Ammount of time (ms) to subtract from the current stream time
//
//Returns:
// void
//
//About:
// Removes given ms to stream time. Does bounds check to make sure stream time doesn't go under min
//
//--------------------------------------------------------------------------------------------------

void SubtractStreamTime(unsigned int time)
{
    // Only allow addition/subtraction of stream time when in Streaming mode
    if (streamInfo.state == STREAM_STATE_STREAMING)
    {
        if (((streamInfo.time.expectedEndTime - streamInfo.time.startTime) - time) < streamParam.minimumStreamTime)
        {
            streamInfo.time.expectedEndTime = streamParam.minimumStreamTime + streamInfo.time.startTime;
        }
        else
        {
            streamInfo.time.expectedEndTime -= time;
        }
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// ResetStreamTimer
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Resets the stream start time to current time, and stream end time to default stream time start.
// The timers use the same time structure to hold information. The user can use the streaming state
// to find out which timer is currently active.
//
//--------------------------------------------------------------------------------------------------

void ResetStreamTimer(stream_timer_t timerType)
{
    streamInfo.time.startTime = TimeStampNowMS();

    // Set expected end time based upon timer type

    switch (timerType)
    {
    case STREAM_TIMER_ZERO:
        streamInfo.time.expectedEndTime = streamInfo.time.startTime;
        break;

    case STREAM_TIMER_STREAM:
        streamInfo.time.expectedEndTime = streamInfo.time.startTime + streamParam.startingStreamTime;
        break;

    case STREAM_TIMER_WARMUP:
        streamInfo.time.expectedEndTime = streamInfo.time.startTime + streamParam.warmupTimeout;
        break;

    case STREAM_TIMER_COOLDOWN:
        streamInfo.time.expectedEndTime = streamInfo.time.startTime + streamParam.cooldownTimeout;
        break;

    default:
        EventLog("Error. Reset Stream Timer - Default Case...", DEBUG_5);
        break;
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// DecrementStreamUpVote
//
//Arguments:
// void
//
//Returns:
// PASS (0) / FAIL (!0)
//
//About:
// Removes a single up vote. Does bounds checks to make sure value is 0 or greater.
//
//--------------------------------------------------------------------------------------------------

int DecrementStreamUpVote(void)
{
    int error = 0;

    streamInfo.votes.upVotes -= 1;

    if ((int)streamInfo.votes.upVotes < 0)
    {
        streamInfo.votes.upVotes = 0;
        error = -1;
    }

    return error;
}

//--------------------------------------------------------------------------------------------------
//Name:
// DecrementStreamDownVote
//
//Arguments:
// void
//
//Returns:
// PASS (0) / FAIL (!0)
//
//About:
// Removes a single down vote. Does bounds checks to make sure value is 0 or greater.
//
//--------------------------------------------------------------------------------------------------

int DecrementStreamDownVote(void)
{
    int error = 0;

    streamInfo.votes.downVotes -= 1;

    if ((int)streamInfo.votes.downVotes < 0)
    {
        streamInfo.votes.downVotes = 0;
        error = -1;
    }

    return error;
}

//--------------------------------------------------------------------------------------------------
//Name:
// IncrementStreamUpVote
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Increments stream up vote
//
//--------------------------------------------------------------------------------------------------

void IncrementStreamUpVote(void)
{
    streamInfo.votes.upVotes += 1;
}

//--------------------------------------------------------------------------------------------------
//Name:
// IncrementStreamDownVote
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Increments stream down vote
//
//--------------------------------------------------------------------------------------------------

void IncrementStreamDownVote(void)
{
    streamInfo.votes.downVotes += 1;
}

//--------------------------------------------------------------------------------------------------
//Name:
// IncrementStreamNullVote
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Increments stream null vote
//
//--------------------------------------------------------------------------------------------------

void IncrementStreamNullVote(void)
{
    streamInfo.votes.nullVotes += 1;
}

//--------------------------------------------------------------------------------------------------
//Name:
// ResetStreamVotes
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Resets the stream votes
//
//--------------------------------------------------------------------------------------------------

void ResetStreamVotes(void)
{
    streamInfo.votes.upVotes = 0;
    streamInfo.votes.downVotes = 0;
    streamInfo.votes.nullVotes = 0;
}

//--------------------------------------------------------------------------------------------------
//Name:
// WriteParametersToFile
//
//Arguments:
// void
//
//Returns:
// PASS (0) / FAIL (!0)
//
//About:
// Writes the current parameter values to the parameter file, overwriting all content in the file.
//
//--------------------------------------------------------------------------------------------------

int WriteDefaultParametersToFile(void)
{
    int error = 0;

    FILE* paramFile = fopen(PARAM_FILE, "rb+");

    // We cannot operate without the input and output files..

    if (paramFile == NULL)
    {
        fprintf(stdout, "Error. File Dycrypter NULL.\r\n");
        return -1;
    }

    // Reset file pointer to beggining of file
    fseek(paramFile, 0, SEEK_SET);

    error = (int)fwrite((void*)(&STREAM_PARAM_DEFAULTS), sizeof(stream_param_t), 1, paramFile);
    fflush(paramFile);
    fclose(paramFile);

    return error;
}

//--------------------------------------------------------------------------------------------------
//Name:
// ExtractFileParameters
//
//Arguments:
// stream_param_t     -  Pre-initialized param structure to copy read file data into
//
//Returns:
// PASS (0) / FAIL (!0)
//
//About:
// Reads the parameter data into the passed in structure
//
//--------------------------------------------------------------------------------------------------

int ExtractFileParameters(stream_param_t* sp)
{
    int error = 0;
    FILE* paramFile = fopen(PARAM_FILE, "rb+");

    // Does the file exist and can we open it?

    if (paramFile == NULL)
    {
        fprintf(stdout, "Error. File Dycrypter NULL.\r\n");
        return -1;
    }

    // Cannot work with a NULL pointer
    if (sp == NULL)
    {
        error = -1;
        goto EXTRACT_PARAM_CLEANUP;
    }

    // Check against file size
    fseek(paramFile, 0, SEEK_END);

    // If file is empty, then fill in the file with the default parameter values. Else, if the
    // file size and parameter struct sizes don't match up, throw an error.

    if (ftell(paramFile) == 0)
    {
        WriteDefaultParametersToFile();
    }
    else if (ftell(paramFile) < sizeof(stream_param_t))
    {
        //fprintf(stderr, "Size File: %u  Size Struct: %u \r\n", ftell(paramFile), sizeof(stream_param_t));
        error = -2;
        goto EXTRACT_PARAM_CLEANUP;
    }

    // Reset file pointer to beggining of file
    fseek(paramFile, 0, SEEK_SET);

    // Copy file content into structure
    if (fread((void*)sp, sizeof(stream_param_t), 1, paramFile) == 0)
    {
        error = -3;
        goto EXTRACT_PARAM_CLEANUP;
    }

    EXTRACT_PARAM_CLEANUP:

    fclose(paramFile);

    return 0;
}

//--------------------------------------------------------------------------------------------------
//Name:
// UpdateVolatileParameters
//
//Arguments:
// update_param_option_t
//
//Returns:
// int                -  (0) No Update Needed. (1) Updated (-1) Error
//
//About:
// Updates the volatile copy of the parameter values. Behavior dependent upon input option:
//   PARAM_DEFAULT    -  Params defaulted to STREAM_PARAMS_DEFAULT
//   PARAM_FILE_SYNC  -  Param file is read. If major/minor version vary, the parameters will be
//                       updated to match the files
//   PARAM_FORCE_SYNC -  Regardless of major/minor file versions, the parameters will be updated
//                       to match the values in the file
//
//--------------------------------------------------------------------------------------------------

int UpdateVolatileParameters(update_param_option_t option)
{
    int error = 0;

    if ((option == PARAM_FILE_SYNC) || (option == PARAM_FORCE_SYNC))
    {
        // Read the context of the param file into a param structure

        stream_param_t paramTemp;
        error = ExtractFileParameters(&paramTemp);
        if (!error)
        {
            if ((option == PARAM_FORCE_SYNC) ||
                ((option == PARAM_FILE_SYNC) &&
                ((paramTemp.versionMajor != streamParam.versionMajor) ||
                    (paramTemp.versionMinor != streamParam.versionMinor))))
            {
                memcpy(&streamParam, &paramTemp, sizeof(stream_param_t));
                sprintf(messageAry, "Parameters Updated. Version: %u.%u", streamParam.versionMajor,
                                                                            streamParam.versionMinor);
                EventLog(messageAry, DEBUG_6);
                error = 1; // Updated
            }
            else
            {
                EventLog("Parameters Version Same. No Update", DEBUG_0);
            }
        }
        else
        {
            // Error Condition
            sprintf(messageAry, "UpdateVolatileParameters Error. Error Code: %d", error);
            EventLog(messageAry, DEBUG_5);
        }
    }
    else
    {
        // Default Parameters
        memcpy(&streamParam, &STREAM_PARAM_DEFAULTS, sizeof(stream_param_t));
    }

    return error;
}

//--------------------------------------------------------------------------------------------------
//Name:
// UserSelectionStatus
//
//Arguments:
// userID             -  User ID
//
//Returns:
// Current list/queue status of user in user_stream_status_t
//
//About:
// Determines if a given user is currently in the invitation list, queued for streaming, or none
//
//--------------------------------------------------------------------------------------------------

user_stream_status_t UserSelectionStatus(unsigned int userID)
{
    STREAM_ITERATOR_CONST iterator;
    for (iterator = streamInfo.invitationList.begin(); iterator != streamInfo.invitationList.end(); iterator++)
    {
        if ((*iterator).userID == userID)
        {
            return STREAM_STATUS_INVITED;
        }
    }

    for (iterator = streamInfo.queue.begin(); iterator != streamInfo.queue.end(); iterator++)
    {
        if ((*iterator).userID == userID)
        {
            return STREAM_STATUS_QUEUED;
        }
    }

    return STREAM_STATUS_NONE;
}

//--------------------------------------------------------------------------------------------------
//Name:
// RemoveUserFromInvitationList
//
//Arguments:
// userID             -  User ID
//
//Returns:
// void
//
//About:
// Removes provided user from the invitation list
//
//--------------------------------------------------------------------------------------------------

void RemoveUserFromInvitationList(unsigned int userID)
{
    STREAM_ITERATOR iterator;
    for (iterator = streamInfo.invitationList.begin(); iterator != streamInfo.invitationList.end(); iterator++)
    {
        if ((*iterator).userID == userID)
        {
            streamInfo.invitationList.erase(iterator);
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// ExtractFileParameters
//
//Arguments:
// FILE               -  Pointer to file decrypter for parameter file
// stream_param_t     -  Pre-initialized param structure to copy read file data into
//
//Returns:
// void
//
//About:
// Reads the parameter data into the passed in structure
//
//--------------------------------------------------------------------------------------------------

void RemoveUserFromStreamingQueue(unsigned int userID)
{
    STREAM_ITERATOR iterator;
    for (iterator = streamInfo.queue.begin(); iterator != streamInfo.queue.end(); iterator++)
    {
        if ((*iterator).userID == userID)
        {
            streamInfo.queue.erase(iterator);
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// AddUserToInvitationList
//
//Arguments:
// userID             -  User ID
//
//Returns:
// void
//
//About:
// Adds provided user to invitation list. Ensures that user isn't already in the streaming queue
// and not already in the invitation list
//
//--------------------------------------------------------------------------------------------------

void AddUserToInvitationList(unsigned int userID)
{
    stream_ticket_t ticket = { userID, TimeStampNowMS() };
    user_stream_status_t status = UserSelectionStatus(userID);

    switch (status)
    {
        default:
        case STREAM_STATUS_INVITED:
            // Do nothing
            break;

        case STREAM_STATUS_QUEUED:
            RemoveUserFromStreamingQueue(userID);
            streamInfo.invitationList.push_back(ticket);
            break;

        case STREAM_STATUS_NONE:
            streamInfo.invitationList.push_back(ticket);
            break;
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// EnqueueUserToStreamingQueue
//
//Arguments:
// userID             -  User ID
//
//Returns:
// void
//
//About:
// Enqueues provided user into the streaming queue
//
//--------------------------------------------------------------------------------------------------

void EnqueueUserToStreamingQueue(unsigned int userID)
{
    stream_ticket_t ticket = { userID, TimeStampNowMS() };
    user_stream_status_t status = UserSelectionStatus(userID);

    switch (status)
    {
    default:
    case STREAM_STATUS_INVITED:
        RemoveUserFromInvitationList(userID);
        streamInfo.queue.push_back(ticket);
        break;

    case STREAM_STATUS_QUEUED:
        // Do Nothing
        break;

    case STREAM_STATUS_NONE:
        streamInfo.queue.push_back(ticket);
        break;
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// RemoveTimedOutUsersFromInvitationList
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Iterates through the invitation list and removes those users who have timed out. Sends request
// message to the server to update DB
//
//--------------------------------------------------------------------------------------------------

void RemoveTimedOutUsersFromInvitationList(void)
{
    char usersToRemoveString[1024] = { 0 };  // NOTE: Max of 256 users to remove.
    time_t timeNow = TimeStampNowMS();
    STREAM_ITERATOR iterator;
    std::string res;

    // Because we are relying on iterator position in the for loop, don't erase values until we find out
    // the complete range of values to erase.

    STREAM_ITERATOR endRange = streamInfo.invitationList.begin();
    bool elementsToErase = false;

    for (iterator = streamInfo.invitationList.begin(); iterator != streamInfo.invitationList.end(); iterator++)
    {
        if ((timeNow - (*iterator).timeStamp) >= streamParam.inviteResponseTimeout)
        {
            sprintf(messageAry, "Removing userID %u", (*iterator).userID);
            EventLog(messageAry, DEBUG_2);

            sprintf(usersToRemoveString, "%s;%u", usersToRemoveString, (*iterator).userID);
            elementsToErase = true;
            endRange = iterator;
        }
        else
        {
            // Because the invitation list is handled like a queue, the first user we come across
            // that has not timed out is a sufficient stopping point.

            // Remove the last delimiter from the string
            usersToRemoveString[strlen(usersToRemoveString)] = 0;

            break;
        }
    }

    // Erase elements

    if (elementsToErase)
    {
        // Elements to erase range include lower limit but doesn't includeu upper.  (first, last]

        streamInfo.invitationList.erase(streamInfo.invitationList.begin(), ++endRange);
    }

    // Send a request to the server to remove user IDs from invitation list.

    if (usersToRemoveString[0] != 0)
    {
        sprintf(httpRequestAry[0], LINK_REQUEST_USER_REMOVAL_POST, ServerPasskey, usersToRemoveString, 0);
        sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_USER_REMOVAL);
        #if SERVER_COMM
        res = httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
        #endif
        //fprintf(stdout, "%s\r\n", res.c_str());
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// TickleServerInvitationEvent
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Determines how many slots are available in the streaming queue. Sends this value to the laravel
// server with a request to select up to this many people to be invited to stream.
//
//--------------------------------------------------------------------------------------------------

void TickleServerInvitationEvent(void)
{
     int spotsAvailable = streamParam.streamQueueLengthMax -
        (unsigned int)(streamInfo.invitationList.size() + streamInfo.queue.size());

     std::string res;

    // Check for negative value
    if (spotsAvailable < 0)
    {
        spotsAvailable = 0;
    }

    sprintf(httpRequestAry[0], LINK_REQUEST_INVITE_FILL_POST, ServerPasskey, spotsAvailable);
    sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_INVITE_FILL);
    #if SERVER_COMM
    res = httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
    #endif
    //fprintf(stdout, "%s\r\n", res.c_str());

    sprintf(messageAry, "CURL - Invitation Event : %s", ((res == "") ? "FAIL" : "PASS"));
    EventLog(messageAry, DEBUG_0);
    sprintf(messageAry, "Tickling server invitation event for %u users", spotsAvailable);
    EventLog(messageAry, DEBUG_1);
}

//--------------------------------------------------------------------------------------------------
//Name:
// PrintStreamInfo
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Pretty prints stream structure information
//
//--------------------------------------------------------------------------------------------------

void PrintStreamInfo(FILE* out)
{
    STREAM_ITERATOR iterator;

    fprintf(out, "\r\n\n");

    fprintf(out, "Stream State: %u\r\n", streamInfo.state);

    fprintf(out, "Flags:\r\n");
    fprintf(out, "      Start Confirm: %20s\r\n", ((streamInfo.flags.startConfirm) ? "TRUE" : "FALSE"));
    fprintf(out, "  Terminate Confirm: %20s\r\n", ((streamInfo.flags.terminationConfirm) ? "TRUE" : "FALSE"));
    fprintf(out, "     Invite Confirm: %20s\r\n", ((streamInfo.flags.inviteConfirm) ? "TRUE" : "FALSE"));
    fprintf(out, "     Socket Confirm: %20s\r\n", ((streamInfo.flags.socketConfirm) ? "TRUE" : "FALSE"));
    fprintf(out, "       Debug Thread: %20s\r\n", ((streamInfo.flags.debugThread) ? "TRUE" : "FALSE"));
    fprintf(out, "           Reset SM: %20s\r\n", ((streamInfo.flags.resetSateMachine) ? "TRUE" : "FALSE"));
    fprintf(out, "   Terminate Stream: %20s\r\n", ((streamInfo.flags.terminateStream) ? "TRUE" : "FALSE"));

    fprintf(out, "Streamer ID: %u. Alias: %s\r\n", streamInfo.streamer.id, streamInfo.streamer.alias);
    fprintf(out, "Start Time: %llu. Expected End Time: %llu. Time Left: %d.\r\n", streamInfo.time.startTime, streamInfo.time.expectedEndTime, StreamTimeLeft());
    fprintf(out, "Up Votes: %u, Down Votes: %u, Null Votes: %u\r\n", streamInfo.votes.upVotes, streamInfo.votes.downVotes, streamInfo.votes.nullVotes);

    fprintf(out, "\r\n");

    for (unsigned int i = 0; i < INPUT_FILE_COUNT; i++)
    {
        fprintf(out, "Server Offset %2u: %2u\r\n", i, LastServerOffset[i]);
        fprintf(out, "    SM Offset %2u: %2u\r\n", i, LastSMOffset[i]);
    }

    fprintf(out, "\r\n");
    fprintf(out, "Sever Passkey: %s\r\n", ServerPasskey);

    fprintf(out, "\r\n");
    fprintf(out, "Invite List:\r\n");

    for (iterator = streamInfo.invitationList.begin(); iterator != streamInfo.invitationList.end(); iterator++)
    {
        fprintf(out, "{%u,%llu},", (*iterator).userID, (*iterator).timeStamp);
    }

    fprintf(out, "\r\n\n");
    fprintf(out, "Queue:\r\n");

    for (iterator = streamInfo.queue.begin(); iterator != streamInfo.queue.end(); iterator++)
    {
        fprintf(out, "{%u,%llu},", (*iterator).userID, (*iterator).timeStamp);
    }

    fprintf(out, "\r\n\n");
}

//--------------------------------------------------------------------------------------------------
//Name:
// UpdateSocketOutput
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Updates the contents of the file with the current stream information which will be
// eventaully transmitted to the users.
//
// File Format:
//
//   State Machine Status : 1 byte
//   Flags                : 1 byte
//   Streamer ID          : 4 bytes
//   Alias Length         : 2 (Make multiple of 4)
//   Alias                : (Alias Length * 1) bytes
//   Start Time (ms)      : 8 bytes
//   End Time (ms)        : 8 bytes
//   Time Left (ms)       : 4 bytes
//   Up Votes             : 4 bytes
//   Down Votes           : 4 bytes
//   Null Votes           : 4 bytes
//   invitationList_count : 4 bytes
//   invitationList       : (invitationList_count * 4) bytes
//   streamQueue_count    : 4 bytes
//   streamQueue          : (streamQueue_count * 4) bytes
//
//--------------------------------------------------------------------------------------------------

void UpdateSocketOutput(void)
{
    unsigned int tempVar;
    STREAM_ITERATOR iterator;
    unsigned int buffOffset = 0;

    F_FILE socketFile = F_OPEN(OUTPUT_FILE, "rb+", O_RDWR, 0775);

    // We cannot operate without the input and output files..

    if (socketFile == FD_FAIL)
    {
        fprintf(stdout, "Error. File Dycrypter NULL.\r\n");
        return;
    }

    // Clear the volatile buffer
    memset(OutputSocketBuffer, 0, sizeof(OutputSocketBuffer));

    // State Machine Status
    memcpy((OutputSocketBuffer + buffOffset), (void*)(&streamInfo.state), sizeof(unsigned char));
    buffOffset += sizeof(unsigned char);

    // Flags
    memcpy((OutputSocketBuffer + buffOffset), (void*)(&streamInfo.flags), sizeof(unsigned char));
    buffOffset += sizeof(unsigned char);

    // Streamer ID
    memcpy((OutputSocketBuffer + buffOffset), (void*)(&streamInfo.streamer.id), sizeof(unsigned int));
    buffOffset += sizeof(unsigned int);

    // Streamer Alias Length
    tempVar = (unsigned int)strlen(streamInfo.streamer.alias);
    if (tempVar % 4)
    {
        tempVar += (4 - (tempVar % 4));
    }
    memcpy((OutputSocketBuffer + buffOffset), (void*)(&tempVar), (sizeof(unsigned int) / 2));
    buffOffset += (sizeof(unsigned int) / 2);

    // Streamer Alias
    memcpy((OutputSocketBuffer + buffOffset), (void*)(&streamInfo.streamer.alias), sizeof(char) * tempVar);
    buffOffset += (sizeof(char) * tempVar);

    // Start Time and End Time
    memcpy((OutputSocketBuffer + buffOffset), (void*)(&streamInfo.time), sizeof(stream_time_t));
    buffOffset += sizeof(stream_time_t);

    // Time Left
    int timeLeft = StreamTimeLeft();
    memcpy((OutputSocketBuffer + buffOffset), (void*)(&timeLeft), sizeof(int));
    buffOffset += sizeof(int);

    // Votes
    memcpy((OutputSocketBuffer + buffOffset), (void*)(&streamInfo.votes), sizeof(stream_votes_t));
    buffOffset += sizeof(stream_votes_t);

    // Invitation list
    tempVar = (unsigned int)streamInfo.invitationList.size();

    memcpy((OutputSocketBuffer + buffOffset), (void*)(&tempVar), sizeof(unsigned int));
    buffOffset += sizeof(unsigned int);

    for (iterator = streamInfo.invitationList.begin(); iterator != streamInfo.invitationList.end(); iterator++)
    {
        tempVar = (*iterator).userID;

        memcpy((OutputSocketBuffer + buffOffset), (void*)(&tempVar), sizeof(unsigned int));
        buffOffset += sizeof(unsigned int);
    }

    // Streaming queue
    tempVar = (unsigned int)streamInfo.queue.size();

    memcpy((OutputSocketBuffer + buffOffset), (void*)(&tempVar), sizeof(unsigned int));
    buffOffset += sizeof(unsigned int);

    for (iterator = streamInfo.queue.begin(); iterator != streamInfo.queue.end(); iterator++)
    {
        tempVar = (*iterator).userID;

        memcpy((OutputSocketBuffer + buffOffset), (void*)(&tempVar), sizeof(unsigned int));
        buffOffset += sizeof(unsigned int);
    }

    // Write contents to socket file

    // Reset the file pointer to the beginning of the file.
    F_SEEK(socketFile, 0, SEEK_SET);

    // Write buffer to file
    F_WRITE((void*)OutputSocketBuffer, sizeof(unsigned char), buffOffset, socketFile, (buffOffset * sizeof(unsigned char)));

    // Flush to make sure file update is completed
    F_FLUSH(socketFile);

    F_CLOSE(socketFile);
    socketFile = FD_NULL;
}

//--------------------------------------------------------------------------------------------------
//Name:
// TickleServerSocketEvent
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Sends a request to the server which will cause the server to check the socket file, our output
// file, and send out an updated message to all those connect via web socket
//
//--------------------------------------------------------------------------------------------------

void TickleServerSocketEvent(void)
{
    std::string res;

    sprintf(httpRequestAry[0], LINK_REQUEST_SOCKET_EVENT_POST, ServerPasskey);
    sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_SOCKET_EVENT);
    #if SERVER_COMM
    res = httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
    #endif
    //fprintf(stdout, "%s\r\n", res.c_str());

    EventLog("Tickling server socket", DEBUG_1);
    sprintf(messageAry, "CURL - Socket Event : %s", ((res == "") ? "FAIL" : "PASS"));
    EventLog(messageAry, DEBUG_0);
}

//--------------------------------------------------------------------------------------------------
//Name:
// AssembleMultiFrameCommand
//
//Arguments:
// stream_instruction_t -  Stream instruction
//
//Returns:
// PASS (0) / FAIL (!0)
//
//About:
// Handles all multi-frame inputs. Switch determines which value we are fillling.
//
//--------------------------------------------------------------------------------------------------

int AssembleMultiFrameCommand(stream_instruction_t instruction)
{
    int error = 0;

    char* buffer = NULL;
    unsigned int max_length = 0;

    // Determine which buffer we are trying to write to

    switch (instruction.mf_action)
    {
        case ACTION_ASSEMBLE_ALIAS:
            buffer = streamInfo.streamer.alias;
            max_length = USER_NAME_LENGTH_MAX;
            sprintf(messageAry, "Assemble Alias. Frame: %u", instruction.mf_offset);
            EventLog(messageAry, DEBUG_1);
            break;

        case ACTION_ASSEMBLE_PASSKEY:
            buffer = ServerPasskey;
            max_length = SERVER_PASSKEY_LENGTH_MAX;
            sprintf(messageAry, "Assemble Passkey. Frame: %u", instruction.mf_offset);
            EventLog(messageAry, DEBUG_1);
            break;

        default:
            error = -2;
            return error;
            break;
    }

    // Bytes 2-3 of action determines the offset of the string of 6 bytes
    unsigned char offset = ((instruction.action & 0xFFFF0000) >> 16);

    // Check for offset validity
    if ((sizeof(char) * 4 * offset) >= max_length)
    {
        error = -1;
    }
    else
    {
        // The last 4 bytes are data
        char* bufferOffset = (buffer + (sizeof(char) * 4 * offset));
        memcpy((void*)bufferOffset, (void*)instruction.characters, sizeof(char) * 4);
    }

    return error;
}

//--------------------------------------------------------------------------------------------------
//Name:
// ProcessStreamInstruction
//
//Arguments:
// stream_instruction_t -  Stream instruction
//
//Returns:
// PASS (0) / FAIL (!0)
//
//About:
// Performs the approrpirate action based upon stream ID and action
//
//--------------------------------------------------------------------------------------------------

int ProcessStreamInstruction(stream_instruction_t instruction)
{
    int error = 0;

    // We only care about the least significant 16 bits of the action. This way we can re-purpose
    // the remaining 16 bits

    unsigned int action = (instruction.action & 0xFFFF);

    switch (action)
    {
    case ACTION_ADD_UP_VOTE:
        EventLog("Adding UP vote", DEBUG_1);
        IncrementStreamUpVote();
        AddStreamTime(streamParam.deltaTimePerVote);
        break;

    case ACTION_REMOVE_UP_VOTE:
        if (!DecrementStreamUpVote())
        {
            EventLog("Removing UP vote", DEBUG_1);
            SubtractStreamTime(streamParam.deltaTimePerVote);
        }
        else
        {
            error = -1;
        }
        break;

    case ACTION_ADD_DOWN_VOTE:
        EventLog("Adding DOWN vote", DEBUG_1);
        IncrementStreamDownVote();
        SubtractStreamTime(streamParam.deltaTimePerVote);
        break;

    case ACTION_REMOVE_DOWN_VOTE:
        if (!DecrementStreamDownVote())
        {
            EventLog("Removing DOWN vote", DEBUG_1);
            AddStreamTime(streamParam.deltaTimePerVote);
        }
        else
        {
            error = -2;
        }
        break;

    case ACTION_ADD_NULL_VOTE:
        EventLog("Adding NULL vote", DEBUG_1);
        IncrementStreamNullVote();
        break;

    case ACTION_ADD_TO_LIST:
        sprintf(messageAry, "Adding user %u to list", instruction.userID);
        EventLog(messageAry, DEBUG_2);
        AddUserToInvitationList(instruction.userID);
        break;

    case ACTION_REMOVE_FROM_LIST:
        sprintf(messageAry, "Removing user %u from list", instruction.userID);
        EventLog(messageAry, DEBUG_2);
        RemoveUserFromInvitationList(instruction.userID);
        break;

    case ACTION_ADD_TO_QUEUE:
        sprintf(messageAry, "Adding user %u to queue", instruction.userID);
        EventLog(messageAry, DEBUG_2);
        EnqueueUserToStreamingQueue(instruction.userID);
        break;

    case ACTION_REMOVE_FROM_QUEUE:
        sprintf(messageAry, "Removing user %u from queue", instruction.userID);
        EventLog(messageAry, DEBUG_2);
        RemoveUserFromStreamingQueue(instruction.userID);
        break;

    case ACTION_REMOVE_FROM_ALL:
        sprintf(messageAry, "Removing user %u from list/queue", instruction.userID);
        EventLog(messageAry, DEBUG_2);
        RemoveUserFromInvitationList(instruction.userID);
        RemoveUserFromStreamingQueue(instruction.userID);
        // If the user is currently streaming, set the terminate stream flag
        if (instruction.userID == streamInfo.streamer.id)
        {
            streamInfo.flags.terminateStream = 1;
        }
        break;

    case ACTION_START_CONFIRM:
        EventLog("Start Confirm Asserted", DEBUG_1);
        streamInfo.flags.startConfirm = 1;
        break;

    case ACTION_TERMINATE_CONFIRM:
        EventLog("Terminate Confirm Asserted", DEBUG_1);
        streamInfo.flags.terminationConfirm = 1;
        break;

    case ACTION_SOCKET_CONFIRM:
        EventLog("Socket Confirm Asserted", DEBUG_1);
        streamInfo.flags.socketConfirm = 1;
        break;

    case ACTION_INVITE_CONFIRM:
        EventLog("Invite Confirm Asserted", DEBUG_1);
        streamInfo.flags.inviteConfirm = 1;
        break;

    case ACTION_ASSEMBLE_ALIAS:
    case ACTION_ASSEMBLE_PASSKEY:
        AssembleMultiFrameCommand(instruction);
        break;

    case ACTION_RESTART_SM:
        EventLog("State Machine Restart Requested", DEBUG_1);
        streamInfo.flags.resetSateMachine = 1;
        break;

    case ACTION_REMOVE_STREAMER:
        EventLog("Remove Streamer Requested", DEBUG_1);
        streamInfo.flags.terminateStream = 1;
        break;

    case ACTION_LOG_INFO:
        EventLog("Print Info Requested", DEBUG_1);
        PrintStreamInfo(stderr);
        break;

    default:
        error = -3;
        break;
    }

    return error;
}

//--------------------------------------------------------------------------------------------------
//Name:
// ProcessStreamInstructions
//
//Arguments:
// FILE               -  Pointer to file decrypter for parameter file
// maxOps             -  Maximum number of operations to handle for this iteration.
//                       A value of 0 will handle all available instructions
//
//Returns:
// void
//
//About:
// Iterates through the provided file. Extracts instructions and passes instructions to be performed
//
// File Format
//   Server Offset Pointer        : Bytes 0 - 3
//   Server Offset Verify POinter : Bytes 4 - 7
//   File META Data               : Bytes 8 - 11
//   State Machine Offset Pointer : Bytes 12 - 15
//
//   Instructions are 8 bytes :
//      action   : 4 bytes
//      argument : 4 bytes
//
// Continue iteration until state machine pointer reaches server offset pointer, or until max ops
// is reached. If we get to the end of the file, then reset pointer to instruction start, which
// starts on byte INSTRUCTION_START_OFFSET.
//
// As a recent added fix, there are now 2 values which indicate the Server position in the file.
// I was running into an issue where the offset value would be 0 as the Server was writing to the
// value. This second value gives us an extra layer of sanity check to ensure that the current
// offset we are looking at is correct.
//
// I'm once again updating this to not write to the file.. this will be a read only funciton. This
// is to avoid the possibility of overwriting changes input from the server side. The offset will
// be replaced by more meta information.
//
//--------------------------------------------------------------------------------------------------

void ProcessStreamInstructions(unsigned int maxOps)
{
    unsigned int svr_offset = 0;
    unsigned int svr_offset_2 = 0;

    unsigned int opReadLimit = 0;
    unsigned int completedOps = 0;
    unsigned int ops = 0;

    sprintf(fileNameAry, "%s_%u", INPUT_FILE, InputFileIndex);
    F_FILE fd = F_OPEN(fileNameAry, "rb", O_RDONLY, 0775);

    // We cannot operate without the input and output files..

    if (fd == FD_FAIL)
    {
        fprintf(stdout, "Error. File Dycrypter NULL.\r\n");
        return;
    }

    // Read the first few bytes of the file to determine the server's offset

    F_SEEK(fd, INSTRUCTION_IN_OFFSET, SEEK_SET);
    F_READ((void*)(&svr_offset), sizeof(unsigned int), 1, fd, (1 * sizeof(unsigned int)));
    F_SEEK(fd, INSTRUCTION_IN_VERIFY_OFFSET, SEEK_SET);
    F_READ((void*)(&svr_offset_2), sizeof(unsigned int), 1, fd, (1 * sizeof(unsigned int)));

    // Verify that the server offset has settled. Do this by comparing the two offset values
    // for server. Ensure that they are equal, and NOT equal to 0.

    if ((svr_offset != svr_offset_2) || (svr_offset == 0) || (svr_offset_2 == 0))
    {
        sprintf(messageAry, "Server Offset Mis-match. 1: %u 2: %u", svr_offset, svr_offset_2);
        EventLog(messageAry, DEBUG_0);
        goto PROCESS_STREAM_INSTURCT_CLEANUP;
    }

    // Update offset debugging variables

    LastServerOffset[InputFileIndex] = svr_offset;
    LastSMOffset[InputFileIndex] = sm_offset[InputFileIndex];

    if (sm_offset[InputFileIndex] != svr_offset)
    {
        sprintf(messageAry, "Instruction Offsets. SM: %u Server: %u", sm_offset[InputFileIndex], svr_offset); 
        EventLog(messageAry, DEBUG_0);
    }

    // Determine the real maximum operations based upon instruction pointers.

    if (sm_offset[InputFileIndex] > svr_offset)
    {
        ops = (((svr_offset - INSTRUCTION_START_OFFSET) / sizeof(stream_instruction_t)) +
            ((FILE_PIPE_SIZE - sm_offset[InputFileIndex]) / sizeof(stream_instruction_t)));
    }
    else
    {
        ops = ((svr_offset - sm_offset[InputFileIndex]) / sizeof(stream_instruction_t));
    }
    maxOps = (maxOps < ops) ? maxOps : ops;

    // While we still have operations to complete ...
    while (completedOps < maxOps)
    {
        // Navigate to the instruction offset
        F_SEEK(fd, sm_offset[InputFileIndex], SEEK_SET);

        // The limit of operations for each file read is determined by maxOps and sizeof the buffer
        opReadLimit = ((maxOps - completedOps) > VOLATILE_INSTRUCTION_COUNT_SIZE) ? VOLATILE_INSTRUCTION_COUNT_SIZE : (maxOps - completedOps);

        // Do we need to make it to the end of the file and possibly loop?
        if (sm_offset[InputFileIndex] > svr_offset)
        {
            if ((FILE_PIPE_SIZE - sm_offset[InputFileIndex]) > (opReadLimit * sizeof(stream_instruction_t)))
            {
                // Read op limit, update new sm_offset to new location

                F_READ((void*)(&InputInstructionBuffer), sizeof(stream_instruction_t), opReadLimit, fd, (opReadLimit * sizeof(stream_instruction_t)));
                ops = opReadLimit;
                sm_offset[InputFileIndex] += (sizeof(stream_instruction_t) * opReadLimit);
            }
            else
            {
                // Read the remainder of the file and update sm_offset to instruction start offset

                F_READ((void*)(&InputInstructionBuffer), sizeof(stream_instruction_t), ((FILE_PIPE_SIZE - sm_offset[InputFileIndex]) / sizeof(stream_instruction_t)), fd, (((FILE_PIPE_SIZE - sm_offset[InputFileIndex]) / sizeof(stream_instruction_t)) * sizeof(stream_instruction_t)));
                ops = ((FILE_PIPE_SIZE - sm_offset[InputFileIndex]) / sizeof(stream_instruction_t));
                sm_offset[InputFileIndex] = INSTRUCTION_START_OFFSET;
            }
        }
        else
        {
            if ((svr_offset - sm_offset[InputFileIndex]) > (opReadLimit * sizeof(stream_instruction_t)))
            {
                // Read op limit, update new sm_offset to new location

                F_READ((void*)(&InputInstructionBuffer), sizeof(stream_instruction_t), opReadLimit, fd, (opReadLimit * sizeof(stream_instruction_t)));
                ops = opReadLimit;
                sm_offset[InputFileIndex] += (sizeof(stream_instruction_t) * opReadLimit);
            }
            else
            {
                // Read the remainder of the instructions and update sm_offset to new location

                F_READ((void*)(&InputInstructionBuffer), sizeof(stream_instruction_t), ((svr_offset - sm_offset[InputFileIndex]) / sizeof(stream_instruction_t)), fd, (((svr_offset - sm_offset[InputFileIndex]) / sizeof(stream_instruction_t)) * sizeof(stream_instruction_t)));
                ops = ((svr_offset - sm_offset[InputFileIndex]) / sizeof(stream_instruction_t));
                sm_offset[InputFileIndex] = svr_offset;
            }
        }

        // Iterate through the operations

        for (unsigned int i = 0; i < ops; i++)
        {
            ProcessStreamInstruction(((stream_instruction_t*)InputInstructionBuffer)[i]);
            completedOps += 1;
        }
    }

    PROCESS_STREAM_INSTURCT_CLEANUP:

    F_CLOSE(fd);
    fd = FD_NULL;

    // Update the input file inddex

    InputFileIndex += 1;
    if (InputFileIndex >= INPUT_FILE_COUNT)
    {
        InputFileIndex = 0;
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// InitializeStreamStateMachine
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Handles initialization of streaming state machine
//
//--------------------------------------------------------------------------------------------------

void InitializeStreamStateMachine(void)
{
    F_FILE fileFD = FD_NULL;

    #if defined(UNIX_OS)

    // Umask is needed to allow setting of proper file permissions.

    umask(0);

    #endif

    // Set volatile parmeters to default values

    memcpy(&streamParam, &STREAM_PARAM_DEFAULTS, sizeof(stream_param_t));

    #if PARAM_UPDATE

    // Create the parameter file if not created and truncate it.

    fileFD = fopen(PARAM_FILE, "wb+");
    F_CLOSE(fileFD);
    fileFD = FD_NULL;

    // Sync Parameters

    UpdateVolatileParameters(PARAM_FILE_SYNC);

    #endif

    // Create the instruction input file(s), size it, and write initial META data values

    for (unsigned int i = 0; i < INPUT_FILE_COUNT; i++)
    {
        sprintf(fileNameAry, "%s_%u", INPUT_FILE, i);
        fileFD = F_OPEN(fileNameAry, "wb+", O_CREAT | O_RDWR | O_TRUNC, 0775);

        // We cannot operate without the input and output files..

        if (fileFD == FD_FAIL)
        {
            fprintf(stdout, "Error. File Dycrypter NULL - Initialize 1.\r\n");
            exit(1);
        }

        // Initialize the instructions file to have server and state machine offsets at same location
        // and fill the rest of the file with 1's

        unsigned int buffer = INSTRUCTION_START_OFFSET;
        F_SEEK(fileFD, 0, SEEK_SET);
        F_WRITE((void*)(&buffer), sizeof(unsigned int), 1, fileFD, (1 * sizeof(unsigned int)));  // SVR OFFSET 1
        F_WRITE((void*)(&buffer), sizeof(unsigned int), 1, fileFD, (1 * sizeof(unsigned int)));  // SVR OFFSET 2
        F_WRITE((void*)(&buffer), sizeof(unsigned int), 1, fileFD, (1 * sizeof(unsigned int)));  // META DATA
        F_WRITE((void*)(&buffer), sizeof(unsigned int), 1, fileFD, (1 * sizeof(unsigned int)));  // META DATA 2

        buffer = 0xFFFFFFFF;
        for (unsigned int i = 0; i < ((FILE_INSTRUCTION_COUNT_SIZE - 2) * (sizeof(stream_instruction_t) / sizeof(unsigned int))); i++)
        {
            F_WRITE((void*)(&buffer), sizeof(unsigned int), 1, fileFD, (1 * sizeof(unsigned int)));
        }
        F_FLUSH(fileFD);
        F_CLOSE(fileFD);
        fileFD = FD_NULL;
    }

    // Create the socket output file and truncate if it already exists

    fileFD = F_OPEN(OUTPUT_FILE, "wb+", O_CREAT|O_RDWR|O_TRUNC, 0755);

    // We cannot operate without the input and output files..

    if (fileFD == FD_FAIL)
    {
        fprintf(stdout, "Error. File Dycrypter NULL - Initialize 2.\r\n");
        exit(1);
    }
    F_CLOSE(fileFD);
    fileFD = FD_NULL;

    // Initialize stream data handlers

    streamInfo.state = STREAM_STATE_IDLE;

    streamInfo.invitationList.clear();
    streamInfo.queue.clear();

    streamInfo.streamer.id = 0;
    strncpy(streamInfo.streamer.alias, "LimeLight", sizeof(streamInfo.streamer.alias));

    ResetStreamTimer(STREAM_TIMER_ZERO);
    ResetStreamVotes();

    streamInfo.flags.all = 0xFF;
    streamInfo.flags.reserved = 0;

    // Reset the state machine input file offset

    for (unsigned int i = 0; i < INPUT_FILE_COUNT; i++)
    {
        sm_offset[i] = INSTRUCTION_START_OFFSET;
    }

    // Request Server Passkey

    sprintf(httpRequestAry[0], LINK_REQUEST_SERVER_KEY_POST);
    sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_SERVER_KEY);
    #if SERVER_COMM
    httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
    #endif

    // Wait for a little while, then attempt to assemble the server key

    SLEEP_SECONDS(3);
    ProcessStreamInstructions(streamParam.maxOpsPerIteration);

    // Reset any lingering streaming statuses

    sprintf(httpRequestAry[0], LINK_REQUEST_USER_REMOVAL_POST, ServerPasskey, ";", 1);
    sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_USER_REMOVAL);
    #if SERVER_COMM
    httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
    #endif
}

//--------------------------------------------------------------------------------------------------
//Name:
// Debug_AddInstructionToFile
//
//Arguments:
// action                - Instruction to add
// argument              - Associated arguments
//
//Returns:
// void
//
//About:
// Adds an instruction to the input instruction file
//
//--------------------------------------------------------------------------------------------------

void Debug_AddInstructionToFile(unsigned int action, unsigned int argument)
{
    static char fileNameAryDBG[FILE_NAME_BUFFER_SIZE] = { 0 };
    static unsigned int inputFileIndex = 0;

    // Open the instruction file

    F_FILE debugFilePtr_input = FD_NULL;

    sprintf(fileNameAryDBG, "%s_%u", INPUT_FILE, inputFileIndex);
    debugFilePtr_input = F_OPEN(fileNameAryDBG, "rb+", O_RDWR, 0775);

    // Create new instruction

    stream_instruction_t instruction;
    instruction.action = (stream_action_t)action;
    instruction.argument = argument;

    // Get the current instruction offset and goto offset

    unsigned int serverOffset = 0;
    unsigned int serverOffset2 = 0;
    unsigned int stateOffset = 0;
    unsigned int newServerOffset = 0;

    F_SEEK(debugFilePtr_input, INSTRUCTION_IN_OFFSET, SEEK_SET);
    F_READ((void*)&serverOffset, sizeof(unsigned int), 1, debugFilePtr_input, (1 * sizeof(unsigned int)));
    F_SEEK(debugFilePtr_input, INSTRUCTION_IN_VERIFY_OFFSET, SEEK_SET);
    F_READ((void*)&serverOffset2, sizeof(unsigned int), 1, debugFilePtr_input, (1 * sizeof(unsigned int)));

    // Don't add instruction if server offset will be caught up with state machine
    // offset with this new instruction.

    newServerOffset = serverOffset + sizeof(stream_instruction_t);

    if (newServerOffset >= FILE_PIPE_SIZE)
    {
        F_SEEK(debugFilePtr_input, INSTRUCTION_START_OFFSET, SEEK_SET);
        newServerOffset = INSTRUCTION_START_OFFSET;
    }

    if (newServerOffset == stateOffset)
    {
        EventLog("Error: Add Instruction. File Filled", DEBUG_5);
    }
    else
    {
        // Go to the proper offset
        F_SEEK(debugFilePtr_input, serverOffset, SEEK_SET);

        // Write the instruction
        F_WRITE((void*)&instruction, sizeof(stream_instruction_t), 1, debugFilePtr_input, (1 * sizeof(stream_instruction_t)));

        // Go to the server file offset value offset and update
        F_SEEK(debugFilePtr_input, INSTRUCTION_IN_OFFSET, SEEK_SET);
        F_WRITE((void*)&newServerOffset, sizeof(unsigned int), 1, debugFilePtr_input, (1 * sizeof(unsigned int)));

        // Also update the verifaction offset
        F_SEEK(debugFilePtr_input, INSTRUCTION_IN_VERIFY_OFFSET, SEEK_SET);
        F_WRITE((void*)&newServerOffset, sizeof(unsigned int), 1, debugFilePtr_input, (1 * sizeof(unsigned int)));
    }

    F_CLOSE(debugFilePtr_input);
    debugFilePtr_input = FD_NULL;

    // Update the input file inddex

    inputFileIndex += 1;
    if (inputFileIndex >= INPUT_FILE_COUNT)
    {
        inputFileIndex = 0;
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// PrintDebugMenu
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Prints debug menu
//
//--------------------------------------------------------------------------------------------------

void PrintDebugMeu(void)
{
    fprintf(stdout, "[1]  Add user to invitation list -- Quick\r\n");
    fprintf(stdout, "[2]  Add user to invitation list -- Instruct\r\n");
    fprintf(stdout, "[3]  Add user to queue -- Quick\r\n");
    fprintf(stdout, "[4]  Add user to queue -- Instruct\r\n");
    fprintf(stdout, "[5]  Assert Start Confirm -- Instruct\r\n");
    fprintf(stdout, "[6]  Assert Terminate Confirm -- Instruct\r\n");
    fprintf(stdout, "[7]  Assert Socket Confirm -- Instruct\r\n");
    fprintf(stdout, "[8]  Assert Invite Confirm -- Instruct\r\n");
    fprintf(stdout, "[a]  Add 1 up vote -- Instruct\r\n");
    fprintf(stdout, "[z]  Remove 1 up vote -- Instruct\r\n");
    fprintf(stdout, "[s]  Add 1 down vote -- Instruct\r\n");
    fprintf(stdout, "[x]  Remove 1 down vote -- Instruct\r\n");
    fprintf(stdout, "[d]  Add 1 null vote -- Instruct\r\n");
    fprintf(stdout, "[I]  CURL Test - Request Invite Fill\r\n");
    fprintf(stdout, "[J]  CURL Test - Request Pass Key\r\n");
    fprintf(stdout, "[K]  CURL Test - Request Socket Event\r\n");
    fprintf(stdout, "[L]  CURL Test - Request Stream Confirm\r\n");
    fprintf(stdout, "[v]  Alias Test 1 -- Instruct\r\n");
    fprintf(stdout, "[b]  Alias Test 2 -- Instruct\r\n");
    fprintf(stdout, "[n]  Alias Test 3 -- Instruct\r\n");
    fprintf(stdout, "[m]  Alias Test 4 -- Instruct\r\n");
    fprintf(stdout, "[p]  Instruction Speed Test\r\n");
    fprintf(stdout, "[,]  Assert All Confirm Flags\r\n");
    fprintf(stdout, "[R]  Re-Initialize State Machine\r\n");
    fprintf(stdout, "[.]  Print Stream Info\r\n");
    fprintf(stdout, "[q]  Quit\r\n");
}

//--------------------------------------------------------------------------------------------------
//Name:
// Debug_AddUserToInvitationList
//
//Arguments:
// method               - 0: Force Insert
//                        1: Insert Via File Instruction
//
//Returns:
// void
//
//About:
// Adds the next user id to the invitation list
//
//--------------------------------------------------------------------------------------------------

void Debug_AddUserToInvitationList(unsigned char method)
{
    static unsigned int userID = 999999;
    userID += 1;

    if (method == 0)
    {
        AddUserToInvitationList(userID);
    }
    else if (method == 1)
    {
        Debug_AddInstructionToFile(ACTION_ADD_TO_LIST, userID);
    }
    else
    {
        // Void
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// Debug_AddUserToQueue
//
//Arguments:
// method               - 0: Force Insert
//                        1: Insert Via File Instruction
//
//Returns:
// void
//
//About:
// Adds the next user id to the queue
//
//--------------------------------------------------------------------------------------------------

void Debug_AddUserToQueue(unsigned char method)
{
    static unsigned int userID = 999999;
    userID += 1;

    if (method == 0)
    {
        EnqueueUserToStreamingQueue(userID);
    }
    else if (method == 1)
    {
        Debug_AddInstructionToFile(ACTION_ADD_TO_QUEUE, userID);
    }
    else
    {
        // Void
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// Debug_AssembleStreamerAlias
//
//Arguments:
// alias                - Character pointer to alias
//
//Returns:
// void
//
//About:
// Tests the setting of streamer alias via multi-frame instruction
//
//--------------------------------------------------------------------------------------------------

void Debug_AssembleStreamerAlias(const char* alias)
{
    if (alias == NULL)
    {
        return;
    }

    // Get the number of frames to transmit. Remember to add 1 for null terminator,
    // because strlen doesn't count this.

    unsigned int frames = (unsigned int)((strlen(alias) + 1 + 3) / 4);

    stream_instruction_t instruction;
    instruction.mf_action = ACTION_ASSEMBLE_ALIAS;

    // Write the instructions to the file buffer... do it backwards for best testing

    for (int i = (frames - 1); i >= 0; i--)
    {
        memcpy((void*)instruction.characters, (void*)(alias + (4 * i)), 4);
        instruction.mf_offset = i;
        Debug_AddInstructionToFile(instruction.action, instruction.argument);
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// Debug_AssembleServerPassKey
//
//Arguments:
// passkey                - Character pointer to passkey
//
//Returns:
// void
//
//About:
// Tests the setting of streamer alias via multi-frame instruction
//
//--------------------------------------------------------------------------------------------------

void Debug_AssembleServerPassKey(const char* passkey)
{
    if (passkey == NULL)
    {
        return;
    }

    // Get the number of frames to transmit. Remember to add 1 for null terminator,
    // because strlen doesn't count this.

    unsigned int frames = (unsigned int)((strlen(passkey) + 1 + 3) / 4);

    stream_instruction_t instruction;
    instruction.mf_action = ACTION_ASSEMBLE_PASSKEY;

    // Write the instructions to the file buffer... do it backwards for best testing

    for (int i = (frames - 1); i >= 0; i--)
    {
        memcpy((void*)instruction.characters, (void*)(passkey + (4 * i)), 4);
        instruction.mf_offset = i;
        Debug_AddInstructionToFile(instruction.action, instruction.argument);
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// Debug_InstructionSpeedTest
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Tests the speed of adding instructions to the input file. This will come in handy when testing
// shared memory versus non-volatile file.
// Note: Non-volatile took an average of 6 seconds
//
//--------------------------------------------------------------------------------------------------

void Debug_InstructionSpeedTest(void)
{
    // Get the number of iterations to almost fill the buffer
    unsigned int iterations = (FILE_INSTRUCTION_COUNT_SIZE - 10);
    unsigned long long int startTime = TimeStampNowMS();

    for (unsigned int i = 0; i < iterations; i++)
    {
        Debug_AddInstructionToFile(ACTION_ADD_UP_VOTE, 0);
    }

    unsigned long long int endTime = TimeStampNowMS();
    fprintf(stdout, "Speed Test (ms): %llu\r\n", endTime - startTime);
}

//--------------------------------------------------------------------------------------------------
//Name:
// Debugger
//
//Arguments:
// void
//
//Returns:
// void
//
//About:
// Debug handler. Handles the creation of the debug menu, task execution, etc.. Should be called 
// from a separate thread.
//
//--------------------------------------------------------------------------------------------------

void Debugger()
{
    int input = 0;
    std::string res = "";

    while (streamInfo.flags.debugThread)
    {
        // Get character input. 
        input = GET_CHAR();

        switch (input)
        {
        case 'D':
            PrintDebug = (PrintDebug) ? 0 : 1;
            fprintf(stdout, "Toggling Debug Output\r\n");
            break;

        case '1':
            Debug_AddUserToInvitationList(0);
            break;

        case '2':
            Debug_AddUserToInvitationList(1);
            break;

        case '3':
            Debug_AddUserToQueue(0);
            break;

        case '4':
            Debug_AddUserToQueue(1);
            break;

        case '5':
            Debug_AddInstructionToFile(ACTION_START_CONFIRM, 0);
            break;

        case '6':
            Debug_AddInstructionToFile(ACTION_TERMINATE_CONFIRM, 0);
            break;

        case '7':
            Debug_AddInstructionToFile(ACTION_SOCKET_CONFIRM, 0);
            break;

        case '8':
            Debug_AddInstructionToFile(ACTION_INVITE_CONFIRM, 0);
            break;

        case 'a':
            Debug_AddInstructionToFile(ACTION_ADD_UP_VOTE, 0);
            break;

        case 'z':
            Debug_AddInstructionToFile(ACTION_REMOVE_UP_VOTE, 0);
            break;

        case 's':
            Debug_AddInstructionToFile(ACTION_ADD_DOWN_VOTE, 0);
            break;

        case 'x':
            Debug_AddInstructionToFile(ACTION_REMOVE_DOWN_VOTE, 0);
            break;

        case 'd':
            Debug_AddInstructionToFile(ACTION_ADD_NULL_VOTE, 0);
            break;

        case 'I':
            sprintf(httpRequestAry[0], LINK_REQUEST_INVITE_FILL_POST, ServerPasskey, 5);
            sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_INVITE_FILL);
            #if SERVER_COMM
            res = httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
            #endif
            //fprintf(stdout, "%s\r\n", res.c_str());

            sprintf(messageAry, "CURL - Invite Fill : %s", ((res == "") ? "FAIL" : "PASS"));
            EventLog(messageAry, DEBUG_1);
            break;

        case 'J':
            sprintf(httpRequestAry[0], LINK_REQUEST_SERVER_KEY_POST);
            sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_SERVER_KEY);
            #if SERVER_COMM
            res = httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
            #endif
            //fprintf(stdout, "%s\r\n", res.c_str());

            sprintf(messageAry, "CURL - Request Server Key : %s", ((res == "") ? "FAIL" : "PASS"));
            EventLog(messageAry, DEBUG_6);
            break;

        case 'K':
            sprintf(httpRequestAry[0], LINK_REQUEST_SOCKET_EVENT_POST, ServerPasskey);
            sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_SOCKET_EVENT);
            #if SERVER_COMM
            res = httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
            #endif
            //fprintf(stdout, "%s\r\n", res.c_str());

            sprintf(messageAry, "CURL - Socket Event : %s", ((res == "") ? "FAIL" : "PASS"));
            EventLog(messageAry, DEBUG_6);
            break;

        case 'L':
            sprintf(httpRequestAry[0], LINK_REQUEST_STREAM_CONFIRM_POST, ServerPasskey, 1, 0);
            sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_STREAM_CONFIRM);
            #if SERVER_COMM
            res = httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
            #endif
            //fprintf(stdout, "%s\r\n", res.c_str());

            sprintf(messageAry, "CURL - Stream Confirm : %s", ((res == "") ? "FAIL" : "PASS"));
            EventLog(messageAry, DEBUG_6);
            break;

        case 'v':
            Debug_AssembleStreamerAlias("Jared Fowler");
            Debug_AssembleServerPassKey("Jared Fowler");
            break;

        case 'b':
            Debug_AssembleStreamerAlias("Mike Vreeken");
            Debug_AssembleServerPassKey("Mike Vreeken");
            break;

        case 'n':
            Debug_AssembleStreamerAlias("This is a very long sentence... I mean, really long.");
            Debug_AssembleServerPassKey("This is a very long sentence... I mean, really long.");
            break;

        case 'm':
            Debug_AssembleStreamerAlias("Midnight the Meow Meow");
            Debug_AssembleServerPassKey("Midnight the Meow Meow");
            break;

        case 'p':
            Debug_InstructionSpeedTest();
            break;

        case ',':
            streamInfo.flags.startConfirm = 1;
            streamInfo.flags.terminationConfirm = 1;
            streamInfo.flags.socketConfirm = 1;
            streamInfo.flags.inviteConfirm = 1;
            break;

        case '.':
            PrintStreamInfo(stdout);
            break;

        case 'R':
            streamInfo.flags.resetSateMachine = 1;
            break;

        case 'q':
            streamInfo.flags.debugThread = 0;
            break;

        case 'h':
            PrintDebugMeu();
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------
//Name:
// StreamStateMachine
//
//Arguments:
// void
//
//Returns:
// PASS (0) / FAIL (!0)
//
//About:
// Infinite loop state machine. Handles selection of users to invite to stream, handles instructions
// from the main server, and tickles the servers socket stream.
//
//--------------------------------------------------------------------------------------------------

int StreamStateMachine()
{
    int error = 0;
    std::thread debugThread;
    std::string res;
    
    unsigned int stateMachineIterationCount = 0;
    stream_ticket_t streamTicket;

    unsigned int watchdog_streaming = 0;
    unsigned int watchdog_socketConfirm = 0;
    unsigned int watchdog_inviteConfirm = 0;

    // Re-route text output, get file handles

    freopen(LOG_FILE, "w", stderr);

    // Initialize Debug Thread
    streamInfo.flags.debugThread = 1;
    debugThread = std::thread(Debugger);

    // Initialize the state machine
    InitializeStreamStateMachine();
    streamInfo.flags.resetSateMachine = 0;

    // Start the state machine

    while (true)
    {
        stateMachineIterationCount += 1;

        // Reset the state machine?
        // Needs to be done here instead of debug thread due to multi-threading asynchronousness

        if (streamInfo.flags.resetSateMachine)
        {
            EventLog("Re-Initializing State Machine", DEBUG_6);
            InitializeStreamStateMachine();
            streamInfo.flags.resetSateMachine = 0;
        }

        // Check which jobs need to be done

        if ((stateMachineIterationCount % streamParam.socketEventTimeout) == 0)
        {
            // Before attempting to request another socket event, first wait for
            // the server to handshake to confirm that the previous operation
            // was handled and copleted, or wait for confirmation timeout.

            if ((streamInfo.flags.socketConfirm) ||
                (watchdog_socketConfirm > streamParam.socketConfirmTimeout))
            {
                UpdateSocketOutput();
                TickleServerSocketEvent();
                streamInfo.flags.socketConfirm = 0;
                watchdog_socketConfirm = 0;
            }
            else
            {
                watchdog_socketConfirm += 1;
            }
        }

        #if PARAM_UPDATE
        if ((stateMachineIterationCount % streamParam.paramUpdateTimeout) == 0)
        {
            UpdateVolatileParameters(PARAM_FILE_SYNC);
        }
        #endif

        if ((stateMachineIterationCount % streamParam.inviteEventTimeout) == 0)
        {
            // Before attempting to request more people to be put onto the invitation
            // list, first wait for server handshake to confirm that the previous
            // invitation task was handled and completed.

            if ((streamInfo.flags.inviteConfirm) ||
                (watchdog_inviteConfirm > streamParam.inviteConfirmTimeout))
            {
                TickleServerInvitationEvent();
                streamInfo.flags.inviteConfirm = 0;
                watchdog_inviteConfirm = 0;
            }
            else
            {
                watchdog_inviteConfirm += 1;
            }
        }

        // Remove timed-out users from the invitation list

        if ((stateMachineIterationCount % streamParam.inviteRespEventTimeout) == 0)
        {
            RemoveTimedOutUsersFromInvitationList();
        }

        // Handle incoming instructions from Server

        if ((stateMachineIterationCount % streamParam.instructEventTimeout) == 0)
        {
            ProcessStreamInstructions(streamParam.maxOpsPerIteration);
        }

        // Streaming State

        switch (streamInfo.state)
        {
            // IDLE
            // The is no-one currently streaming and all streaming termination events have been
            // completed
            case STREAM_STATE_IDLE:

                // If the stream queue isn't empty, dequeue the first user and set them to the streaming state
                if (!streamInfo.queue.empty())
                {
                    memcpy(&streamTicket, &streamInfo.queue.front(), sizeof(stream_ticket_t));
                    streamInfo.queue.pop_front();

                    watchdog_streaming = 0;
                    streamInfo.flags.startConfirm = 0;
                    streamInfo.streamer.id = streamTicket.userID;

                    // Update the state machine state
                    streamInfo.state = STREAM_STATE_WAIT_FOR_CONFRIMATION;

                    // This flag reset should be placed here in the event that the streamer decides to back
                    // out during the warmup or any other time before their stream begins.
                    streamInfo.flags.terminateStream = 0;

                    // Send request to server to confirm the start of the stream. Server will need to send
                    // additional META DATA to the user.

                    sprintf(httpRequestAry[0], LINK_REQUEST_STREAM_CONFIRM_POST, ServerPasskey, streamInfo.streamer.id, SM_START_CONFIRM);
                    sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_STREAM_CONFIRM);
                    #if SERVER_COMM
                    res = httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
                    #endif
                    //fprintf(stdout, "%s\r\n", res.c_str());

                    sprintf(messageAry, "CURL - Start Confirm : %s", ((res == "") ? "FAIL" : "PASS"));
                    EventLog(messageAry, DEBUG_1);
                }

                break;

            // WAIT_FOR_STREAM_CONFIRMATION
            // Wait for the Server to respond to the stream request made by the state machine. This block is
            // released when either the server sends confirmation and associated user information, or when
            // a timeout has been reached, in which case the state machine will ultimately revert back to the
            // idle state.
            case STREAM_STATE_WAIT_FOR_CONFRIMATION:

                watchdog_streaming += 1;

                if (streamInfo.flags.startConfirm)
                {
                    streamInfo.state = STREAM_STATE_WARMUP;
                    ResetStreamTimer(STREAM_TIMER_WARMUP);
                }
                else if (watchdog_streaming > streamParam.serverConfirmTimeout)
                {
                    EventLog("ERROR. Stream Confirmation Timeout.", DEBUG_1);
                    streamInfo.state = STREAM_STATE_IDLE;
                }

                break;

            // WARMUP
            // The warmup phase gives time for the new streamer to request the stream key and also have a 
            // countdown before they start streaming.
            case STREAM_STATE_WARMUP:

               if (StreamTimeLeft() == 0)
               {
                  streamInfo.state = STREAM_STATE_INITIALIZING;
               }

               break;

            // INITIALIZING
            // A user has been selected to stream. Do all preparation work
            case STREAM_STATE_INITIALIZING:

                ResetStreamTimer(STREAM_TIMER_STREAM);
                ResetStreamVotes();

                streamInfo.state = STREAM_STATE_STREAMING;

                // break; - No break. Just fall through

            // STREAMING
            // A user is currently streaming. Poll stream time remaining and terminate stream
            // when time runs out.
            case STREAM_STATE_STREAMING:

                if ((StreamTimeLeft() == 0) || (streamInfo.flags.terminateStream))
                {
                    // Update the state
                    streamInfo.state = STREAM_STATE_TERMINATING;

                    watchdog_streaming = 0;
                    streamInfo.flags.terminationConfirm = 0;

                    // Tell the Server to start the cleanup process.

                    sprintf(httpRequestAry[0], LINK_REQUEST_STREAM_CONFIRM_POST, ServerPasskey, streamInfo.streamer.id, SM_TERMINATE_CONFIRM);
                    sprintf(httpRequestAry[1], "%s%s%s%s", LINK_PROTOCOL, LINK_ROOT, LINK_PATH, LINK_REQUEST_STREAM_CONFIRM);
                    #if SERVER_COMM
                    res = httpDownloader.download(httpRequestAry[1], httpRequestAry[0], HttpDownloader::POST);
                    #endif
                    //fprintf(stdout, "%s\r\n", res.c_str());

                    sprintf(messageAry, "CURL - Terminate Confirm : %s", ((res == "") ? "FAIL" : "PASS"));
                    EventLog(messageAry, DEBUG_0);
                }

                break;

            // TERMINATING
            // Cleanup is handled on the server side. Wait until the server sends completed confirmation
            // or until timeout is reached
            case STREAM_STATE_TERMINATING:

                watchdog_streaming += 1;

                if (streamInfo.flags.terminationConfirm)
                {
                    streamInfo.state = STREAM_STATE_COOLDOWN;
                    ResetStreamTimer(STREAM_TIMER_COOLDOWN);
                    watchdog_streaming = 0;

                    // Clear the streamer ID and alias
                    streamInfo.streamer.id = 0;
                    strncpy(streamInfo.streamer.alias, "LimeLight", sizeof(streamInfo.streamer.alias));
                }
                else if (watchdog_streaming > streamParam.serverConfirmTimeout)
                {
                    EventLog("ERROR. Termination Confirmation Timeout.", DEBUG_0);
                    streamInfo.state = STREAM_STATE_COOLDOWN;
                    ResetStreamTimer(STREAM_TIMER_COOLDOWN);

                    // Clear the streamer ID and alias
                    streamInfo.streamer.id = 0;
                    strncpy(streamInfo.streamer.alias, "LimeLight", sizeof(streamInfo.streamer.alias));
                }

                break;

            // COOLDOWN
            // The cooldown phase gives time for things to settle before starting another warmup. We 
            // could use this time to display an advertisment. 
            case STREAM_STATE_COOLDOWN:

               if (StreamTimeLeft() == 0)
               {
                  streamInfo.state = STREAM_STATE_IDLE;
               }

               break;

            // ERROR
            // We should never get here... this is for debugging purposes ONLY
            default:
            case STREAM_STATE_ERROR:
                EventLog("STATE MACHINE ERROR.", DEBUG_5);
                break;
        }

        // Has termination been called
        if (!streamInfo.flags.debugThread)
        {
            break;
        }

        // State Machine Timeout
        SLEEP_SECONDS(streamParam.stateMachineTimeout);
    }

    // Wait for debug thread to end
    streamInfo.flags.debugThread = 0;
    if (debugThread.joinable())
    {
        debugThread.join();
    }

    return 0;
}
