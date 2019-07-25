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

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <thread>
#include "Decrypter424.h"

#if defined(WINDOWS_OS)

#include <Windows.h>

#define GUI_CLEAR_SCREEN                system("cls")
#define GET_PID()                       (GetCurrentProcessId())
#define SLEEP_MS(A)                     (Sleep(A))
#define GET_CHAR()                      (_getchar_nolock())

#else

#include <unistd.h>

#define GUI_CLEAR_SCREEN                system("clear")
#define GET_PID()                       (getpid())
#define SLEEP_MS(A)                     (usleep(A * 1000))
#define GET_CHAR()                      (getchar())

#endif

// -------------------------------------------------------------------------------------------------
// Prototypes --------------------------------------------------------------------------------------

/*
 * letterShiftAnalysis - Assigns a score to all the letter shifting possibilities.
 */

D424::error_t letterShiftAnalysis(void);

/*
 * columnWithLetterShiftAnalysis - Performs the letter shift analysis if it hasn't been done
 * already. Then proceeds to decipher the cipher text with the behavior setup in initialize.
 */

D424::error_t columnWithLetterShiftAnalysis(void);

/*
 * calculateTotalJobs - Calculates the total number of "jobs" to do. A single job is the dictionary
 * lookup and scoring work that needs to be done for a single shift-columnCount-columnOrder
 * permutation.
 *
 *      _cypherLength       Length of the cypher text
 *      _minColumns         Minimum number of columns to analyze.
 *      _maxColumns         Maximum number of columns to analyze.
 *      _letterCount        Number of letters in the language. THis also serves as the maximum
 *                          number of shifts.
 */

uint64_t calculateTotalJobs(const uint32_t _cypherLength, const uint8_t _minColumns,
                            const uint8_t _maxColumns, const uint16_t _letterCount);

/*
 * constructInternalDynamicStructures - Gets the dynamic memory structures ready. The structures
 * are dynamic to accomodate the users dynamic input, in particular the cypher size and number of
 * "survivors" the user wants to hold on to.
 */

D424::error_t constructInternalDynamicStructures(void);

/*
 * destructInternalDynamicStructures - Cleans up the dynamic memory created in the construct 
 * function.
 */

D424::error_t destructInternalDynamicStructures(void);

/*
 * scoreLetterShift - Returns a score associated with a single letter shift input. The score is
 * calculated based upon letter frequency correspondance to the dictionary information table.
 */

float scoreLetterShift(const std::string& _str);

/*
 * sortLetterShiftArray - Orders a provided list of letter_shift_t structures from highest to
 * lowest based upon score.
 *
 *      _ary                Pointer to the list
 *      _size               Size of the list
 */

D424::error_t sortLetterShiftArray(D424::letter_shift_t* _ary, uint16_t _size);

/*
 * factorial - Uses recurssion to calculate the factorial. ( n! )
 */

uint64_t factorial(uint64_t _arg);

/*
 * doColumnPermutation - Iterates through all the possible permutations for a given cypher with a
 * maximum of N columns. Calls getNextPermutation and scoreDecipheredText. 
 *
 *      _cypher             The cypher text. (pre-shifted) Determined best left as a char* for 
 *                          faster/easier memory copy.
 *      _shifts             This is meta data, not actually used to shift the cypher. The shift
 *                          value is included in the survivor list structure.
 *      _columns            Number of columns. Because in columnar encryption the longest columns
 *                          need to go  beforethe shorter ones, the permutations are calculated as:
 *                          (L!)*(S!) where (L + S) = Total Columns (C). This is much more 
 *                          efficient than (C!)
 */

D424::error_t doColumnPermutation(const char* _cypher,
                                  const uint16_t _shifts,
                                  const uint8_t _columns);

/*
 * getNextPermutation - Gets the next permutation of a passed in integer array. It is essential
 * that the array be initialized with integers in ascending order. The function relies upon 
 * this, and will end once the order is reversed. This code was originally taken from:
 * http://www.nayuki.io, and was then modified. Returns true if next permutation found, else,
 * returns false if the array is already at its last permutation.
 * 
 *
 *      _ary                Pointer to the integer array. The array will be updated to the next
 *                          permutation. The original input array should be ordered and contain 
 *                          values 0-N
 *      _size               Size of the array
 */

