/*Simple tools to analyze ciphertext and help decode it.

Written By: Jared Fowler
February 18, 2015

References:

//Thread API
https://msdn.microsoft.com/en-us/library/windows/desktop/ms684847%28v=vs.85%29.aspx#thread_functions

//Console API
https://msdn.microsoft.com/en-us/library/windows/desktop/ms682073(v=vs.85).aspx

//Permutation Algorithm
http://www.nayuki.io/page/next-lexicographical-permutation-algorithm

*/

#ifndef CRYPTOGRAPHY_JWF_H
#define CRYPTOGRAPHY_JWF_H

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include "WordBank.h"


//Defines
#define FILENAME_LENGTH         40
#define LETTERS_IN_ALPHABET     26
#define CIPHER_SIZE             sizeof(CIPHER)
#define READ_BUFFER_SIZE        CIPHER_SIZE * 512
#define WRITE_BUFFER_SIZE       READ_BUFFER_SIZE + 512
#define MAX_WORD_SIZE           8
#define TOP_SHIFT_LIST_SIZE     3
#define THREAD_COUNT            TOP_SHIFT_LIST_SIZE + 1
#define THREAD_SLEEP_TIME       CIPHER_SIZE * MAX_KEY_SIZE * 20
#define THREAD_SLEEP_TIME_FIXED 5000
#define THREAD_SLEEP_TIME_STAT  2000
#define SURVIVAL_SCORE_MIN      CIPHER_SIZE * 3
#define SURVIVAL_COUNT          3

#define TRUE                    1
#define FALSE                   0

#define FILE_OPEN_ERROR         0x01
#define FILE_NAME_ERROR         0x02

//Typedefs
typedef unsigned char           BYTE;
typedef unsigned int            UINT;
typedef unsigned long int       ULONG;
typedef unsigned long long int  ULLONG;

//Structs
typedef struct CipherAnalysis{

    UINT totalCount;
    ULONG letterCount[LETTERS_IN_ALPHABET];
    float ratio[LETTERS_IN_ALPHABET];

};

typedef struct ShiftGuess{

    BYTE shiftOffset;
    float compareDiff;

};

typedef struct Survivor{

    char cipher[CIPHER_SIZE];
    UINT score;

};

typedef struct Survivors{

    struct Survivor survivor[SURVIVAL_COUNT];
    UINT scoreToBeat;

};

//Constants
const char DEFAULT_OUT[]  = "outputFile.txt";
const char WORD_BANK[]    = "dictOutput.txt";
const char SURVIVOR_OUT[] = "thread_xx_out.txt";

//Global Vars -- Bad practice, makes life easier...
char simpleShifts[LETTERS_IN_ALPHABET][CIPHER_SIZE] = { 0 };

struct ShiftGuess topShiftGuesses[TOP_SHIFT_LIST_SIZE];

struct Survivors theSurvivors[THREAD_COUNT];

char columnMajor[THREAD_COUNT][CIPHER_SIZE][CIPHER_SIZE];
char bufferOut[WRITE_BUFFER_SIZE] = { 0 };
char bufferIn[READ_BUFFER_SIZE]   = { 0 };

struct CipherAnalysis cA, cAref;

HANDLE thread[THREAD_COUNT];
DWORD threadId[THREAD_COUNT];

char tInput[THREAD_COUNT][5] = { 0 };

HANDLE hConsole_out;
BOOL weAreDone = FALSE;

ULLONG jobsDone[THREAD_COUNT];
ULLONG jobsToDo = 0;

//Prototypes

/*Reads selected file and outputs to file information concerning file.
Information includes total char count, and ratios of each character used.*/
BYTE fileCharCountAndRatioAnalysis(void);

/*Analysis hard-coded cipher text and stores away information such as total
char count, and ratios of each character used. This will also compare the
results with a dictionary input file and attempt to determine the best 
possible simple shift answer.*/
BYTE charCountAndRatioAnalysis(void);

/*Calls 3 functions. charCountAndRatioAnalysis, simpleShift, and bestSHiftGuess. This
is essentially the prep work before getting into the major permutations.*/
BYTE letterAnalysisAndSimpleShift(void);

/*Outputs to a file and stores away a list of new cipher-texts, each having
been simply shifted one to the right from its predecessor.*/
BYTE simpleShift(void);

/*Determines which one of the 26 different results from simple shift best match
the frequencies given by a standard dictionary input. Top 4 are kept in a list.*/
BYTE bestShiftGuess(void);

/*Main manager function which sets up threads which are called to statusBar and threadPermutation*/
BYTE permutateAndFilterate(BYTE choice, UINT survivalScore);

/*Most complex function here. Permutates through all the given possibilities
of a single cipher and calls assignCipherScore to determine if it should be kept or not.*/
DWORD WINAPI threadPermutation(LPVOID lpParam);

