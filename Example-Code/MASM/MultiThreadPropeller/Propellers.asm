;******************************************************************
;*  Jared Fowler
;*  CS M30 Fall 2013
;*  Project 5
;*
;*  Propellers   V_1.00
;*
;*  Purpose - The main objective of this program is to learn how to 
;*  create a multi-threaded program.  There will be a total of three (3)
;*  threads.  The first and main thread will control the speed of the two
;*  (2) propellers. The second thread will be in charge of displaying the
;*  first propeller, and the final thread will be in charge of displaying
;*  the second propeller.  
;*
;*  README:
;*
;*    There will be six different speeds for the propellers.  These speeds 
;*  are dependent on how much time in miliseconds a propeller will rest
;*  before changing and being redrawn. These rest times will be: 
;*  inifinte ("OFF"), 500ms, 250ms, 125ms, and 62 ms.  
;*    The speed of propeller one will be increased by pressing 1, and 
;*  decreased by pressing 2.  Similiarly, propeller 2 will use the keys
;*  3 and 4 for the same function.  
;*    Pressing the 9 key will end the program.  
;*    The procs in this program preform different operations but are very 
;*  tightly coupled.  Any change to one proc could result in the need to 
;*  change many.
;*
;******************************************************************

        .586
        .MODEL flat, stdcall

        include Win32API.asm

        .STACK 4096
        

;-------------------------------------------------------------------------------------------
;DATA
;-------------------------------------------------------------------------------------------

                    .DATA


; EQUATES ----------------------------------------------------------------------------------
SPEED0              EQU         0       ;suspend the thread
SPEED1              EQU         1000    ;1000 milisecond delay
SPEED2              EQU         500     ;500  milisecond delay
SPEED3              EQU         250     ;250  milisecond delay
SPEED4              EQU         125     ;125  milisecond delay
SPEED5              EQU         62      ;62   milisecond delay

MAXSPEEDLVL         EQU         5       ;highest speed level
MINSPEEDLVL         EQU         0       ;lowest speed level

MAXSPINPHASE        EQU         3       ;highest spin phase
MINSPINPHASE        EQU         0       ;lowest spin phase

INCREASEP1          EQU         '1'     ;ASCII 1 increases P1 speed
DECREASEP1          EQU         '2'     ;ASCII 2 decreased P1 speed
INCREASEP2          EQU         '3'     ;ASCII 3 increases P2 speed
DECREASEP2          EQU         '4'     ;ASCII 4 decreases P2 speed

ESCAPE              EQU         '9'     ;Exits the loop and ends program

SPINPHASE0          EQU         '|'     ;Phases of the propeller rotation
SPINPHASE1          EQU         '/'     
SPINPHASE2          EQU         '-'     
SPINPHASE3          EQU         '\'

P1SpeedOffset       EQU         7       ;Position of P1 lvl start in byte array of SpeedBar
P2SpeedOffset       EQU         27      ;Position of P2 lvl start in byte array of SpeedBar

CR                  EQU         0Dh     ;carriage return
LF                  EQU         0Ah     ;New Line 

;VARIABLES ---------------------------------------------------------------------------------
SpeedArray          dword       SPEED0, SPEED1, SPEED2, SPEED3, SPEED4, SPEED5
SpinArray           byte        SPINPHASE0, SPINPHASE1, SPINPHASE2, SPINPHASE3

Propeller1SpeedLVL  byte        MINSPEEDLVL     ;Propeller 1 starts as suspended
Propeller2SPeedLVL  byte        MINSPEEDLVL     ;Propeller 2 starts as suspended

P1SPeedChange       byte        0               ;boolean value determines if we need to update speedbar
P2SpeedChange       byte        0

Propeller1Speed     dword       SPEED0          ;Sleep time for propeller 1
Propeller2Speed     dword       SPEED0          ;Sleep time for propeller 2

Spin1               byte        SPINPHASE0      ;Both propellers start at phase 0
Spin2               byte        SPINPHASE0  

Thread1_ID          dword       ?
Thread2_ID          dword       ?
Thread3_ID          dword       ?

Thread1             dword       ?       ;Handle for first thread
Thread2             dword       ?       ;Handle for seconde thread
Thread3             dword       ?       ;Handle for third thread
MutexHandle         dword       ?       ;Handle for the mutex

hStdOut             dword       0       ;Handle to allow us to write to screen
hStdIn              dword       0       ;Handle to allow us to read from keyboard

UserInput           byte        0       ;Will hold user input from keyboard
CharsRead           dword       0       ;number of bytes written by user