bool getNextPermutation(uint8_t* _ary, const uint8_t _size);

/*
 * scoreDecipheredText - Compares a given string to the registered dictionary and assigns a 
 * score based upon the number and length of matching words. Starting with the largest 
 * possible word size, queries if the word exists. If it does, the word score is added to the
 * total score, else, the next word size is attempted (N - 1) and so on... Takes into consideration
 * the max and min word sizes established in the parameter structure. 
 *
 *
 *      _decipheredText     Deciphered text string to compare to dictionary.
 *      _size               Size of the string
 *      _score              Reference to score, passed by caller. Set to zero when the function 
 *                          first starts and added upon through the scoring process.
 */

D424::error_t scoreDecipheredText(const char* _decipheredText,
                                  const uint32_t _size,
                                  uint32_t& _score);
/*
 * addNewSurvivor - Adds a new deciphered text and its meta data to the survivor list, then 
 * sorts it to be in descending order. Will only add the new survivor if its score is greater
 * than the lowest score on the list. (This check should be done before hand.)
 *
 *
 *      _survivor           decryption_t structure, which contains the deciphered text, the score,
 *                          and the shift and number of columns at which it was found.
 */

D424::error_t addNewSurvivor(const D424::decryption_t& _survivor);

// -------------------------------------------------------------------------------------------------

/*
 * Keep a internal copy of the data structures at the global level. This provides easy access to
 * the helper functions and protects it agains external user misuse. These values can only be 
 * changed through the initialize funciton which goes through the process of error checking.
 */

static D424::language_info_t language;
static D424::parameters_t parameters;

/*
 * Dynamic structures which will be modified based upon parameter input value. These hold the 
 * letter shift outputs as well as the list of "survivors" which are the top possible 
 * solutions gathered during the decryption algorithm.
 */

static D424::decryptions_t decryptions;
static D424::letter_shift_t* lettershifts = NULL;

/*
 * Status flags which help the flow of the program. These flags are also useful for cross-thread
 * communication. 
 */

static D424::status_t status;

// -------------------------------------------------------------------------------------------------

D424::error_t D424::IsValidCypher(const std::string& _cypher,
                                  int32_t (*_charToValueFuncPtr)(const char))
{
    D424::error_t error = D424::ERROR_NONE;

    // Must not be an empty string

    if (0 == parameters.cypher.length())
    {
        error = D424::ERROR_CYPHER_EMPTY_STRING;
        goto Decrypter424_IsValidCypher_End;
    }

    // Support for characters is determined by language structure ....

    if (NULL == _charToValueFuncPtr)
    {
        _charToValueFuncPtr = WordBank424_English::GetLetterOffset;
    }

    for (uint32_t i = 0; i < _cypher.length(); i++)
    {
        if (-1 == _charToValueFuncPtr(_cypher[i]))
        {
            error = D424::ERROR_CYPHER_NON_SUPPORTED_CHAR;
            goto Decrypter424_IsValidCypher_End;
        }
    }

    Decrypter424_IsValidCypher_End:
    return error;
}

// -------------------------------------------------------------------------------------------------

D424::status_t D424::GetStatus(void)
{
    return status;
}

// -------------------------------------------------------------------------------------------------

