
                _._     _,-'""`-._
              (,-.`._,'(       |\`-/|
                  `-.-' \ )-`( , o o)
                        `-    \`_`"'-       LaserChaser.org




This is a spin-off of a project done at CSU Northridge 400-
level class based on system security. The task at hand is 
to decipher a given cypher text which has been encrypted with
both letter shift and columnar transposition. 

The original project done back in 2015 took around 30 minutes
to run completely. (Max 10 columns with cipher ~80 letters.)
The final product of this spin-off takes around 2 minutes to
decipher the same. :-) 

The biggest improvement comes from word lookup. Various tricks
were done in the original version, such as ordering the 
dictionary in order of word length backwards. There would be
a single linear pass through the dictionary, comparing it with
the cypher. The new technique uses a binary lookup. This 
requires multiple dicitonary traversals for each possible word
length, however, even with the multiple traversals it still
reduces the ammount of comparisons done by a factor of 7x.

This code is provided as-is. May be used if inline credit 
given to me, Jared Fowler.


Goals:

1. Make compatible for different languages - Easy plug-in
2. Make compatible for both linux and windows
3. Clean-up. Modern formatting.
4. Make faster ...
5. While working, have "best guess so far print out"

Results:

1. Languages are handled by variable and function pointers,
   so easy import is possible.
2. Compatible with both Windows and linux
3. Looks great!
4. What used to take ~30 minutes took only 10 minutes with 
   version 0.2. Further reduced down to 2 minutes with 
   version 0.3. 
5. This significantly speeds up the program. If observed, the
   program can be stopped early. The original test text string
   is deciphered within the first 10 seconds.
