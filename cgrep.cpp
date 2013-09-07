#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

//#define PARAMETER_CHECK(param, paramLen, actualLen) (strncmp(argv[i], param, min(actualLen, paramLen) )== 0) && (actualLen == paramLen)

void search(string file, string delim, string searchStr);
void cgrep_help(void);
void Tokenize(const string &str, vector<string> &elems, const string &delimiter);




main(int argc, char* argv[])
{
	/*initialize cmd line parsing varoiables*/
	bool haveFile = true;
	bool haveString = false;
	bool showHelp = false;
	bool lineNum = false;
	int i;
	string searchStr, delim = "\t", fileName = "stdin";
	
	/*look to output help*/
	if(argc <= 1) showHelp = true;
	for(int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-h") == 0 ||
		strcmp(argv[i], "-?") == 0  || 
		strcmp(argv[i], "-help") == 0 || 
		strcmp(argv[i],"--help") == 0) {
		showHelp = true;
		}
		//start looking for options
		else if (strcmp(argv[i], "-d") == 0) {
			i++;
			delim = argv[i];
		} 
		else if (strcmp(argv[i], "-n") == 0) {
			lineNum = true;
		}
		else if (!haveString) {
			searchStr = argv[i];
			haveString = true;
		}
		else if (haveString) {
			fileName = argv[i];
			haveFile = true;
		}
	}

	if(showHelp) {
		cgrep_help();
		exit(1);
	}	
	
	// make sure we have input string
    if (!haveString) {
        cerr << endl << "*****" << endl << "*****ERROR: Need input search string. " << endl << "*****" << endl;
        showHelp = true;
    }

	if (showHelp) { 
		cgrep_help();
	} else if (!showHelp && haveString) {
		/*no problem or helps here, parse the file*/
		search(fileName, delim, searchStr);
		/*it worked?*/
		return 0;
	} else {
	cgrep_help();
	}
}

void search(string file, string delim, string searchStr)
{
	unsigned long lineNum = 0;
	istream *stream;
	string line;
	vector<std::string> fields;
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
	cerr << "cgrep : " << file << ": No such file or directory" << endl; exit(1);
	}
	
	//get each line and look for matches
	while(getline(*stream, line)) {
		Tokenize(line, fields, delim);
		lineNum++;
		int pos;
		for(std::vector<std::string>::size_type i = 0; i != fields.size(); i++) {
			pos = fields[i].find(searchStr);
			if (pos >= 0) {
						std::cout << "l:" << lineNum <<"\tc:" << i+1 << "\ts:" << fields[i] << endl;	
			}
		}
		fields.clear();
		}
}

// tokenize into a list of strings.
void Tokenize(const string &str, vector<string> &elems, const string &delimiter) 
{
    char* tok;
    char cchars [str.size()+1];
    char* cstr = &cchars[0];
    strcpy(cstr, str.c_str());
    tok = strtok(cstr, delimiter.c_str());
    while (tok != NULL) {
        elems.push_back(tok);
        tok = strtok(NULL, delimiter.c_str());
    }
}
	//if (strcmp(file.c_str(),"stdin") == 0)
	//	fp = stdin;
	//else
	//	fp = fopen(file.c_str(), "r");
	//	if (fp == NULL) {
	//		cerr << file << " : No such file or directory" << endl;
	//		exit(1);
	//	}
	//while(fgets(str,sizeof(str),fp) != NULL) {
	//	printf("%sFOO", "Encountered a Tab");
	//	} 

	//	pos = str.c_str().find(searchStr));
	//	printf("%", pos);





void cgrep_help(void) {

    cerr << "Reports the line (l): column (c): and string (s): that a string matches from a single file" << endl << endl;
    cerr << "Usage:     grep [OPTIONS] PATTERN FILE " << endl << endl;
    cerr << "Options: " << endl;
    cerr << "\t-d\t"                    << "Delimiter (default = tab)" << endl;

    // end the program here
    exit(1);
}