D424::error_t D424::Initialize(const D424::parameters_t* const _parameters,
                               const D424::language_info_t* const _language)
{
    D424::error_t error = D424::ERROR_NONE;

    // Clean up existing structures and reset initialization status to false

    status.allFlags = 0;

    error = destructInternalDynamicStructures();
    if (D424::ERROR_NONE != error)
    {
        goto Decrypter424_Initialize_End;
    }

    // Input Validation - Early return if invalid.

    if (NULL == _parameters)
    {
        error = D424::ERROR_PARAM_1;
        goto Decrypter424_Initialize_End;
    }

    // Copy input parameters into internal structure

    parameters = *_parameters;

    // If present, copy language structure, else, use pre-equipped English

    if (NULL != _language)
    {
        language = *_language;
    }
    else
    {
        language.LETTER_COUNT = &WordBank424_English::LETTERS_IN_ALPHABET;
        language.WordLookupFuncPtr = WordBank424_English::WordLookup;
        language.FrequencyFuncPtr = WordBank424_English::FrequencyLookup;
        language.CharToValueFuncPtr = WordBank424_English::GetLetterOffset;
        language.ValueToCharFuncPtr = WordBank424_English::GetOffsetLetter;
    }

    // Validate initialization

    error = D424::IsValidCypher(parameters.cypher);
    if (D424::ERROR_NONE != error)
    {
        goto Decrypter424_Initialize_End;
    }
    else if ((0 == parameters.minColumns) ||
             (0 == parameters.maxColumns) ||
             (parameters.minColumns > parameters.maxColumns) ||
             (parameters.testMinWordSize > parameters.testMaxWordSize) ||
             (0 == parameters.maxSurvivors))
    {
        error = D424::ERROR_INITIALIZE_PARAMETERS;
        goto Decrypter424_Initialize_End;
    }
    else if ((NULL == language.LETTER_COUNT) ||
             (NULL == language.WordLookupFuncPtr) ||
             (NULL == language.FrequencyFuncPtr) ||
             (NULL == language.CharToValueFuncPtr) ||
             (NULL == language.ValueToCharFuncPtr))
    {
        error = D424::ERROR_INITIALIZE_LANGUAGE;
        goto Decrypter424_Initialize_End;
    }

    // Convert cypher to all upper-case

    for (uint32_t i = 0; i < parameters.cypher.length(); i++)
    {
        parameters.cypher[i] = toupper(parameters.cypher[i]);
    }

    // Initialize internal dynamic structures

    error = constructInternalDynamicStructures();
    if (D424::ERROR_NONE != error)
    {
        goto Decrypter424_Initialize_End;
    }

    status.initialized = 1;

    Decrypter424_Initialize_End:
    return error;
}

// -------------------------------------------------------------------------------------------------

void D424::StopOperations(void)
{
    status.stopOps = 1;
}

// -------------------------------------------------------------------------------------------------

void D424::Terminate(void)
{
    status.stopOps = 1;
    status.terminate = 1;
}

// -------------------------------------------------------------------------------------------------

D424::error_t D424::Destruct(void)
{
    D424::error_t error = D424::ERROR_NONE;

    destructInternalDynamicStructures();
    status.initialized = 0;

    return error;
}

// -------------------------------------------------------------------------------------------------

D424::error_t letterShiftAnalysis(void)
{
    D424::error_t error = D424::ERROR_NONE;

    // Can this operation be performed now? Check status bits.

    if (0 == status.initialized)
    {
        error = D424::ERROR_STATUS_NOT_INITIALIZED;
        goto Decrypter424_letterShiftAnalysis_End;
    }
    else if (1 == status.busy)
    {
        error = D424::ERROR_STATUS_BUSY;
        goto Decrypter424_letterShiftAnalysis_End;
    }

    // Set the busy flag to indicate we are in the middle of operation

    status.busy = 1;

    // For each offset possibility, create the new string, analyze it, and assign it a score.

    for (uint16_t i = 0; i < *(language.LETTER_COUNT); i++)
    {
        for (uint32_t j = 0; j < parameters.cypher.length(); j++)
        {
            // Get the original character offset

            int32_t letterOffset = language.CharToValueFuncPtr(parameters.cypher[j]);

            // Validate the offset value

            if (-1 == letterOffset)
            {
                error = D424::ERROR_CRITICAL_1;
                goto Decrypter424_letterShiftAnalysis_Clear_Busy;
            }

            // Shift the offset. Take wrapping into account.

            letterOffset = ((uint16_t)(letterOffset + i) % *(language.LETTER_COUNT));

            // Get the shifted letter

            char offsetChar = language.ValueToCharFuncPtr(letterOffset);

            // Validate the letter value

            if (0 == offsetChar)
            {
                error = D424::ERROR_CRITICAL_2;
                goto Decrypter424_letterShiftAnalysis_Clear_Busy;
            }

            // Store the character

            lettershifts[i].text[j] = offsetChar;
        }

        lettershifts[i].shift = i;
        lettershifts[i].score = (uint32_t)scoreLetterShift(lettershifts[i].text);
    }

    // Sort according to score - Highest to Lowest

    error = sortLetterShiftArray(lettershifts, *(language.LETTER_COUNT));

    if (D424::ERROR_NONE != error)
    {
        goto Decrypter424_letterShiftAnalysis_Clear_Busy;
    }

    // Shift analysis completed successfully

    status.shiftAnalysisDone = 1;

    Decrypter424_letterShiftAnalysis_Clear_Busy:
    status.busy = 0;

    Decrypter424_letterShiftAnalysis_End:
    return error;
}

