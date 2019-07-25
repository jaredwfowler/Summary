/***************************************************************************************************
 *
 *                                  _._     _,-'""`-._
 *                                (,-.`._,'(       |\`-/|
 *                                    `-.-' \ )-`( , o o)
 *                                          `-    \`_`"'-       LaserChaser.org
 *
 ***************************************************************************************************
 *
 * Created By: Jared Fowler
 * Date: July 14, 2019
 *
 **************************************************************************************************/

#ifndef DECRYPTER_424_H
#define DECRYPTER_424_H

#include <stdint.h>
#include <string.h>
#include <string>

#include "WordBank424_English.h"

// -------------------------------------------------------------------------------------------------

#define DECRYPTER_424_VERSION_MAJOR                 0
#define DECRYPTER_424_VERSION_MINOR                 3

// -------------------------------------------------------------------------------------------------
// VERSION      MESSAGE
//
//   0.4        TODO - Read user input from a file. Write results to a file.
//
//   0.3        Upgraded language library to use a binomial search. This places the entire word
//              dictionary into a single array in alphabetical order. This should be about 7 times
//              faster than the old method. (Separate dictionaries for each letter ordered from
//              longest word to shortest.) Say there were 1000 words per letter, so a total of
//              26000 words. Per the old method, there would be an average of 13 comparisons for
//              the English letter dictionary. On average, all 1000 string comparisons would be
//              performed (or at minimum strlen operations) b/c most decipher candidates are not
//              correct. So, a total of 1013 comparisons per the old method. The new method would
//              have a maximum word length to consider, say 10. For each word length, binary search
//              into the dictionary and try to find a match. With a dictionary length of 26000,
//              this would be 10*ln_2(26000) = 150 comparisons.
//
//   0.2        Updated with a GUI console interface.
//
//   0.1        Working!
//
// -------------------------------------------------------------------------------------------------

// Add a simplified define for windows versus unix system.

#if defined(_MSC_VER) || defined(__WINDOWS__)
#define WINDOWS_OS
#else
#define UNIX_OS
#endif

// -------------------------------------------------------------------------------------------------

// Declare various constants and structures within a namespace as to avoid possible collisions with
// other libraries.

namespace D424
{
//--------------------------------------------------------------------------------------------------
// Structures --------------------------------------------------------------------------------------

/*
 * status_t - General status of the algorithm. Ideally, a separate thread would get a copy of the
 * status to determine what the algorithms are currently doing, and how much more work remains.
 *
 *      allFlags            Easy way to clear/set all flags at once
 *      initialized         Has the initialize function been ran and completed successfully?
 *      busy                Is there a current job active?
 *      refreshConsole      Does the console need to be cleared?
 *      stopOps             Signals internal operations to stop and go back to idle state.
 *      terminate           Set when program should shutdown - Indicator to threads.
 *      totalJobs           Calculated before the start of the columnar algorithm. Each job
 *                          represents a single column length and order permutation. This single
 *                          job entails comparing the generated "decoded" text to the dictionary
 *                          and assigning it a score.
 *      jobsDone            Total number of completed jobs in relation to totalJobs
 */

struct status_t
{
    union
    {
        uint8_t allFlags;
        struct
        {
            uint8_t initialized         : 1;
            uint8_t busy                : 1;
            uint8_t shiftAnalysisDone   : 1;
            uint8_t reserved            : 2;
            uint8_t refreshConsole      : 1;
            uint8_t stopOps             : 1;
            uint8_t terminate           : 1;
        };
    };

    uint64_t totalJobs;
    uint64_t jobsDone;

