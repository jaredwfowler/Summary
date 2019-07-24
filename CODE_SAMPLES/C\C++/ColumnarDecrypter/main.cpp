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

#include "Decrypter424.h"
#include <stdio.h>
#include <thread>

int main()
{
    D424::parameters_t params;
    D424::error_t error;

    params.cypher = "KUHPVIBQKVOSHWHXBPOFUXHRPVLLDDWVOSKWPREDDVVIDWQRBHBGLLBBPKQUNRVOHQEIRLWOKKRDD";
    params.minColumns = 1;
    params.maxColumns = 10;
    params.maxSurvivors = 5;
    params.testMinWordSize = 3;
    params.testMaxWordSize = 8;

    error = D424::Initialize(&params);

    if (D424::ERROR_NONE == error)
    {
        // Begin interface threads

        std::thread userInterfaceGui(D424::UserInterfaceGuiThread);
        std::thread userInterfaceInput(D424::UserInterfaceInputThread);

        // Wait for interface threads to finish

        userInterfaceGui.join();
        userInterfaceInput.join();
    }
    else
    {
        printf("\r\nError: %s\r\n", D424::ERROR_MSG[error]);
    }

    // Clean up

    D424::Destruct();

    return 0;
}