// -------------------------------------------------------------------------------------------------

D424::error_t columnWithletterShiftAnalysis(void)
{
    D424::error_t error = D424::ERROR_NONE;
    uint8_t maxColumns;

    // The shift analysis is required to have ran before this can.

    if (0 == status.shiftAnalysisDone)
    {
        error = letterShiftAnalysis();

        if (D424::ERROR_NONE != error)
        {
            goto Decrypter424_columnWithletterShiftAnalysis_End;
        }
    }

    // Can this operation be performed now? Check status bits.

    if (0 == status.initialized)
    {
        error = D424::ERROR_STATUS_NOT_INITIALIZED;
        goto Decrypter424_columnWithletterShiftAnalysis_End;
    }
    else if (1 == status.busy)
    {
        error = D424::ERROR_STATUS_BUSY;
        goto Decrypter424_columnWithletterShiftAnalysis_End;
    }

    // Set the busy flag to indicate we are in the middle of operation

    status.busy = 1;

    // Set status structure with the number of jobs

    status.jobsDone = 0;
    status.totalJobs = calculateTotalJobs((uint32_t)parameters.cypher.length(),
                                          parameters.minColumns,
                                          parameters.maxColumns,
                                          *(language.LETTER_COUNT));

    // Handle case in which the size of the cypher is smaller than the max number of columns

    maxColumns = parameters.maxColumns;
    if (maxColumns > parameters.cypher.length())
    {
        maxColumns = (uint8_t)parameters.cypher.length();
    }

    // Lots of nested loops ...

    // For every letter shift possibility ...
    for (uint8_t shift = 0; shift < *(language.LETTER_COUNT); shift++)
    {
        // For every column count ...
        for (uint8_t columns = parameters.minColumns; columns <= maxColumns; columns++)
        {
            error = doColumnPermutation(lettershifts[shift].text.c_str(),
                                        lettershifts[shift].shift, columns);
            if (D424::ERROR_NONE != error)
            {
                goto Decrypter424_columnWithletterShiftAnalysis_Clear_Busy;
            }
        }
    }

    Decrypter424_columnWithletterShiftAnalysis_Clear_Busy:
    status.busy = 0;

    Decrypter424_columnWithletterShiftAnalysis_End:
    return error;
}

// -------------------------------------------------------------------------------------------------

uint64_t calculateTotalJobs(const uint32_t _cypherLength, const uint8_t _minColumns,
                            const uint8_t _maxColumns, const uint16_t _letterCount)
{
    uint64_t totalIterations = 0;

    // For each column count, determine how many combinations the
    // columns can be ordered in. Something to remember is that columns
    // which are shorter than the others by 1 character must come after those
    // which are full.

    for (uint8_t columns = _minColumns; columns <= _maxColumns; columns++)
    {
        // How many columns would have 1 extra character?

        uint8_t columnsWithExtraChar = (_cypherLength % columns);
        totalIterations += (factorial(columnsWithExtraChar) *
                            factorial(columns - columnsWithExtraChar));
    }

    // The iteration count found above will be repeated for each possible
    // letter shift combination.

    totalIterations *= _letterCount;

    return totalIterations;
}

// -------------------------------------------------------------------------------------------------