    status_t()
      : allFlags(0), totalJobs(0), jobsDone(0)
    { }
};

/*
 * parameters_t - Define the behavior of the program.
 *
 *      cypher              The cypher text we want to decode
 *      minColumns          Minimum number of columns to analyze. Error if 0. Recommended as 1.
 *      maxColumns          Maximum number of columns to analyze. Must be greater than minColumns.
 *                          Because the workload increases exponentially the larger this number
 *                          gets, it's recommended that it be set below 20.
 *      maxSurvivors        Number of best possible deciphers to keep during the analysis process.
 *      testMaxWordSize     The maximum word length to compare with the dictionary word list. For
 *                          best results, this should probably be at a maximum of 8-10 for English.
 *      testMinWordSize     The minimum word length to compare to the dictionary word list. This
 *                          should probably be put around 3 for better scoring results. (Avoid
 *                          strange 1-2 letter words that might take away from a larger word.)
 */

struct parameters_t
{
    std::string cypher;
    uint8_t minColumns;
    uint8_t maxColumns;
    uint8_t maxSurvivors;
    uint8_t testMaxWordSize;
    uint8_t testMinWordSize;

    parameters_t(void)
      : cypher("HelloWorld"),
        minColumns(1),
        maxColumns(12),
        maxSurvivors(5),
        testMaxWordSize(10),
        testMinWordSize(3)
    { }
};

/*
 * language_info_t - Information regarding the language. The structure has been setup to allow
 * an easier plug-in of other language libraries.
 *
 *      LETTER_COUNT        Number of letters in this language's alphabet
 *      WordLookupFuncPtr   Pointer to a function which looks up the provided string of length N,
 *                          in the dictionary. The offset is returned if found, else, -1.
 *      FrequencyFuncPtr    Pointer to a function which returns the ratio of how many times that
 *                          letter is used in the entire dictionary to all letters.
 *      CharToValueFuncPtr  Returns the integer mapped value for a given character.
 *      ValueToCharFuncPtr  Returns the character mapped to by the given integer value.
 */

struct language_info_t
{
    const uint16_t* LETTER_COUNT;
    int64_t (*WordLookupFuncPtr)(const char*, const uint8_t);
    float (*FrequencyFuncPtr)(const char);
    int32_t (*CharToValueFuncPtr)(const char);
    char (*ValueToCharFuncPtr)(const uint16_t);

    language_info_t& operator=(const language_info_t& _other)
    {
        if (this != &_other) // self-assignment check
        {
            memcpy(this, &_other, sizeof(language_info_t));
        }

        return *this;
    }
};

/*
 * decryption_t - A single decryption.
 *
 *      score               Decided based upon the number of words in the text and dictionary match.
 *                          Longer words give more points than shorter words. Formula: N^2
 *      shift               The letter shift at which this decryption occurred.
 *      columns             The number of columns at which this decryption occurred.
 *      text                The decrypted text.
 */

struct decryption_t
{
    uint32_t score;
    uint16_t shift;
    uint8_t columns;
    std::string text;

    decryption_t()
      : score(0), shift(0), columns(0), text("")
    { }

    decryption_t(uint32_t _score, uint16_t _shift, uint8_t _columns, std::string _text)
      : score(_score), shift(_shift), columns(_columns), text(_text)
    { }

    bool operator < (const decryption_t& _other) const
    {
        return (score < _other.score);
    }
};

/*
 * decryptions_t - A collection of decryption structures.
 *
 *      list                Dynamic list of decryption_t
 *      scoreToBeat         Reflects the lowest score in the decryption_t list
 */

struct decryptions_t
{
    decryption_t* list;
    uint32_t scoreToBeat;

    decryptions_t()
      : list(NULL), scoreToBeat(0)
    { }
};

/*
 * letter_shift_t - A single letter shift result and its score.
 *
 *      text                The shifted text.
 *      score               How likely it is to be the correct shift. (Higher the better.)
 *      shift               Shift value ranging from 0 - (LETTER_COUNT - 1)
 */

struct letter_shift_t
{
    std::string text;
    uint32_t score;
    uint16_t shift;