/*Prints out a percent complete bar as to indicate the progress.*/
DWORD WINAPI statusBar(LPVOID lpParam);

/*Used especially for agruments being passed in which are bound to a value of 25 or less.*/
BOOL converter_offset_string_2CharsOnly(BYTE* offset, char* str, BYTE conversionDirection);

/*Assigns a score to a given cipher by comparing it to a dictionary. The dictionary is 
in reverse order as to compare the largest words first.*/
UINT assignCipherScore(char* theCipher, UINT sizeofCipher);

/*Returns a pointer to a dictionary with a specific letter.*/
char** const** getDictionaryPtr(char letter);

/*Sets the values in an array to either 1's and 0's or from 0 to size - 1. This
is in preparation for nextPermutation which reuqires an array to be passed in
which starts at the lowest permutation first.*/
BOOL setUpPermutationAry(UINT* aryIn, UINT size, BOOL binary, UINT zeroCount);

/*Calculates Factorials*/
UINT factorial(UINT n);

/*Calculates a choose b*/
UINT choose(UINT a, UINT b);

/*Calculates the number of jobs to do based off the length of the cipher, 
key length, and the number of times we are going to iterate through this process.*/
ULLONG calculateJobsToDo(UINT cipherLength, UINT maxKeyLength, UINT iterations);

/*Prints out final results gained from permutateAndFilterate*/
void printOutFinalResults(void);

/*Reads an input file composed of words separated by a space or \n and ouputs 
it into another file in a format that can easily be copied and pasted into 
WordBank.h. Will generate 26 different files, each with words only
pertaining to that files letter in the dictionary.*/
BYTE createWordBank(void);

/*I did not write this function, though, I did mod it slightly to work with my code.
A reference to where I found this function can be seen at the top of this file.*/
BOOLEAN nextPermutation(int array[], int arrayLength);