D424::error_t constructInternalDynamicStructures(void)
{
    D424::error_t error = D424::ERROR_NONE;

    if ((NULL != decryptions.list) || (NULL != lettershifts))
    {
        error = D424::ERROR_DYNAMIC_NEWING_NON_NULL;
        goto Decrypter424_constructInternalDynamicStructures_End;
    }

    // Column Decryptions

    decryptions.scoreToBeat = 0;
    decryptions.list = new D424::decryption_t[parameters.maxSurvivors];

    for (uint8_t i = 0; i < parameters.maxSurvivors; i++)
    {
        decryptions.list[i].text.resize(parameters.cypher.length());
        decryptions.list[i].score = 0;
    }

    // Letter Shifts

    lettershifts = new D424::letter_shift_t[*(language.LETTER_COUNT)];

    for (uint16_t i = 0; i < *(language.LETTER_COUNT); i++)
    {
        lettershifts[i].text.resize(parameters.cypher.length());
    }

    Decrypter424_constructInternalDynamicStructures_End:
    return error;
}

// -------------------------------------------------------------------------------------------------

D424::error_t destructInternalDynamicStructures(void)
{
    D424::error_t error = D424::ERROR_NONE;

    // Column Decryptions

    if (NULL != decryptions.list)
    {
        delete [] decryptions.list;
        decryptions.list = NULL;
        decryptions.scoreToBeat = 0;
    }

    // Letter Shifts

    if (NULL != lettershifts)
    {
        delete [] lettershifts;
        lettershifts = NULL;
    }

    return error;
}

// -------------------------------------------------------------------------------------------------

float scoreLetterShift(const std::string& _str)
{
    float score = 0;

    for (uint16_t i = 0; i < _str.length(); i++)
    {
        score += language.FrequencyFuncPtr(_str[i]);
    }

    return score;
}

// -------------------------------------------------------------------------------------------------

D424::error_t sortLetterShiftArray(D424::letter_shift_t* _ary, uint16_t _size)
{
    D424::error_t error = D424::ERROR_NONE;
    D424::letter_shift_t tempObj;
    uint16_t indexOfBiggest;

    if (NULL == _ary)
    {
        D424::error_t error = D424::ERROR_PARAM_1;
        goto Decrypter424_sortLetterShiftArray_End;
    }

    for (uint16_t i = 0; i < (_size - 1); i++)
    {
        indexOfBiggest = i;

        for (uint16_t j = (i + 1); j < _size; j++)
        {
            if (_ary[indexOfBiggest].score < _ary[j].score)
            {
                indexOfBiggest = j;
            }
        }

        tempObj = _ary[i];
        _ary[i] = _ary[indexOfBiggest];
        _ary[indexOfBiggest] = tempObj;
    }

    Decrypter424_sortLetterShiftArray_End:
    return error;
}

// -------------------------------------------------------------------------------------------------

uint64_t factorial(uint64_t _arg)
{
    if (_arg <= 1)
    {
        return 1;
    }
    else
    {
        return (_arg * factorial(_arg - 1));
    }
}

// -------------------------------------------------------------------------------------------------

