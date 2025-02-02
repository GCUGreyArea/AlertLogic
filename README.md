
### NOTES:
1. the XML translation doesn't take xml escape sequences into account, and will double double escapes them. This is not really bug as
&amp; is the xml escaped character '&'. The word '&amp;' would need to be represented as '&amp;amp;' to be compatible with most xml parsers. It would 
othersise be a representation, not of "&amp;" but of "&".
2. the match function was fixed to three possible states, which is not very configurable. A better aproach might be a table of boolean values to match 
the input characters with an index of the charcter's value. This is what is done with the --scanconfig option. A text file with an alphabet is read in and the 
characters are added to the match table. Each character is then compared to the table and dismised, or added to the word. Idealy the input file should be a 
formated structure, but this is a minor problem. Note that while this functionality is fairly simple, it is also untested.
3. The filescanner deals with ASCII codes only.

### BUILDING THE PROJECT:
1. It is recomended that the project is built and run in a wsl2 instalation or on a Linux machine.
2. A script "./scripts/setup.sh" can be run to set up the dependancies.  This will install development libraries and compilers needed to build "filestat".
3. To build the project run 'make' from the main directory. This will build the libraries, main application, and build, then run the test harness.

### Known problems 
1. Currentlt the program is based on a very simple stateless parser that would need to read in a file on one chunk to guarentee that all symbols where parsed correctly. If it is interrupted while parsing a word which lies between two chunck of the file, say "swindon", what you might get are the words "swin" and "don" (depending on where the break ocured). To rectify this we need to save the state of the current parsed word and give the parser a notion of streem (file) size in terms of number of bufferes it will need to parse.

### LINKS:
1. https://www.ascii-code.com/

