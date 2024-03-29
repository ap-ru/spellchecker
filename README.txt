ap2040 - Anish Patel / hm569 - Haris Malik 

Systems Programming Project 2: Spelling Checker

This program reads a dictionary file and checks the words in one or more text files to see if any are not listed in the dictionary.
 
To run the spchk:

- The first argument must be to a dictionary file
- Subsequent arguments must be paths to text files or directories.



The following headers are needed in order for these functions to run,  and should be included at the top each code file:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/stat.h>


Error Handling:

When spchk finds a mistake, it will report the word, the file containing the word, and the word's line and column number.

Example output:

test5 (1,6): danxing

If a file cannot be opened, spchk will print an error message:
"file information could not be retrieved"


Test Plan:

Compile test programs using make test 

Run them using: 
./spchk../dict../testfile


Test 1:

Tests to see that spchk runs with no errors when checking a txt file with no spelling mistakes.

Test 2:

Tests to see that spchk runs with no errors when checking a txt file with capitalization errors. 


Test 3: 

Tests to see that spchk runs with no errors when checking a txt file that includes trailing punctuation.

Test 4: 

Tests to see that spchk runs with no errors when checking a txt file that includes hyphenated words.

Tests 5:

Tests to see that spchk runs with no errors when checking a txt file that includes multiple misspelled words.

Test 6:

Tests to see that spchk runs with no errors when checking a txt file longer than the previous test, with multiple misspelled words. 