Airplane            byte        LF, CR, 
                                "                \  MM  /                ", LF,CR,
                                "                \\MMMM//                ", LF,CR,
                                "                 \\||//                 ", LF,CR,
                                "                 / || \                 ", LF,CR,
                                "                 |/  \|                 ", LF,CR,
                                "                 |\  /|                 ", LF,CR,
                                "  |\             | \/ |             /|  ", LF,CR,

AirPlane2           byte        "  | ^^^^^^^^^^^^^      ^^^^^^^^^^^^^ |  ", LF,CR,
                                "   \        J-SOFT JWF 2013         /   ", LF,CR,
                                "     \___________|    |___________/     ", LF,CR,
                                "          ^      |    |      ^          ", LF,CR

AirPlane3           byte        "                 |    |                 ", LF,CR,
                                "                 |\  /|                 ", LF,CR,
                                "                  \\//                  ", LF,CR,
                                "                  \\//                  ", LF,CR,
                                "                   \/                   ", LF,CR,
                                "                   ||                   ", LF,CR  

Instruct            byte        "1 - Increase Speed Propeller 1", LF, CR,
                                "2 - Decrease Speed Propeller 1", LF, CR,
                                "3 - Increase Speed Propeller 2", LF, CR,
                                "4 - Decrease Speed Propeller 2", LF, CR,
                                "9 - Exit Program", LF, CR

SpeedBar            byte        "   OFF |..... MAX      OFF |..... MAX", LF, CR
SpeedBarOrig        byte        "   OFF ...... MAX      OFF ...... MAX", LF, CR

SpeedBarCoord       LABEL   DWORD
CursorXPosition     word    0
CursorYPosition     word    25

Propeller1Coord     LABEL   DWORD
CursorXPosition1    word    10
CursorYPosition1    word    17

Propeller2Coord     LABEL   DWORD
CursorXPosition2    word    29
CursorYPosition2    word    17

EndCoord            LABEL   DWORD
CursorXPosition3    word    0
CursorYPosition3    word    27

;-------------------------------------------------------------------------------------------
;CODE
;-------------------------------------------------------------------------------------------

                    .CODE


