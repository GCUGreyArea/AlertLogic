/**
 * @file filestat.c
 * @author Barry Robinson (barry.w.robinson64@gmail.com)
 * @brief Program to parse a text file and return stats for characters
 * @version 0.1
 * @date 2021-08-28
 *
 * @copyright Copyright (c) Alert Logic 2021
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>


#include <libfilestat.h>

#include "filestat.h"

/**
 * @brief 
 * @addtogroup aplication
 */
inline static void printHelp() {
	printf("Usage : filestat <--infile inName> [--outfile stdout --tablesize 4096 --infilebuff size --matchfunc name --wordstats --highfreqwordstats --charstats --scanconfig file --help]\n");
	printf("\t--infile: <the name of the text file to parse>\n");
	printf("\t--outfile: [the name of the file to write write to, or stdout] ");
	printf("Defaults to stdout if no value is supplied\n");
	printf("\t--tableSize: [size of hash table for word analysis] ");
	printf("Defaults to 4096\n");
	printf("\t--infilebuff: [Size of the buffer for file reads] defauts to FILE_READ_MAX\n");
	printf("\t--wordstats: [print out all collected words with freequency] defaults to off\n");
	printf("\t--highfreqwordstats: [printout high freequency words and stats] defaults to off\n");
	printf("\t--charstats: [print out stats on all characters scanned] defaults to off\n");
	printf("\t--matchfunc: name [isWordChar, isValidChar, or isPrintableCharacter]\n");
	printf("\t\tisWordChar matches [A-Za-z] and nothing else\n");
	printf("\t\tisValidChar macthes [!-~] including all valid word characters [A-Za-z] and all digits [0-9]\n");
	printf("\t\tisPrintableCharacter matches any printable character\n");
	printf("\t\t--scanconfig: A file text with ASCII characters to use word matching\n");
	printf("\t--help: Print this message\n");
	printf("NOTE 1: Currently the program will not scan in chnucks\n");
	printf("NOTE 2: If --scanconfig is used --matchfunc will be ignored\n");
}

int main(int argc, const char ** argv) {
	char * inputFile = NULL;
	char * outFileName = "stdout";
	size_t tableSize = 4096;
	size_t buffSize = FILE_READ_MAX;
	bool wordStats = false;
	bool charStats = false;
	bool highestFreeWordStats = false;
	char * matchFunc = "isWordChar";
	const char * scanConfig = NULL;

	int i=1;
	while(i<argc) {
		if(strcmp(argv[i], "--help")==0) {
			printHelp();
			exit(0);
		}

		if(strcmp(argv[i], "--charstats")==0) {
			charStats=true;
		}

		if(strcmp(argv[i], "--wordstats")==0) {
			wordStats=true;
		}

		if(strcmp(argv[i], "--highfreqwordstats")==0) {
			highestFreeWordStats=true;
		}

		if(strcmp(argv[i],"--infile") == 0) {
			i++;
			if(i>argc) break;
			inputFile = (char*) argv[i];
		}

		if(strcmp(argv[i], "--outfile") == 0) {
			i++;
			if(i>argc) break;
			outFileName = (char*) argv[i];
		}

		if(strcmp(argv[i], "--tableSize") == 0) {
			i++;
			if(i>argc) break;
			tableSize = atoi(argv[i]);
		}

		if(strcmp(argv[i], "--infilebuff") == 0) {
			i++;
			if(i>argc) break;
			buffSize = atoi(argv[i]);
		}

		if(strcmp(argv[i],"--matchfunc") == 0) {
			i++;
			if(i>argc) break;
			matchFunc = (char*) argv[i];
		}

		if(strcmp(argv[i],"--scanconfig") == 0) {
			i++;
			if(i>argc) break;
			scanConfig = argv[i];
		}

		i++;
	}

	if(inputFile == NULL) {
		printf("An input file must be specified on the command line\n");
		printHelp();
		exit(0);
	}

	FILE * outFile = stdout;
	if(strcmp(outFileName,"stdout") != 0) {
		outFile = fopen(outFileName, "w+");
		if(outFile == NULL) {
			fprintf(stderr,"ERROR: Failed to open file [path %s]\n", outFileName);
			exit(1);

		}
	}

	// Open output file
	if(access( inputFile, F_OK ) != 0) {
		fprintf(stderr,"ERROR: Failed to access input file [path %s]\n", inputFile);
		exit(1);
	}


	tFileDetails * inFilePt = openFile(inputFile, buffSize);
	if(inFilePt == NULL) {
		fprintf(stderr,"ERROR: Failed to open input file [path %s]\n",inputFile);
		exit(1);
	}

	fileStats_t * stats = buffScan_init(tableSize);
	if(stats == NULL) {
		fprintf(stderr,"ERROR: Failed to create scanner. Return was NULL\n");
		exit(1);
	}

	if(scanConfig) {
		FILE * f = fopen(scanConfig,"r");
		if(f == NULL) {
			fprintf(stderr,"ERORR: Coud not open %s\n", scanConfig);
			exit(1);
		}

		// Get the size of the file now incase wee need to allocate the total file size to the buufer
		fseek(f, 0, SEEK_END);
		size_t size = ftell(f);
		// Reset the file handle to point to the start of the file
		fseek(f, 0, SEEK_SET);

		char * b = alloc_mem(size);
		if(b == NULL) {
			fprintf(stderr,"ERROR: Memory allocation failure, exiting..\n");
			exit(1);
		} 

		// Read in the file and add all characters to the table
		int act = fread(b, size, 1, f);
		if(act == 1) {
			for(size_t i=0;i<size;i++) {
				buffScan_addCharToMatchFunc(b[i]);
			}


		}

		buffScan_setWordMatch(stats,"isTableMatch");

		free(b);
		fclose(f);
	}
	else {
		buffScan_setWordMatch(stats,matchFunc);
	}

	// Set up the scanner for output
    buffScan_setCharFreqStats(stats,charStats);
    buffScan_setWordStats(stats,wordStats);
	buffScan_setHighestFreqWordsStats(stats,highestFreeWordStats);
    buffScan_setOutputFile(stats,outFile);
	

	// Get the read size and start reading
	size_t bufferLen = getBuffReadSize(inFilePt);
	const unsigned char * buff = getNexFileBuffer(inFilePt);
	// printf("In file %s\n", (char*) buff);
	buffScan_scan(stats,buff,bufferLen);


	// Output to the file
    printFrequencyAnalysis(stats);

	fclose(outFile);

	return 0;
}

/**
 * @}
 * 
 */