D424::error_t doColumnPermutation(const char* _cypher,
                                  const uint16_t _shifts,
                                  const uint8_t _columns)
{
    D424::error_t error = D424::ERROR_NONE;
    uint64_t offset;
    uint32_t score;

    // Arrays which will help with the permutations. The arrays will consist of numbers (0 - N)
    // which represent the column order.

    uint8_t* longColumnOrder = NULL;
    uint8_t* shortColumnOrder = NULL;

    // Arrays which will hold the divided out long and short columns. These will then be added
    // together, based upon the long and short column order arrays permutation, to form the possible
    // deciphered text.

    char** longColumns = NULL;
    char** shortColumns = NULL;

    // Combined long and short columns for a single permutation. This final string will be scored
    // based upon dictionary word match.

    char* decipheredText = NULL;

    // Other useful meta information

    uint32_t textLength = (uint32_t)parameters.cypher.length();
    uint8_t longColumnCount = (textLength % _columns);
    uint8_t shortColumnCount = (_columns - longColumnCount);
    uint32_t shortColumnLength = (textLength / _columns);
    uint32_t longColumnLength = shortColumnLength + 1;
    uint64_t largeColumnPermutations = factorial(longColumnCount);
    uint64_t shortColumnPermutations = factorial(shortColumnCount);


    // Allocate and initialize memory

    decipheredText = new char[(textLength + 1)];
    decipheredText[textLength] = 0;

    if (0 != longColumnCount)
    {
        longColumnOrder = new uint8_t[longColumnCount];
        longColumns = new char*[longColumnCount];

        for (uint8_t i = 0; i < longColumnCount; i++)
        {
            longColumnOrder[i] = i;
            longColumns[i] = new char[longColumnLength];
        }
    }

    shortColumnOrder = new uint8_t[shortColumnCount];
    shortColumns = new char*[shortColumnCount];
    for (uint8_t i = 0; i < shortColumnCount; i++)
    {
        shortColumnOrder[i] = i;
        shortColumns[i] = new char[shortColumnLength];
    }

    // Fill the columns with the cypher text

    offset = 0;
    for (uint8_t i = 0; i < longColumnCount; i++)
    {
        memcpy(longColumns[i], (_cypher + offset), longColumnLength);
        offset += longColumnLength;
    }
    for (uint8_t i = 0; i < shortColumnCount; i++)
    {
        memcpy(shortColumns[i], (_cypher + offset), shortColumnLength);
        offset += shortColumnLength;
    }

    // Analyze possible decipher for every column permutation ...

    // For every long column permutation - Long come before short.
    for (uint32_t i = 0; i < largeColumnPermutations; i++)
    {
        // For every short column permutation ...
        for (uint32_t j = 0; j < shortColumnPermutations; j++)
        {
            // Create the decipher text

            offset = 0;
            for (uint32_t x = 0; x < longColumnLength; x++)
            {
                for (uint8_t y = 0; y < longColumnCount; y++)
                {
                    decipheredText[offset] = longColumns[longColumnOrder[y]][x];
                    ++offset;
                }

                if (x < shortColumnLength)
                {
                    for (uint8_t z = 0; z < shortColumnCount; z++)
                    {
                        decipheredText[offset] = shortColumns[shortColumnOrder[z]][x];
                        ++offset;
                    }
                }
            }

            // Analyze the decipher text and assign it a score

            error = scoreDecipheredText(decipheredText, textLength, score);
            if (D424::ERROR_NONE != error)
            {
                goto Decrypter424_doColumnPermutation_Cleanup;
            }

            // Place decipher in survivor list if the score is high enough

            if (score > decryptions.scoreToBeat)
            {
                D424::decryption_t surivor(score, _shifts, _columns, decipheredText);
                decryptions.scoreToBeat = addNewSurvivor(surivor);
            }

            // Increment job done count

            ++status.jobsDone;

            // Has a stop or terminate been requested?

            if (status.stopOps || status.terminate)
            {
                error = D424::ERROR_STOP_REQUESTED;
                goto Decrypter424_doColumnPermutation_Cleanup;
            }

            // Get next permutation

            getNextPermutation(shortColumnOrder, shortColumnCount);
        }

        getNextPermutation(longColumnOrder, longColumnCount);
    }

    Decrypter424_doColumnPermutation_Cleanup:
    // Free memory

    if (NULL != decipheredText)
    {
        delete [] decipheredText;
        decipheredText = NULL;
    }

    if (NULL != longColumns)
    {
        for (uint8_t i = 0; i < longColumnCount; i++)
        {
            if (NULL != longColumns[i])
            {
                delete [] longColumns[i];
                longColumns[i] = NULL;
            }
        }
        delete [] longColumns;
        longColumns = NULL;
    }

    if (NULL != longColumnOrder)
    {
        delete [] longColumnOrder;
        longColumnOrder = NULL;
    }

    if (NULL != shortColumns)
    {
        for (uint8_t i = 0; i < shortColumnCount; i++)
        {
            if (NULL != shortColumns[i])
            {
                delete [] shortColumns[i];
                shortColumns[i] = NULL;
            }
        }
        delete [] shortColumns;
        shortColumns = NULL;
    }

    if (NULL != shortColumnOrder)
    {
        delete [] shortColumnOrder;
        shortColumnOrder = NULL;
    }

    return error;
}

// -------------------------------------------------------------------------------------------------