    letter_shift_t()
      : text(""), score(0), shift(0)
    { }
};

//--------------------------------------------------------------------------------------------------
// Error Codes -------------------------------------------------------------------------------------

/*
 * error_t - List of errors which can occur. This list much match perfectly with the ERROR_MSG list.
 */

enum error_t
{
    ERROR_NONE                                      = 0,
    ERROR_PARAM_1                                   = 1,
    ERROR_PARAM_2                                   = 2,
    ERROR_PARAM_3                                   = 3,
    ERROR_PARAM_4                                   = 4,
    ERROR_PARAM_N                                   = 5,
    ERROR_INITIALIZE_PARAMETERS                     = 6,
    ERROR_INITIALIZE_LANGUAGE                       = 7,
    ERROR_DYNAMIC_NEWING_NON_NULL                   = 8,
    ERROR_CYPHER_EMPTY_STRING                       = 9,
    ERROR_CYPHER_NON_SUPPORTED_CHAR                 = 10,
    ERROR_STATUS_NOT_INITIALIZED                    = 11,
    ERROR_STATUS_BUSY                               = 12,
    ERROR_CRITICAL_1                                = 13,
    ERROR_CRITICAL_2                                = 14,
    ERROR_STOP_REQUESTED                            = 15,

    ERROR_LIST_END
};

/*
 * ERROR_MSG - Error descriptions. This list much match perfectly with the error_t list.
 */

const char ERROR_MSG[ERROR_LIST_END][64] =
{
    { "No error."                                       },
    { "Parameter 1 invalid."                            },
    { "Parameter 2 invalid."                            },
    { "Parameter 3 invalid."                            },
    { "Parameter 4 invalid."                            },
    { "Parameter(s) invalid."                           },
    { "Initialization - Invalid parameter structure."   },
    { "Initialization - Invalid language structure."    },
    { "Internal Dynamic Struct - Newing non-NULL."      },
    { "Cypher - Empty string."                          },
    { "Cypher - Unsupported character(s)."              },
    { "Initialization not completed."                   },
    { "Decrypter already performing an operation."      },
    { "LetterShiftAnalysis - Character to Offset -1."   },
    { "LetterShiftAnalysis - Offset to Character NULL." },
    { "User Interface - Stop Requested."                }
};

//--------------------------------------------------------------------------------------------------
// Prototypes --------------------------------------------------------------------------------------

/*
 * IsValidCypher - Determines if the input cypher is supported by the dictionary library.
 *
 *      _cypher             The cypher text
 *      _charToValueFuncPtr Pointer to language library function which determines the integer value
 *                          of the input character. -1 is returned if not supported.
 */

error_t IsValidCypher(const std::string& _cypher,
                      int32_t (*_charToValueFuncPtr)(const char) = NULL);

/*
 * GetStatus - Returns a copy of the internal status structure.
 */

status_t GetStatus(void);

/*
 * Initialize - Needs to be called and successfully executed before the deciphering algorithms can
 * run. Parameter and language information are copied to internal data structures and are thereby
 * "locked-in" and cannot be changed unless another initialize is done.
 *
 *      _parameters         A parameter data structure must be filled out by the caller which will
 *                          be copied to an internal data structure. Once initialization is
 *                          completed, free to delete their copy.
 *      _language           Due to the largeness of this structure, pointers are used to reference
 *                          read-only memory. This project comes pre-equipped with an English
 *                          library which will be used if the argument is left NULL, else, the user
 *                          may define their own library based upon the provided dictionary format.
 */

error_t Initialize(const parameters_t* const _parameters,
                   const language_info_t* const _language = NULL);

/*
 * Destruct - Cleans up any left-over resources. Should be called from main programming once done
 * using this library.
 */

error_t Destruct(void);

/*
 * Delivers signal to stop all current operations and return to idle state
 */

void StopOperations(void);

/*
 * Delivers signal to terminate the program
 */

void Terminate(void);

/*
 * Handles console output. Thread should be created and called from client side code.
 */

void UserInterfaceGuiThread(void);

/*
 * Handles keyboard input. Thread should be created and called from client side code.
 */

void UserInterfaceInputThread(void);

}  // namespace D424
#endif  // DECRYPTER_424_H
