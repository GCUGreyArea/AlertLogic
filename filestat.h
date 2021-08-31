/**
 * @mainpage	filesatsta version 1.0
 * @section     intro_sec Introduction
 * 
 * 1. [application]@(ref application) Main application files
 * 2. [test](@ref test) Test files for GTest modules
 * 3. [library](@ref library) Library code for application 
 * 
 * @note  
 * 1. the XML translation doesn't take xml escape sequences into account, and will double double escapes them. This is not really bug as
 *   &amp; is the xml escaped character '&'. The word '&amp;' would need to be represented as '&amp;amp;' to be compatible with most xml parsers. It would 
 *   othersise be a representation, not of "&amp;" but of "&".
 *
 *   2. the match function was fixed to three possible states, which is not very configurable. A better aproach would is to use a table of boolean values to match 
 *   the input characters with an index of the charcter's value. this is what is done with the --scanconfig option. A text file with an alphabet is read in and the 
 *   characters are added to the match table. Each character is then compared to the table and dismised, or added to the word. Idealy the input file should be a 
 *   formated structure, but this is a minor problem. Note that while this functionality is fairly simple, it is also untested.
 *
 * building the project:
 *   1. It is recomended that the project is built and run in a wsl2 instalation or on a Linux machine.
 *   2. A script "./scripts/setup.sh" can be run to set up the dependancies.  This will install development libraries and compilers needed to build "filestat".
 *   3. To build the project run 'make' from the main directory. This will build the libraries, main application, and build, then run the test harness.
 */

#ifndef __FILESTAT_APP__
#define __FILESTAT_APP__

/**
 * @}
 */

#endif//__FILESTAT_APP__