D424::error_t addNewSurvivor(const D424::decryption_t& _survivor)
{
    D424::error_t error = D424::ERROR_NONE;
    uint16_t indexOfBiggest;
    D424::decryption_t tempObj;

    for (int16_t i = (parameters.maxSurvivors - 1); i >= 0; i--)
    {
        if (decryptions.list[i].score < _survivor.score)
        {
            // Replace the lowest score with this new one

            decryptions.list[i] = _survivor;

            // Re-sort the list in ascending order

            for (uint16_t i = 0; i < (parameters.maxSurvivors - 1); i++)
            {
                indexOfBiggest = i;

                for (uint16_t j = (i + 1); j < parameters.maxSurvivors; j++)
                {
                    if (decryptions.list[indexOfBiggest].score < decryptions.list[j].score)
                    {
                        indexOfBiggest = j;
                    }
                }

                tempObj = decryptions.list[i];
                decryptions.list[i] = decryptions.list[indexOfBiggest];
                decryptions.list[indexOfBiggest] = tempObj;
            }

            decryptions.scoreToBeat = decryptions.list[parameters.maxSurvivors - 1].score;

            break;
        }
    }

    return error;
}

// -------------------------------------------------------------------------------------------------

bool getNextPermutation(uint8_t* _ary, const uint8_t _size)
{
    // Validate input

    if ((NULL == _ary) || (0 == _size))
    {
        return false;
    }

    // Find longest non-increasing suffix

    uint8_t i = _size - 1;
    while ((i > 0) && (_ary[i - 1] >= _ary[i]))
    {
        --i;
    }

    // Now i is the head index of the suffix
    // Are we at the last permutation already?

    if (i <= 0)
    {
        return false;
    }

    // Let _ary[i - 1] be the pivot
    // Find rightmost element that exceeds the pivot

    uint8_t j = _size - 1;
    while (_ary[j] <= _ary[i - 1])
    {
        --j;
    }

    // Now the value _ary[j] will become the new pivot
    // Assertion: j >= i
    // Swap the pivot with j

    uint8_t temp = _ary[i - 1];
    _ary[i - 1] = _ary[j];
    _ary[j] = temp;

    // Reverse the suffix

    j = _size - 1;
    while (i < j)
    {
        temp = _ary[i];
        _ary[i] = _ary[j];
        _ary[j] = temp;
        ++i;
        --j;
    }

    return true;
}

// -------------------------------------------------------------------------------------------------

D424::error_t scoreDecipheredText(const char* _decipheredText,
                                  const uint32_t _size,
                                  uint32_t& _score)
{
    D424::error_t error = D424::ERROR_NONE;
    uint32_t offset = 0;
    uint8_t maxWordLength;
    uint8_t minWordLength;

    // Validate input

    if (NULL == _decipheredText)
    {
        error = D424::ERROR_PARAM_1;
        goto Decrypter424_scoreDecipheredText_End;
    }
    if (0 == _size)
    {
        error = D424::ERROR_PARAM_2;
        goto Decrypter424_scoreDecipheredText_End;
    }

    _score = 0;

    // While there are still characters to analyze ...

    while (_size > offset)
    {
        // Take into consideration how many letters are left in the text with comparison to
        // maximum and minimum word lengths

        maxWordLength = (_size - offset);
        if (maxWordLength > parameters.testMaxWordSize)
        {
            maxWordLength = parameters.testMaxWordSize;
        }

        minWordLength = parameters.testMinWordSize;
        if (minWordLength > maxWordLength)
        {
            minWordLength = maxWordLength;
        }

        // Working from longest possible word to shortest, determine if the word exists in the
        // dictionary. If it does, update the score and offset, and move on to the next iteration.

        for (uint8_t wordLength = maxWordLength; wordLength >= minWordLength; wordLength--)
        {
            if (-1 != language.WordLookupFuncPtr((_decipheredText + offset), wordLength))
            {
                // Our super-sophisticated scoring method ...

                _score += (uint32_t)pow(2, wordLength);
                offset += (wordLength - 1);
                break;
            }
        }

        // Increment the offset no-matter what.. taken into account when word is found b subtracting
        // 1 from the wordLength value. This helps us avoid another conditional.

        ++offset;
    }

    Decrypter424_scoreDecipheredText_End:
    return error;
}

// -------------------------------------------------------------------------------------------------

