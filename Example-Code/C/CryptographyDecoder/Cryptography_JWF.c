/*Simple tools to analyze ciphertext and help decode it.

  Written By: Jared Fowler
  February 18, 2015

*/

#include "Cryptography_JWF.h"

/***********************************************************************
Outputs a table of options. Requests user selection. Calls appropriate
funciton. 
***********************************************************************/
int main(){

    //Local Variables:
    UINT choice    = 'z';
    BYTE returnVal = 0;
    BOOLEAN s2Done = FALSE;

    //Intro Title
    fprintf(stdout, "Cryptography Analyzer by Jared Fowler\n\n\n\n");
    
    //Present the user with option table
    do{
        fprintf(stdout, "1. External File Letter Count Analysis\n");
        fprintf(stdout, "2. Cipher Letter Analysis, Simple Shift, and Guess\n");
        fprintf(stdout, "3. Decypher Message (Simple Shift and Columnar Transposition)\n");

        fprintf(stdout, "\n9. Create Word Bank\n");
        fprintf(stdout, "0. Quit\n");

        fprintf(stdout, "\nChoice: ");
        fread(&choice, sizeof(char), 1, stdin);
        fflush(stdin);
        fprintf(stdout, "\n\n");

        switch ((char)choice){

        case '1':
            returnVal = fileCharCountAndRatioAnalysis();
            fprintf(stdout, "\nDONE.\n\n");
            break;

        case '2':
            returnVal = letterAnalysisAndSimpleShift();
            fprintf(stdout, "DONE.\n\n");
            s2Done = TRUE;
            break;

        case '3':
            //Was the basic information already analyzed?
            if (!s2Done)
                returnVal = letterAnalysisAndSimpleShift();

            //Determine based off best guess, specific offset, or all?
            fprintf(stdout, "Which Cipher?\n Enter 'b' for Best %u Guess(es), 'a' for all. : ", TOP_SHIFT_LIST_SIZE);
            fread(&choice, sizeof(char), 1, stdin);
            fflush(stdin);
            fprintf(stdout, "\n\n");

            //Call function with right arguments
            if ((char)choice == 'a' || (char)choice == 'A' || (char)choice == 'b' || (char)choice == 'B'){
                choice = toupper((char)choice);
                returnVal = (returnVal | permutateAndFilterate((BYTE)choice, SURVIVAL_SCORE_MIN));
            }
            else{
                fprintf(stderr, "Invalid Input...");
                break;
            }
                
            //Print results
            printOutFinalResults();

            fprintf(stdout, "\nDONE.\n\n");
            

            break;

        case '9':
            returnVal = createWordBank();
            fprintf(stdout, "\nDONE.\n\n");
            break;

        case '0':
            fprintf(stderr, "Good-Bye");
            break;

        default:
            fprintf(stderr, "Invalid Selection...");
            break;

        }

        //Error Messages?
        if (returnVal != 0)
            fprintf(stderr, "Error Code: %d", returnVal);
        else
            returnVal = 0;


    } while (choice != '0');

    fprintf(stderr, "\n\n");

    return 0;
}