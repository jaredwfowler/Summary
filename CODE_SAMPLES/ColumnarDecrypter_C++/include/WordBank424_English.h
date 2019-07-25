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

#ifndef WORD_BANK_424_ENGLISH_H
#define WORD_BANK_424_ENGLISH_H

#include <stdint.h>

// -------------------------------------------------------------------------------------------------

// Declare various constants and structures within a namespace as to avoid possible collisions
// with other libraries.

namespace WordBank424_English
{

/*
 * information_map_t - Information regarding a character.
 *
 *      value               Numerical representation of the value.
 *      letter              Character representation
 *      letterAlt           Alternate character representation. (Example: 'a', 'A')
 *      frequency           Frequency which the letter occurs in the dictionary on a scale of
 *                          0-100 compared with the other letters.
 */

struct information_map_t
{
    uint16_t value;
    char letter;
    char letterAlt;
    float frequency;
};

/*
 * Total number of letters in the alphabet
 */

extern const uint16_t LETTERS_IN_ALPHABET;

/*
 * Total number of words in the dictionary.
 */

extern const uint32_t WORDS_IN_DICTIONARY;

/*
 * WordLookup - Uses a binary search to find the input word. If the word is found, its index is
 * returned, else, -1 is returned.
 *
 *      _str                Word to look for in the dictionary
 *      _size               Size of _str. (Letters only, do not count NULL terminator)
 */

int64_t WordLookup(const char* _str, const uint8_t _size);

/*
 * FrequencyLookup - Returns the frequency of a letter. If the letter does not exist, returns 0.
 *
 *      _letter            The letter
 */

float FrequencyLookup(const char _letter);

/*
 * GetLetterOffset - Returns the integer value of a letter according to the map. If not found,
 * returns -1.
 *
 *      _letter            The letter
 */

int32_t GetLetterOffset(const char _letter);

/*
 * GetOffsetLetter - Returns the character value of an integer according to the map. If not found,
 * returns NULL (0).
 *
 *      _letter            The letter
 */

char GetOffsetLetter(const uint16_t _offset);

}  // WordBank424_English
#endif  // WORD_BANK_424_ENGLISH_H