void D424::UserInterfaceGuiThread(void)
{
    const uint32_t THREAD_BUSY_TIMEOUT_MS = 1000;
    const uint32_t THREAD_IDLE_TIMEOUT_MS = 200;

    uint32_t jobAveragerCount = 0;
    uint32_t lastJobsDone = 0;
    uint32_t jobsRemaining;
    uint32_t timeLeftHours;
    uint32_t timeLeftMinutes;
    uint32_t timeLeftSeconds;
    float percentComplete;

    bool menuPrinted = false;
    status.refreshConsole = 1;

    while (!status.terminate)
    {
        if (status.refreshConsole)
        {
            GUI_CLEAR_SCREEN;
            menuPrinted = false;
            status.refreshConsole = 0;
        }

        if (!status.busy)
        {
            if (!menuPrinted)
            {
                printf("1. Initialize -- Not Implemented Yet\r\n");
                printf("2. Start columnar decipher\r\n");
                printf("3. Write results to file -- Not Implemented Yet\r\n");
                printf("\r\n");
                printf("Note: {s + enter = stop operations}\r\n");
                printf("Note: {q + enter = quit program}\r\n");
                printf("\r\n");
                printf("Choice: ");

                menuPrinted = true;
                fflush(stdout);
            }

            SLEEP_MS(THREAD_IDLE_TIMEOUT_MS);
        }
        else
        {
            if (0 == status.totalJobs)
            {
                continue;
            }

            percentComplete = ((float)status.jobsDone * 100) / (float)status.totalJobs;
            jobsRemaining = (uint32_t)(status.totalJobs - status.jobsDone);

            if (100.0f == percentComplete)
            {
                timeLeftSeconds = 0;
                timeLeftMinutes = 0;
                timeLeftHours = 0;
            }
            else if (status.jobsDone == lastJobsDone)
            {
                timeLeftSeconds = 59;
                timeLeftMinutes = 59;
                timeLeftHours = 999;
            }
            else
            {
                timeLeftSeconds = (uint32_t)(((double)jobsRemaining /
                                             ((double)status.jobsDone / jobAveragerCount)) *
                                             ((double)THREAD_BUSY_TIMEOUT_MS / 1000));
                timeLeftHours = (uint32_t)(timeLeftSeconds / 3600);
                timeLeftMinutes = (uint32_t)((timeLeftSeconds % 3600) / 60);
                timeLeftSeconds = (uint32_t)(timeLeftSeconds % 60);
            }

            printf("\r\nProgress: %u / %u   -- %3.02f%%   -- Time Left: %02u:%02u:%02u",
                (uint32_t)status.jobsDone, (uint32_t)status.totalJobs, percentComplete,
                timeLeftHours, timeLeftMinutes, timeLeftSeconds);

            printf("\r\nBest Guesses:\r\n");

            for (uint8_t i = 0; i < parameters.maxSurvivors; i++)
            {
                printf("%s\r\n", decryptions.list[i].text.c_str());
            }

            lastJobsDone = (uint32_t)status.jobsDone;
            ++jobAveragerCount;
            status.refreshConsole = 1;

            fflush(stdout);
            SLEEP_MS(THREAD_BUSY_TIMEOUT_MS);
        }
    }
}

// -------------------------------------------------------------------------------------------------

void D424::UserInterfaceInputThread(void)
{
    std::thread algorithm;
    int input = 0;

    while (!status.terminate)
    {
        // Wait/Block for character input.

        input = GET_CHAR();

        // Unconditional input

        switch (input)
        {
            // QUIT

            case 'Q':
            case 'q':
                D424::Terminate();
                if (algorithm.joinable())
                {
                    algorithm.join();
                }
                break;

            // STOP

            case 'S':
            case 's':
                D424::StopOperations();
                if (algorithm.joinable())
                {
                    algorithm.join();
                }
                status.stopOps = 0;
                status.refreshConsole = 1;
                break;
        }

        // Conditional input

        if (!status.busy)
        {
            switch (input)
            {
                case '1':
                    // TODO
                    break;

                // Star Columnar Decipher Algorithm

                case '2':
                    algorithm = std::thread(columnWithletterShiftAnalysis);
                    status.refreshConsole = 1;
                    break;

                case '3':
                    // TODO
                    break;
            }
        }


    }
}