;**************************************************************************************************
;Main Procedure
;
;Gets the handles for the input and output.  Creates a mutex and stores the handle.  Creates
;handles for three threads.  Draws the airplane image to the console.  Starts the first thread.  
;This proc, being itself the main program, and a thread of its own, uses the WaitForSingleObject
;while it prints out the updated speed bars.  When the proram ends, the mutex is closed.
;
;**************************************************************************************************
Main                Proc


                    ;*********************************************
                    ;In order to write to windows console screen
                    ;we need to get a handle to the screen
                    ;*********************************************
                    invoke      GetStdHandle, STD_OUTPUT_HANDLE
                    mov         hStdOut,eax	                        ;Save output handle

                    ;*********************************************
                    ;Get a handle to the standard input device
                    ;*********************************************
                    invoke      GetStdHandle, STD_INPUT_HANDLE
                    mov         hStdIn,eax                          ;Save input handle

                    ;*********************************************
                    ;Set the console mode for input 
                    ;(don't show input on screen)
                    ;*********************************************
                    invoke      SetConsoleMode, hStdIn, ENABLE_NOTHING_INPUT

                    ;*********************************************
                    ;Create a mutex and store its handle in MutexHandle
                    ;*********************************************
                    invoke      CreateMutexA, NULL,0,NULL
                    mov         MutexHandle,eax                     ;Save Mutex handle

                    ;*********************************************
                    ;Get handles for the three threads
                    ;NOTE: The number of threads a process
                    ;  can create is limited by the available virtual
                    ;  memory. By default, every thread has one megabyte
                    ;  of stack space.  Also, the treads will be created
                    ;  in a suspended mode.  This is done by putting 
                    ;  the fifth parameter as 4 hex.
                    ;*********************************************
                    mov         eax, OFFSET Thread1Proc
                    invoke      CreateThread,NULL,NULL,eax,0,4,ADDR Thread1_ID
                    mov         Thread1,eax                         ;Save Thread1 handle

                    mov         eax, OFFSET Thread2Proc
                    invoke      CreateThread,NULL,NULL,eax,0,4,ADDR Thread2_ID
                    mov         Thread2,eax                         ;Save Thread2 handle

                    mov         eax, OFFSET Thread3Proc
                    invoke      CreateThread,NULL,NULL,eax,0,4,ADDR Thread3_ID
                    mov         Thread3,eax                         ;Save Thread3 handle


                    ;*********************************************
                    ;Draw the initial objects to the console
                    ;*********************************************
                    invoke  WriteConsoleA, hStdOut, OFFSET Instruct, SIZEOF Instruct, NULL, 0
                    invoke  WriteConsoleA, hStdOut, OFFSET Airplane, SIZEOF Airplane, NULL, 0
                    invoke  WriteConsoleA, hStdOut, OFFSET Airplane2, SIZEOF Airplane2, NULL, 0
                    invoke  WriteConsoleA, hStdOut, OFFSET Airplane3, SIZEOF Airplane3, NULL, 0
                    
                    ;*********************************************
                    ;Resume Thread1.  The other threads will be
                    ;resumed from thread1 proc
                    ;*********************************************
                    invoke      ResumeThread, Thread1

            
MainLoop:           
                    ;*********************************************
                    ;Have the other threads wait upon this one
                    ;Once the printing is done, release the mutex
                    ;*********************************************
                    invoke      WaitForSingleObject, MutexHandle, 0FFFFFFFFh
                    call        PaintSpeedBar                       ;update what is shown on the console
                    invoke      ReleaseMutex, MutexHandle

                    ;*********************************************
                    ;Has the escape key been pressed?
                    ;*********************************************
                    mov         al,UserInput                        ;get the last user input
                    cmp         al,ESCAPE                           ;was the escape key pressed?
                    jne         MainLoop                            ;if yes, continue, else, loop

                    ;*********************************************
                    ;Close the mutex handle and set the console
                    ;cursor out of the way.
                    ;*********************************************
                    invoke      CloseHandle, MutexHandle			;Terminate the mutex handle
                    invoke      SetConsoleCursorPosition, hStdOut, EndCoord

                    ;*************************
                    ;Terminate Program
                    ;*************************
                    invoke  ExitProcess,0

Main                ENDP


;**************************************************************************************************
;Thread1 Procedure
;
;This thread constantly waits upon user input. Upon recieving input, it gets the mutex. It then might 
;increase the velocity of a propeller, or if no valid char is entered, it will simply loop back to the start.  
;If a command is given to change a propeller speed, this value is first error tested, and then 
;updated.  Other values are written, such as P1SpeedChange which will take effect in different 
;procs. 
;
;**************************************************************************************************
Thread1Proc         proc

CONTINUE:

                    ;*********************************************
                    ;Start by getting user input.  Once there 
                    ;is input, get the mutex!  Depending on
                    ;what is input, either a propeller speed will
                    ;change,else upon completion of the proc the 
                    ;program may exit.  
                    ;*********************************************
                    invoke ReadConsoleA, hStdIn, OFFSET UserInput, 1,OFFSET CharsRead, 0

                    ;*********************************************
                    ;Once we recieve user input, have the other threads
                    ;wait for this one to finish 
                    ;*********************************************
                    invoke  WaitForSingleObject, MutexHandle, 0FFFFFFFFh
                    

                    cmp     UserInput,INCREASEP1        ;User wants to increase speed of P1?
                    jne     T1Next0                     ;if not, jump to next test
                    cmp     Propeller1SpeedLVL,MAXSPEEDLVL  ;is the propeller already at max speed?
                    je      T1Final                     ;if so, restart at the top of Thread1Proc
                    inc     Propeller1SpeedLVL          ;else, increment speed to next level
                    cmp     Propeller1SpeedLVL,1        ;did we just come out of suspension?
                    jne     UpdateP1                    ;if not, continue to UpdateP1
                    invoke  ResumeThread,Thread2        ;else, resume the thread
                    jmp     UpdateP1
T1Next0:
                    cmp     UserInput,DECREASEP1        ;User wants to decrease speed of P1?
                    jne     T1Next1                     ;if not, jump to next test
                    cmp     Propeller1SpeedLVL,MINSPEEDLVL  ;is the propeller already at min speed?
                    je      T1Final                     ;if so, restart at the top of Thread1Proc
                    dec     Propeller1SpeedLVL          ;else, decrement speed to previous level
                    cmp     Propeller1SpeedLVL,0        ;did we just shut off the propeller?
                    jne     UpdateP1                    ;if not, continue to UpdateP1
                    invoke  SuspendThread,Thread2       ;else, suspend the thread
                    jmp     UpdateP1
T1Next1:
                    cmp     UserInput,INCREASEP2        ;User wants to increase speed of P2?
                    jne     T1Next2                     ;if not, jump to next test
                    cmp     Propeller2SpeedLVL,MAXSPEEDLVL  ;is the propeller already at max speed?
                    je      T1Final                     ;if so, restart at top of Thread1Proc
                    inc     Propeller2SPeedLVL          ;else, increment speed to next level
                    cmp     Propeller2SpeedLVL,1        ;did we just come out of suspension?
                    jne     UpdateP2                    ;if not, continue to UpdateP2
                    invoke  ResumeThread,Thread3        ;else, resume the thread
                    jmp     UpdateP2
T1Next2:
                    cmp     UserInput,DECREASEP2        ;User wants to decrease speed of P2?
                    jne     T1Final                     ;if not, jump to next test
                    cmp     Propeller2SpeedLVL,MINSPEEDLVL  ;is the propeller already at min speed?
                    je      T1Final                     ;if so, restart at top of Thread1Proc
                    dec     Propeller2SPeedLVL          ;else, decrement speed to next level
                    cmp     Propeller2SpeedLVL,0        ;did we just shut off the propeller?
                    jne     UpdateP2                    ;if not, continue to UpdateP2
                    invoke  SuspendThread,Thread3       ;else, suspend the thread
                    jmp     UpdateP2

                    ;*********************************************
                    ;If we are to this point, it is b/c we changed
                    ;a propeller speed lvl, and now we will update
                    ;its actual speed (aka, how long the thread
                    ;should rest) 
                    ;*********************************************
UpdateP1:
                    mov     P1SpeedChange,1             ;update boolean expression
                    movzx   eax,Propeller1SpeedLVL      ;get current speed level
                    mov     ebx, OFFSET SpeedArray      ;ebx holds array of sleep values
                    mov     eax,[ebx][eax * 4]          ;eax holds the sleep time
                    mov     Propeller1Speed,eax         ;update the sleep time for P1
                    jmp     T1Final
UpdateP2:
                    mov     P2SpeedChange,1             ;update boolean expression
                    movzx   eax,Propeller2SpeedLVL      ;get current speed level
                    mov     ebx, OFFSET SpeedArray      ;ebx holds array of sleep values
                    mov     eax,[ebx][eax * 4]          ;eax holds the sleep time
                    mov     Propeller2Speed,eax         ;update the sleep time for P2
                    jmp     T1Final


T1Final:    
                    invoke	ReleaseMutex, MutexHandle

                    jmp      CONTINUE                   ;loop back to start of Thread1Proc
                    
                    ret

Thread1Proc         ENDP

;**************************************************************************************************
;Thread2 Procedure
;
;Determines the next propeller value to be written.  Given a coord, it prints the propeller
;
;**************************************************************************************************
Thread2Proc         proc

                    LOCAL PhaseCount:byte               ;keeps track of what phase the propeller is on
                    
                    mov PhaseCount,MINSPINPHASE         ;we start off with zero turns of the propeller

                    ;*********************************************
                    ;Update the spin
                    ;position, that is, what ascii value the propeller
                    ;represents.  Update the loop for next time around,
                    ;and then sleep.
                    ;*********************************************
CONTINUE:
                    ;*********************************************
                    ;Have the other threads wait for this one to finish
                    ;*********************************************
                    invoke	WaitForSingleObject, MutexHandle, 0FFFFFFFFh


                    mov     ebx, OFFSET SpinArray       ;ebx holds the array of possible spins
                    movzx   eax, PhaseCount             ;eax holds the offset for SpinArray
                    mov     eax, [ebx][eax]             ;put into eax the proper spin value
                    mov     Spin1,al                    ;update the variable that keeps track of this

                    ;*********************************************
                    ;Given specific coordinants, print the propeller
                    ;*********************************************
                    invoke SetConsoleCursorPosition, hStdOut, Propeller1Coord
                    invoke WriteConsoleA, hStdOut, OFFSET Spin1, SIZEOF Spin1, NULL, 0

                    ;*********************************************
                    ;Update the propeller for next time
                    ;*********************************************
                    cmp     PhaseCount,MAXSPINPHASE     ;are we at the last phase?
                    jne     T2Next0                     ;if not, continue
                    mov     PhaseCount,MINSPINPHASE     ;else, restart the propeller cycle
                    jmp     T2Next1
T2Next0:
                    inc     PhaseCount                  ;update the phasecount
T2Next1:
                    invoke  ReleaseMutex,MutexHandle    ;Release the mutex
                    invoke  Sleep,Propeller1Speed       ;sleep for specified time
                    
                    jmp CONTINUE                        ;loop back to start of Thread2Proc

                    ret

Thread2Proc         ENDP

;**************************************************************************************************
;Thread3 Procedure
;
;Determines the next propeller value to be written.  Given a coord, it prints the propeller
;
;**************************************************************************************************
Thread3Proc         proc

                    LOCAL PhaseCount:byte               ;keeps track of what phase the propeller is on
                    
                    mov PhaseCount,MINSPINPHASE         ;we start off with zero turns of the propeller

                    ;*********************************************
                    ;Update the spin
                    ;position, that is, what ascii value the propeller
                    ;represents.  Update the loop for next time around,
                    ;and then sleep.
                    ;*********************************************
CONTINUE:
                    ;*********************************************
                    ;Have the other threads wait for this one to finish
                    ;*********************************************
                    invoke	WaitForSingleObject, MutexHandle, 0FFFFFFFFh


                    mov     ebx, OFFSET SpinArray       ;eax holds the array of possible spins
                    movzx   eax, PhaseCount             ;eax holds the offset for SpinArray
                    mov     eax, [ebx][eax]             ;put into eax the proper spin value
                    mov     Spin2,al                    ;update the variable that keeps track of this


                    ;*********************************************
                    ;Given specific coordinants, print the propeller
                    ;*********************************************
                    invoke SetConsoleCursorPosition, hStdOut, Propeller2Coord
                    invoke WriteConsoleA, hStdOut, OFFSET Spin2, SIZEOF Spin2, NULL, 0


                    ;*********************************************
                    ;Update the propeller for next time
                    ;*********************************************
                    cmp     PhaseCount,MAXSPINPHASE     ;are we at the last phase?
                    jne     T3Next0                     ;if not, continue
                    mov     PhaseCount,MINSPINPHASE     ;else, restart the propeller cycle
                    jmp     T3Next1
T3Next0:
                    inc     PhaseCount                  ;update the phase for next loop around
T3Next1:
                    invoke  ReleaseMutex,MutexHandle    ;Release the mutex
                    invoke Sleep,Propeller2Speed        ;sleep for specified time


                    jmp CONTINUE                        ;loop back to start of Thread2Proc

                    ret

Thread3Proc         ENDP

;**************************************************************************************************
;DrawPlaneAndSpeed Procedure
;
;Draws the byte arrays as shown in the data section.  Checks if the speed bars need to be updated,
;and if so, updates them and then prints them.
;
;**************************************************************************************************
PaintSpeedBar		proc
                    
                    push    eax                         ;preserve eax
                    push    ebx                         ;preserve ebx
                    push    ecx                         ;preserve ecx
                    push    edx                         ;preserve edx

                    ;*********************************************
                    ;Determine if we need to update the speed bar.
                    ;If not, jump to DrawFinal, else, clear the bars
                    ;in prep for new values
                    ;*********************************************
                    mov     al,P1SpeedChange            ;Boolean value into al
                    cmp     al,P2SpeedChange            ;Is there a need for an update?
                    je      DrawFinal                   ;If not, jump to end of proc

                    mov     eax,OFFSET SpeedBar         ;SpeedBar is a byte array which shows the speed
                    mov     ebx,OFFSET SpeedBarOrig     ;This is the template for speedbar
                    mov     ecx,SIZEOF SpeedBarOrig     ;get the number of bytes to write
ResetArray:
                    mov     dl,byte ptr [ebx][ecx - 1]	;get element from original array
                    mov     [eax][ecx - 1],dl           ;put element into drawn array

                    loopd   ResetArray                  ;loop back until all array is copied over

                    ;*********************************************
                    ;Write the new values in their appropriate places 
                    ;*********************************************
                    movzx   ebx,Propeller1SpeedLVL                      ;what is P1 speed level?
                    mov     byte ptr [eax][ebx + P1SpeedOffset], "|"    ;write the char
                    movzx   ebx,Propeller2SpeedLVL                      ;what is P2 speed level?
                    mov     byte ptr [eax][ebx + P2SpeedOffset], "|"    ;write the char

                    ;*********************************************
                    ;Set the boolean Speed Change values to false
                    ;*********************************************
                    mov     P1SpeedChange,0
                    mov     P2SpeedChange,0

                    ;*********************************************
                    ;Draw the speed bars 
                    ;*********************************************
DrawFinal:
                    invoke SetConsoleCursorPosition, hStdOut, SpeedBarCoord
                    invoke WriteConsoleA, hStdOut, OFFSET SpeedBar, SIZEOF SpeedBar, NULL, 0
                    
                    pop     edx
                    pop     ecx
                    pop     ebx
                    pop     eax

                    ret

PaintSpeedBar       ENDP


                    END     Main