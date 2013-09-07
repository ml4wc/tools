#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <sstream>
#include <gsl_rng.h>
#include <gsl_randist.h>
//beast /usr/include


using namespace std;


#define PARAMETER_CHECK(param, paramLen, actualLen) (strncmp(argv[i], param, min(actualLen, paramLen) )== 0) && (actualLen == paramLen)
#define HALF 0.50

void sample(string file, string lines);
void samplin_help(void);
void largeSamp(unsigned long length, unsigned long* pickArray, string file);
void smallSamp(unsigned long length, unsigned long* pickArray, string file);



int main(int argc, char* argv[])
{
	/*initialize cmd line parsing varoiables*/
	bool haveFile = false;
	bool haveLines = false;
	bool showHelp = false;

	/*look to output help*/

   if(argc <= 1) showHelp = true;

    for(int i = 1; i < argc; ++i) {
        int parameterLength = (int)strlen(argv[i]);

        if((PARAMETER_CHECK("-h", 2, parameterLength) ) ||
        (PARAMETER_CHECK("--help", 5, parameterLength) )) {
            showHelp = true;
        }
    }

	string strLines, fileName;

	for(int i = 1; i < argc; ++i) {

		int parameterLength = (int)strlen(argv[i]);

		if (PARAMETER_CHECK("-i", 2, parameterLength) ) {
			if ((i+1) < argc) {
				fileName = argv[i + 1];
				haveFile = true;
				++i;
			}
		} else if (PARAMETER_CHECK("-l", 2, parameterLength) ) {
			if ((i+1) < argc) {
				strLines = argv[i + 1];
				haveLines = true;
				++i;
			}
		} else {
			cerr << endl << "*****ERROR: Unrecognized parameter: " << argv[i] << " *****" << endl << endl;
			showHelp = true;
		}
	}
	
	if(showHelp) {
		samplin_help();
		exit(1);
	}	
	
	// make sure we have input file
    if (!haveFile) {
        cerr << endl << "*****" << endl << "*****ERROR: Need -i input file. " << endl << "*****" << endl;
        showHelp = true;
    }
    // make sure we have lines
    if (!haveLines) {
        cerr << endl << "*****" << endl << "*****ERROR: Need -l number of lines. " << endl << "*****" << endl;
        showHelp = true;
    }

	if (showHelp) { 
		samplin_help();
	} else if (!showHelp && haveLines && haveFile) {
		/*no problem or helps here, parse the file*/
		sample(fileName, strLines);
		/*it worked?*/
		return 0;
	} else {
	samplin_help();
	}
}

void sample(string file, string lines)
{
	istream *stream;
	string line;
	unsigned long numLines = 0, n1 = 0, i;
	float lenRatio, fnumLines, fn1;

	numLines = atoi(lines.c_str() );
	//assume stdin
	if (strcmp(file.c_str(), "stdin") == 0 || 
		strcmp(file.c_str(),"-") == 0) {
        stream = &cin;
    }
	//open stream if file
    else {
        stream = new ifstream(file.c_str(), ios::in);
	}
	//check if actually file
	if (stream->fail()) {
	cerr << "samplin : " << file << ": No such file or directory" << endl; exit(1);
	}
	
	while(getline(*stream, line)) {
		n1++;
	}
	
	if (numLines > n1) {
		cerr << "Number of lines to sample is greater than length of file" << endl;
		exit(2);
		/*Bail because too many lanes asked for*/
	}
	
	//random numbers
	const gsl_rng_type* T;
    gsl_rng* r;
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
	unsigned long *randArray = (unsigned long *) malloc(n1 * sizeof(unsigned long) );
	unsigned long *chooseArray = (unsigned long *) malloc(numLines * sizeof(unsigned long) );
	//initialize list of numbers to choose from
	for (i = 0; i < n1; ++i) {
		randArray[i] = i+1;
	}
	//select numbers from the list to pluck from file
	fnumLines = numLines;
	fn1 = n1;
	lenRatio = (fnumLines/fn1);
	if (lenRatio >= HALF) {
		gsl_ran_choose(r, chooseArray, n1-numLines, randArray, n1, sizeof(unsigned long) );
		largeSamp(n1-numLines, chooseArray, file); 
		/*Large l, over half of the file -- Select against the list of PRNs*/	
	}
	else if (lenRatio < HALF) {
		gsl_ran_choose(r, chooseArray, numLines, randArray, n1, sizeof(unsigned long) );
		smallSamp(numLines, chooseArray, file);
		/*Small l, less than half of the file -- Select from the list  of PRNs*/
	}
	gsl_rng_free (r);
}


void largeSamp(unsigned long numLines, unsigned long* pickArray, string file)
{	
	unsigned long n2 = 0, c =0;
	istream *stream;
	string line;
	if (strcmp(file.c_str(), "stdin") == 0 || 
		strcmp(file.c_str(),"-") == 0) {
        stream = &cin;
    }
	//open stream if file
    else {
        stream = new ifstream(file.c_str(), ios::in);
	}
	//check if actually file
	if (stream->fail()) {
	cerr << "samplin : " << file << ": No such file or directory" << endl; exit(1);
	}
	
	while(getline(*stream, line)) {
		n2++;
		if (n2 != pickArray[c]) {
			cout << line << endl;
			c++;
		}
	}
}

void smallSamp(unsigned long numLines, unsigned long* pickArray, string file)
{
	unsigned long n2 = 0, c = 0;
	istream *stream;
	string line;
	if (strcmp(file.c_str(), "stdin") == 0 || 
		strcmp(file.c_str(),"-") == 0) {
        stream = &cin;
    }
	//open stream if file
    else {
        stream = new ifstream(file.c_str(), ios::in);
	}
	//check if actually file
	if (stream->fail()) {
	cerr << "samplin : " << file << ": No such file or directory" << endl; exit(1);
	}
	
	while(getline(*stream, line)) {
		n2++;
		if (n2 == pickArray[c]) {
			cout << line << endl;
			c++;
		}
	}
	exit(0);
}


void samplin_help(void) {

    cerr << "This program will randomly sample lines from a file and return a desired number lines from the file (in order)." << endl << endl;
    cerr << "Usage:    samplin -i <file> -l <number of lines>" << endl << endl;
    cerr << "Options: " << endl;
    cerr << "\t-i\t"                    << "A file that separated by newlines." << endl;
    cerr << "\t-l\t"                    << "Number of randomly selected lines desired." << endl;

    // end the program here
    exit(1);
}





/*
  int n;
  qsort (values, 6, sizeof(int), compare);
  for (n=0; n<6; n++)
     printf ("%d ",values[n]);
  return 0;
*/