//Functions
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BYTE fileCharCountAndRatioAnalysis(void){

    //Local Variables
    BYTE returnVal   = 0;
    UINT i, charsRead;

    char fileNameIn[FILENAME_LENGTH] = { 0 };
    char fileNameOut[FILENAME_LENGTH] = { 0 };
    FILE *fptrIn = NULL;
    FILE *fptrOut = NULL;

    //Prompt for input file name
    fprintf(stdout, "Input File: ");

    //Get the user input
    if (fgets(fileNameIn, FILENAME_LENGTH, stdin) == NULL)
        return (returnVal | FILE_NAME_ERROR);

    //Eliminate the '\n'
    for (i = 0; i < FILENAME_LENGTH; i++){

        if (fileNameIn[i] == '\n'){
            fileNameIn[i] = '\0';
            break;
        }

    }

    //Prompt for output file name
    fprintf(stdout, "Output File: ");

    //Get the user input
    if (fgets(fileNameOut, FILENAME_LENGTH, stdin) == NULL)
        return (returnVal | FILE_NAME_ERROR);

    //Eliminate the '\n'
    for (i = 0; i < FILENAME_LENGTH; i++){

        if (fileNameOut[i] == '\n'){
            fileNameOut[i] = '\0';
            break;
        }

    }

    //Attempt to open the input file and output file
    if ((fptrIn = fopen(fileNameIn, "r")) == NULL){
        returnVal = (returnVal | FILE_OPEN_ERROR);
        return returnVal;
    }
    if ((fptrOut = fopen(fileNameOut, "a")) == NULL){
        returnVal = (returnVal | FILE_OPEN_ERROR);
        return returnVal;
    }

    //Clear the structure
    cAref.totalCount = 0;
    for (i = 0; i < LETTERS_IN_ALPHABET; i++){
        cAref.letterCount[i] = 0;
        cAref.ratio[i] = 0;
    }

    //Begin analysis
    while (TRUE){

        //Read next group of chars
        if ((charsRead = fread(bufferIn, 1, READ_BUFFER_SIZE, fptrIn)) == 0){
            //We didn't read anything. EOF
            break;
        }

        //Count how many of each letter there is
        for (i = 0; i < charsRead; i++){

            if (bufferIn[i] >= 'a' && bufferIn[i] <= 'z'){

                cAref.letterCount[bufferIn[i] - 'a'] += 1;
            }
            else if (bufferIn[i] >= 'A' && bufferIn[i] <= 'Z'){

                cAref.letterCount[bufferIn[i] - 'A'] += 1;
            }
            else{ //Non-Alphabetical Char
                continue;
            }

            //Increment the letter count
            cAref.totalCount++;
        }

        //Did we reach EOF?
        if (charsRead != READ_BUFFER_SIZE){
            break;
        }

    }

    //Output analysis to file
    fprintf(fptrOut, "File Name: %s\n", fileNameOut);
    fprintf(fptrOut, "Total Letters Read: %lu\n\n", cAref.totalCount);

    for (i = 0; i < LETTERS_IN_ALPHABET; i++){
        cAref.ratio[i] = ((float)cAref.letterCount[i] / cAref.totalCount) * 100;
        fprintf(fptrOut, "%c: %9lu   %3.5f    \n", ('A' + i), cAref.letterCount[i], cAref.ratio[i]);
    }

    fprintf(fptrOut, "\n\n\n\n");

    //Close files
    fclose(fptrIn);
    fclose(fptrOut);

    return returnVal;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BYTE letterAnalysisAndSimpleShift(void){

    //Local Variables
    BYTE returnVal = 0;

    //Simple letter analysis
    returnVal = charCountAndRatioAnalysis();
    returnVal = (returnVal | simpleShift());
    returnVal = (returnVal | bestShiftGuess());

    return returnVal;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BYTE charCountAndRatioAnalysis(void){

    //Local Variables
    BYTE returnVal = 0;
    UINT i;

    char fileNameOut[FILENAME_LENGTH] = { 0 };
    FILE *fptrOut = NULL;

    //Prepare the structure
    cA.totalCount = 0;
    for (i = 0; i < LETTERS_IN_ALPHABET; i++){
        cA.letterCount[i] = 0;
        cA.ratio[i] = 0;
    }

    //Open file for output
    if ((fptrOut = fopen(DEFAULT_OUT, "a")) == NULL){
        returnVal = (returnVal | FILE_OPEN_ERROR);
        return returnVal;
    }

    //Count how many of each letter there is
    for (i = 0; i < CIPHER_SIZE; i++){

        if (CIPHER[i] >= 'A' && CIPHER[i] <= 'Z'){

            cA.letterCount[CIPHER[i] - 'A'] += 1;
        }
        else{ //Non-Alphabetical Upper-Case Char
            continue;
        }

        //Increment the letter count
        cA.totalCount++;
    }

    //Output analysis to file
    fprintf(fptrOut, "File Letter Analysis:\n");
    fprintf(fptrOut, "Total Letters Read: %lu\n\n", cA.totalCount);

    for (i = 0; i < LETTERS_IN_ALPHABET; i++){
        cA.ratio[i] = ((float)cA.letterCount[i] / cA.totalCount) * 100;
        fprintf(fptrOut, "%c: %9lu   %3.5f    \n", ('A' + i), cA.letterCount[i], cA.ratio[i]);
    }

    fprintf(fptrOut, "\n\n\n\n");

    //Close output file
    fclose(fptrOut);

    return returnVal;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BYTE simpleShift(void){

    //Local Variables
    BYTE returnVal = 0;
    UINT i, j;
    char value;

    char fileNameOut[FILENAME_LENGTH] = { 0 };
    FILE *fptrOut = NULL;

    //Open file for output
    if ((fptrOut = fopen(DEFAULT_OUT, "a")) == NULL){
        returnVal = (returnVal | FILE_OPEN_ERROR);
        return returnVal;
    }

    //Title
    fprintf(fptrOut, "Simple Shift Permutation: \n\n");
    
    //Fill in the permutations
    for (i = 0; i < LETTERS_IN_ALPHABET; i++){

        for (j = 0; j < sizeof(CIPHER) - 1; j++){

            //Get the char value
            value = ((((CIPHER[j] - 'A') + i) % LETTERS_IN_ALPHABET) + 'A');

            //Backup in structure
            simpleShifts[i][j] = value;

        }

        //Write to file
        fwrite(simpleShifts[i], sizeof(char), CIPHER_SIZE - 1, fptrOut);
        fwrite("\n", sizeof(char), 1, fptrOut);

    }

    fprintf(fptrOut, "\n\n\n");

    //Close the file
    fclose(fptrOut);

    return returnVal;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BYTE bestShiftGuess(void){

    //Local Variables
    BYTE returnVal = 0;
    UINT i, j;
    float difference;

    char fileNameOut[FILENAME_LENGTH] = { 0 };
    FILE *fptrOut = NULL;

    /*Compare the ratios of letters with that of a reference. (My dictionary letter ratio reference.)
    Take the difference between the two for each letter in each permutation. The n permutations which
    closest match will be kept.*/

    //Prepare the variable structure
    for (i = 0; i < TOP_SHIFT_LIST_SIZE; i++){
        topShiftGuesses[i].compareDiff = 999.9f;
        topShiftGuesses[i].shiftOffset = 100;
    }

    for (i = 0; i < LETTERS_IN_ALPHABET; i++){

        difference = 0;

        for (j = 0; j < LETTERS_IN_ALPHABET; j++){
            //For each letter, calculate the difference and take absolute value
            difference += fabsf(cA.ratio[(j + i) % LETTERS_IN_ALPHABET] - LETTER_FREQ_REF[j]);
        }

        //Now, determine if this is in the top n
        for (j = 0; j < TOP_SHIFT_LIST_SIZE; j++){

            if (difference < topShiftGuesses[j].compareDiff){
                //Update the top list
                topShiftGuesses[j].compareDiff = difference;
                topShiftGuesses[j].shiftOffset = i;
                break;
            }
        }
    }

    //Open file for output
    if ((fptrOut = fopen(DEFAULT_OUT, "a")) == NULL){
        returnVal = (returnVal | FILE_OPEN_ERROR);
        return returnVal;
    }

    //Print out results:
    fprintf(fptrOut, "My top guesses for simple shift offset: \n\n");

    for (i = 0; i < TOP_SHIFT_LIST_SIZE; i++){
        fprintf(fptrOut, "%2u. %u\n", i + 1, topShiftGuesses[i].shiftOffset);
    }

    fprintf(fptrOut, "\n\n\n");

    //Close the file
    fclose(fptrOut);

    return returnVal;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BYTE permutateAndFilterate(BYTE choice, UINT survivalScore){

    //Local Variables
    BYTE returnVal = 0;
    BYTE i, j; 
    DWORD result;
    BOOL bRet;
    
    //Valid Choice?
    if ((((char)choice != 'A') && ((char)choice != 'B')) && ((choice < 0) || (choice > 25)))
        return (returnVal | 0x03);

    //Reset jobsDone
    for (i = 0; i < THREAD_COUNT; i++){
        jobsDone[i] = 0;
    }

    weAreDone = FALSE;

    //Start Progress Thread
    thread[THREAD_COUNT] = CreateThread(
        NULL,                       // default security attributes
        0,                          // use default stack size  
        statusBar,                  // thread function name
        NULL,                       // argument to thread function
        0,                          // use default creation flags 
        &threadId[THREAD_COUNT]);   // returns the thread identifier

    //Iterations will depend on user choice
    if ((char)choice == 'A'){ //All permutations

        //Calculate the jobSize
        jobsToDo = calculateJobsToDo(CIPHER_SIZE - 1, MAX_KEY_SIZE, LETTERS_IN_ALPHABET);

        //Create threads and give them their tasks
        j = 0;
        while (TRUE){

            for (i = 0; i < THREAD_COUNT - 1; i++){

                bRet = GetExitCodeThread(thread[i], &result);

                //If so, assign it the next task
                if (STILL_ACTIVE != result) {

                    //Get the argument list
                    converter_offset_string_2CharsOnly(&j, tInput[i], 0);
                    converter_offset_string_2CharsOnly(&i, (tInput[i] + 2), 0);

                    //Assign thread
                    thread[i] = CreateThread(NULL, 0, threadPermutation, tInput[i], 0, &threadId[i]);

                    //Update what permutation we are on
                    j++;
                        
                    //Are we done?
                    if (j >= LETTERS_IN_ALPHABET)
                        goto WAIT_FOR_THREADS;
                }
            }
            
            //Give some down time
            Sleep(THREAD_SLEEP_TIME);
        }
    }
        
    else if ((char)choice == 'B'){ //Best guess permutations

        //Calculate the jobSize
        jobsToDo = calculateJobsToDo(CIPHER_SIZE - 1, MAX_KEY_SIZE, TOP_SHIFT_LIST_SIZE);

        //Create threads and give them their tasks
        j = 0;
        while (TRUE){

            for (i = 0; i < THREAD_COUNT - 1; i++){

                //Is this thread done with its work?
                bRet = GetExitCodeThread(thread[i], &result);

                //If so, assign it the next task
                if (STILL_ACTIVE != result) {

                    //Get the argument list
                    converter_offset_string_2CharsOnly(&topShiftGuesses[j].shiftOffset, tInput[i], 0);
                    converter_offset_string_2CharsOnly(&i, (tInput[i] + 2), 0);

                    //Assign thread
                    thread[i] = CreateThread(NULL, 0, threadPermutation, tInput[i], 0, &threadId[i]);

                    //Update what permutation we are on
                    j++;

                    //Are we done?
                    if (j >= TOP_SHIFT_LIST_SIZE)
                        goto WAIT_FOR_THREADS;
                }
            }

            //Give some down time
            Sleep(THREAD_SLEEP_TIME);
        }
    }

    //Wait for all the threads to finish what they are doing
WAIT_FOR_THREADS:

    while (TRUE){

        j = 0;

        for (i = 0; i < THREAD_COUNT - 1; i++){

            //Is this thread done with its work?
            bRet = GetExitCodeThread(thread[i], &result);

            if (result == STILL_ACTIVE)
                j = 1;
        }

        //Is everything finished?
        if (j == 0)
            break;

        //Sleep a little
        Sleep(THREAD_SLEEP_TIME_FIXED);
    }

    //Set a flag to indicate that we are done (signals status thread)
    weAreDone = TRUE;

    //Wait for the status thread to finish
    while (TRUE){
        bRet = GetExitCodeThread(thread[THREAD_COUNT], &result);
        if (result != STILL_ACTIVE)
            break;
        Sleep(THREAD_SLEEP_TIME_FIXED);
    }



    //Wait for status thread to finish


    return returnVal;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
DWORD WINAPI threadPermutation(LPVOID lpParam){

    //Local Varaibles
    DWORD returnVal = 0;
    char fileNameOut[sizeof(SURVIVOR_OUT)] = { 0 };
    UINT ary[MAX_KEY_SIZE];
    UINT ary2[MAX_KEY_SIZE];
    char theCipher[CIPHER_SIZE] = { 0 };
    BYTE offset;
    BYTE tNumber;
    UINT i, j, k;
    UINT r, c, e;
    UINT score;
    FILE* fOut;

    //Get offset
    converter_offset_string_2CharsOnly(&offset, (char*)lpParam, 1);

    //Get this thread id
    converter_offset_string_2CharsOnly(&tNumber, ((char*)lpParam) + 2, 1);

    //Clear survivor structure
    theSurvivors[tNumber].scoreToBeat = 0;
    for (i = 0; i < SURVIVAL_COUNT; i++)
        theSurvivors[tNumber].survivor[i].score = 0;

    //Attempt to open an output file
    strcpy(fileNameOut, SURVIVOR_OUT);
    fileNameOut[7] = ((char*)lpParam)[2];
    fileNameOut[8] = ((char*)lpParam)[3];

    if ((fOut = fopen(fileNameOut, "w")) == NULL){
        returnVal = (returnVal | FILE_OPEN_ERROR);
        return returnVal;
    }

    /*Now begins the hardest part of the program. For each key length, and for each column combination,
    call the assignCipherScore function. If result is good enough, write it to a file and if it's one
    of the best, keep it in structure. Note, a hard part to this is realizing that if a row doesn't fill
    we do not know which columns are the shorter ones!!*/
    for (c = 1; c <= MAX_KEY_SIZE; c++){  //KeySize == Columns

        //Calculate the number of rows and empty slots
        r = (CIPHER_SIZE - 1) / c;
        e = c - ((CIPHER_SIZE - 1) % c);
        if (e == c) e = 0;
        if (e != 0) r += 1;

        //Prepare array to hold permutations for column length
        setUpPermutationAry(ary, c, TRUE, e);

        //For each column length permutation
        do{

            //Clear the array
            for (k = 0; k < CIPHER_SIZE; k++){
                for (j = 0; j < CIPHER_SIZE; j++){
                    columnMajor[tNumber][k][j] = '\0';
                }
            }

            //Fill the columns
            j = 0;
            for (i = 0; i < c; i++){

                if (ary[i] == 1) k = r;
                else k = (r - 1);
           
                memcpy(columnMajor[tNumber][i], (simpleShifts[offset] + j), k);

                j += k;

            }

            //Prepare array to hold permutations for column order
            setUpPermutationAry(ary2, c, FALSE, 0);

            do{
                //Update the job done count
                jobsDone[tNumber] += 1;

                //Can we exclude this permutation? - All shortest columns not all to right portion
                if (e != 0){
                    for (j = 0; j < (c - e); j++){
                        if (ary[ary2[j]] == 0)
                            goto NEXT_THREADP_ITERATION;
                    }
                }
                
                //Fill the cipher
                k = 0;
                for (j = 0; j < r; j++){
                    for (i = 0; i < c; i++){

                        theCipher[k] = columnMajor[tNumber][ary2[i]][j];

                        if ((theCipher[k] != '\0') && (theCipher[k] != '\n')) //Increase location
                            k++;

                    }
                }
            
                //Assign a score to this cipher
                score = assignCipherScore(theCipher, CIPHER_SIZE - 1);

                //Does it make the cut?
                if (score >= SURVIVAL_SCORE_MIN){

                    //Add to output file
                    fprintf(fOut, "%s", theCipher);
                    fprintf(fOut, "\n");

                    //Does it make the top survivors?
                    if (score >= theSurvivors[tNumber].scoreToBeat){

                        k = -1;
                        j = 0;
                        for (i = 0; i < SURVIVAL_COUNT; i++){

                            if (theSurvivors[tNumber].survivor[i].score < k){

                                if (k == theSurvivors[tNumber].scoreToBeat)
                                    j = i;
                                else
                                    k = theSurvivors[tNumber].survivor[i].score;
                                
                                }
                            }

                        //New Score to beat?
                        if (score < k)
                            theSurvivors[tNumber].scoreToBeat = score;

                        //Copy contents
                        theSurvivors[tNumber].survivor[j].score = score;
                        memcpy(theSurvivors[tNumber].survivor[j].cipher, theCipher, CIPHER_SIZE);

                    }

                } 
            
                NEXT_THREADP_ITERATION:;

            } while (nextPermutation(ary2, c));



        } while (nextPermutation(ary, c));


    }


    //Close the file
    fclose(fOut);

    return returnVal;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
DWORD WINAPI statusBar(LPVOID lpParam){

    //Local Variables
    COORD pos;
    char percent[4] = { '0', '0', '%', '\0' };
    BYTE percentage = 0;
    ULLONG totalJobsDone;
    UINT i;
    char statusStr[54] = { 0 };
    CONSOLE_SCREEN_BUFFER_INFO bsbiInfo;

    //Initialize ouput string
    for (i = 0; i < 53; i++)
        statusStr[i] = 0x20;

    //Get a handle to the console
    hConsole_out = GetStdHandle(STD_OUTPUT_HANDLE);
    
    //Get Information about the console
    GetConsoleScreenBufferInfo(hConsole_out, &bsbiInfo);

    //Set x,y values to where we want to print out the status bar
    pos.Y = bsbiInfo.dwCursorPosition.Y + 2;
    pos.X = 10;

    while (TRUE){

        //Get percentage of jobs done
        totalJobsDone = 0;
        for (i = 0; i < THREAD_COUNT; i++)
            totalJobsDone += jobsDone[i];

        percentage = (BYTE)(((double)totalJobsDone / jobsToDo) * 100);

        //Convert this percentage to a string
        sprintf(percent, "%u", percentage);

        //Add progress chars to the bar
        for (i = 0; i < (UINT)(percentage / 2); i++){
            if (!(i < 52))
                break;
            statusStr[i + 1] = '=';
        }
            
        //Add percentage numerical values to bar
        statusStr[25] = percent[0];
        statusStr[26] = percent[1];
        statusStr[27] = '%';
        statusStr[0] = '<';
        statusStr[52] = '>';
        statusStr[53] = '\0';

        //Print a status percent bar to indicate progress
        SetConsoleCursorPosition(hConsole_out, pos);
        WriteConsoleA(hConsole_out, statusStr, 53, NULL, NULL);

        if (weAreDone == TRUE){

            //Fill in the bar the rest of the way
            for (i = 0; i < 53; i++)
                statusStr[i + 1] = '=';

            //Add percentage numerical values to bar
            statusStr[25] = '9';
            statusStr[26] = '9';
            statusStr[27] = '%';
            statusStr[0] = '<';
            statusStr[52] = '>';
            statusStr[53] = '\0';

            //Print a status percent bar to indicate progress
            SetConsoleCursorPosition(hConsole_out, pos);
            WriteConsoleA(hConsole_out, statusStr, 53, NULL, NULL);

            break;
        }
            
        else //Sleep
            Sleep(THREAD_SLEEP_TIME_STAT);
    }

    //Close handle
    //CloseHandle(hConsole_out);

    return 0;
    
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BOOL converter_offset_string_2CharsOnly(BYTE* offset, char* str, BYTE conversionDirection){

    /*Depending on the direction, either convert UINT to char* or vice-versa*/
    if (conversionDirection == 0){  //offset to str

        if (*offset > LETTERS_IN_ALPHABET - 1)
            return FALSE;

        if (*offset >= 20){
            str[0] = '2';
            str[1] = (*offset % 20) + '0';
        }
        else if (*offset >= 10){
            str[0] = '1';
            str[1] = (*offset % 10) + '0';
        }
        else{
            str[0] = '0';
            str[1] = *offset + '0';
        }

    }
    else if (conversionDirection == 1){  //str to offset

        if ((str[0] >= '2') && (str[1] >= '6'))
            return FALSE;

        if (str[0] >= '2'){
            *offset = 20 + (str[1] - '0');
        }
        else if (str[0] >= '1'){
            *offset = 10 + (str[1] - '0');
        }
        else{
            *offset = str[1] - '0';
        }

    }
    else{
        return FALSE;
    }

    return TRUE;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
UINT assignCipherScore(char* theCipher, UINT sizeofCipher){

    //Local Variables
    UINT theScore = 0;
    UINT i, j;
    UINT maxWordLength;
    char compareResults;
    BYTE currentWordLength = 0;
    char** const** dPtr = NULL;

    /*Walk through the passed in cipher and look for words. Scores will be given based upon the letter
    count of the word found. Formula: theScore += (sizeof(word)^2) */
    i = -1;
    while (TRUE){

        i++;

        //Are we done?
        if (theCipher[i] == '\0')
            break;

        //Determine the available max word length
        if ((sizeofCipher - i) >= MAX_WORD_SIZE)
            maxWordLength = MAX_WORD_SIZE;
        else
            maxWordLength = (MAX_WORD_SIZE - (sizeofCipher - (i + 1)));

        //Determine which dictionary we are using
        dPtr = getDictionaryPtr(theCipher[i]);

        if (i >= sizeofCipher)
            goto RETURN_SCORE;

        //Walk through dictionary starting at longest word and going to shortest
        j = -1;
        while (TRUE){

            j++;

            if (dPtr[j] == '\0'){ //End of dictionary
                break;
            }

            //Get the current word length we are comparing
            currentWordLength = strlen((const char*)dPtr[j]);
            
            if (currentWordLength > maxWordLength){ //Go to next word in dictionary
                continue;
            }

            //Compare the strings
            compareResults = memcmp((theCipher + i), dPtr[j], currentWordLength);

            //Results?
            if ((compareResults == 0)){ //We have a match! Save and read next CIPHER

                //Update Score
                theScore += (UINT)pow(currentWordLength, 2);

                //Skip ahead
                i += (currentWordLength - 1);

                break;
            }

            else if (compareResults > 0){ //WE have surpassed it. Move on to next letters
                
                while (TRUE){

                    j++;

                    if (dPtr[j] == '\0'){ //End of dictioary
                        j--;
                        break;
                    }

                    else if (strlen((const char*)dPtr[j]) < currentWordLength){
                        j--;
                        break;
                    }
                }
            }
        }

    }

    RETURN_SCORE:

    return theScore;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
char** const** getDictionaryPtr(char letter){

    //Returns a pointer to a dictionary which starts with the argument letter
    if      (letter == 'A') return (char** const**)WORDBANK_A;
    else if (letter == 'B') return (char** const**)WORDBANK_B;
    else if (letter == 'C') return (char** const**)WORDBANK_C;
    else if (letter == 'D') return (char** const**)WORDBANK_D;
    else if (letter == 'E') return (char** const**)WORDBANK_E;
    else if (letter == 'F') return (char** const**)WORDBANK_F;
    else if (letter == 'G') return (char** const**)WORDBANK_G;
    else if (letter == 'H') return (char** const**)WORDBANK_H;
    else if (letter == 'I') return (char** const**)WORDBANK_I;
    else if (letter == 'J') return (char** const**)WORDBANK_J;
    else if (letter == 'K') return (char** const**)WORDBANK_K;
    else if (letter == 'L') return (char** const**)WORDBANK_L;
    else if (letter == 'M') return (char** const**)WORDBANK_M;
    else if (letter == 'N') return (char** const**)WORDBANK_N;
    else if (letter == 'O') return (char** const**)WORDBANK_O;
    else if (letter == 'P') return (char** const**)WORDBANK_P;
    else if (letter == 'Q') return (char** const**)WORDBANK_Q;
    else if (letter == 'R') return (char** const**)WORDBANK_R;
    else if (letter == 'S') return (char** const**)WORDBANK_S;
    else if (letter == 'T') return (char** const**)WORDBANK_T;
    else if (letter == 'U') return (char** const**)WORDBANK_U;
    else if (letter == 'V') return (char** const**)WORDBANK_V;
    else if (letter == 'W') return (char** const**)WORDBANK_W;
    else if (letter == 'X') return (char** const**)WORDBANK_X;
    else if (letter == 'Y') return (char** const**)WORDBANK_Y;
    else if (letter == 'Z') return (char** const**)WORDBANK_Z;
    else return NULL;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BOOL setUpPermutationAry(UINT* aryIn, UINT size, BOOL binary, UINT zeroCount){

    //Sets up an array for permutation. Must be lowest possible order.

    //Local Variables
    UINT i;

    if (binary){
        if (zeroCount > size)
            return FALSE;

        for (i = 0; i < zeroCount; i++)
            aryIn[i] = 0;

        for (i = zeroCount; i < size; i++)
            aryIn[i] = 1;
    }
    else{
        for (i = 0; i < size; i++)
            aryIn[i] = i;
    }

    return TRUE;

}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
UINT factorial(UINT n){

    if (n == 1 || n == 0)
        return 1;
    else
        return n * factorial(n - 1);
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
UINT choose(UINT a, UINT b){

    //n = (a! / ((b! (a-b)!)))
    return ((factorial(a) / (factorial(b) * factorial(a - b))));
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
ULLONG calculateJobsToDo(UINT cipherLength, UINT maxKeyLength, UINT iterations){

    //Local Variables
    ULLONG n = 0;
    ULLONG m, p;
    UINT i, j;

    //n! + (n-1)! + ... + 1!
    for (i = 1; i <= maxKeyLength; i++){

        //How many leftovers would we have?
        j = i - (cipherLength % i);
        if (j == i) j = 0;
        //Calculate added permutations
        m = choose(i, j);
        p = factorial(i);

        n += (m * p);
    }

    //Finally, multiply this number by the iterations
    return (n * iterations);
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void printOutFinalResults(void){

    fprintf(stdout, "\n\nMy guesses are: \n\n");

    UINT i, j;
    BYTE k = 0;

    for (i = 0; i < THREAD_COUNT; i++){

        for (j = 0; j < SURVIVAL_COUNT; j++){

            if (theSurvivors[i].survivor[j].score != 0){
                k++;
                fprintf(stdout, "%s", theSurvivors[i].survivor[j].cipher);
                fprintf(stdout, "\n");
            }
        }
    }

    if (k == 0)
        fprintf(stdout, "Well, guess you better lower the minimal survival score and try again...\n");

}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BYTE createWordBank(void){

    //Local Variables
    BYTE returnVal = 0;
    UINT i, j;
    UINT charsRead;
    char outputWord[MAX_WORD_SIZE] = { 0 };
    char* outWord = (outputWord + (2 * sizeof(char)));
    outputWord[0] = '{';
    outputWord[1] = '"';
    FILE *fIn;
    FILE *fOut[LETTERS_IN_ALPHABET];

    char fileNameIn[FILENAME_LENGTH] = { 0 };
    char fileNameOut[FILENAME_LENGTH] = { 0 };
    FILE *fptrIn = NULL;
    FILE *fptrOut = NULL;

    //Prompt for input file name
    fprintf(stdout, "Input File: ", FILENAME_LENGTH);

    //Get the user input
    if (fgets(fileNameIn, FILENAME_LENGTH, stdin) == NULL)
        return (returnVal | FILE_NAME_ERROR);

    //Eliminate the '\n'
    for (i = 0; i < FILENAME_LENGTH; i++){

        if (fileNameIn[i] == '\n'){
            fileNameIn[i] = '\0';
            break;
        }

    }

    //Open Files
    if ((fIn = fopen(fileNameIn, "r")) == NULL){
        returnVal = (returnVal | FILE_OPEN_ERROR);
        return returnVal;
    }

    for (i = 0; i < LETTERS_IN_ALPHABET; i++){

        sprintf(fileNameOut, "%c%s", 'A' + i, WORD_BANK);

        if ((fOut[i] = fopen(fileNameOut, "w")) == NULL){
            returnVal = (returnVal | FILE_OPEN_ERROR);
            return returnVal;
        }

    }
    

    //Now, go through the file. Read and write.
    j = -1;
    while (TRUE){

        //Read next group of chars
        if ((charsRead = fread(bufferIn, 1, READ_BUFFER_SIZE, fIn)) == 0){
            //We didn't read anything. EOF
            break;
        }

        //Adjust the buffer to end with a complete word
        for (i = 1; i <= MAX_WORD_SIZE; i++){
            if (bufferIn[charsRead - i] == '\n'){
                //Set null terminator to show end of buffer
                bufferIn[charsRead - i] = '\0';
                //Adjust number of chars read
                charsRead -= (i - 1);
                //Set appropriate place in file
                fseek(fIn, 1 - i, SEEK_CUR);
                break;
            }
        }

        //Write to the appropriate files
        for (i = 0; i < charsRead; i++){

            j++;
            outWord[j] = toupper(bufferIn[i]);

            if (bufferIn[i] == '\n' || bufferIn[i] == '\0'){ //End of word reached

                //Append ending chars
                outWord[j]     = '"';
                outWord[j + 1] = '}';
                outWord[j + 2] = ',';
                outWord[j + 3] = '\0';

                //Write to file
                fprintf(fOut[outWord[0] - 'A'], "%s", outputWord);

                //Update count
                j = -1;

                if (bufferIn[i] == '\0')
                    break;
            }
        }
    }

    //Close files
    fclose(fIn);
    for (i = 0; i < LETTERS_IN_ALPHABET; i++){
        fclose(fOut[i]);
    }

    return returnVal;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//This following function was taken from: 
//http://www.nayuki.io/page/next-lexicographical-permutation-algorithm
//Slightly modified to work with C, but props go to the real creator!
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
BOOLEAN nextPermutation(int array[], int arrayLength) {
    // Find longest non-increasing suffix
    int i = arrayLength - 1;
    while (i > 0 && array[i - 1] >= array[i])
        i--;
    // Now i is the head index of the suffix

    // Are we at the last permutation already?
    if (i <= 0)
        return FALSE;

    // Let array[i - 1] be the pivot
    // Find rightmost element that exceeds the pivot
    int j = arrayLength - 1;
    while (array[j] <= array[i - 1])
        j--;
    // Now the value array[j] will become the new pivot
    // Assertion: j >= i

    // Swap the pivot with j
    int temp = array[i - 1];
    array[i - 1] = array[j];
    array[j] = temp;

    // Reverse the suffix
    j = arrayLength - 1;
    while (i < j) {
        temp = array[i];
        array[i] = array[j];
        array[j] = temp;
        i++;
        j--;
    }

    // Successfully computed the next permutation
    return TRUE;
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$


#endif