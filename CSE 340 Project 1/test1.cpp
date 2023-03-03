

#include "program1.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <cctype>
#include <stdio.h>
#include <vector>
#include <array>
#include <unordered_map>





using namespace std;



/* You may use this file to test your program.
	This file will not be submitted, as a different test1.cpp will be used. */
int main(int argv, char **argc) {


	//LoadGrammar("Sample Grammar.txt");
	LoadGrammar("input.txt");
	char *status = GetStats();
	char *myFirstSets = GetFirstSets();
	char *myFollowSets = GetFollowSets();

	
	cout << "Returned from GetStats" << endl;
	cout << status << endl;
	cout << "Returned from GetFirstSets" << endl;
	cout << myFirstSets << endl;

	cout << "Returned from GetFollowSets" << endl;
	cout << myFollowSets << endl;

	//cout << GetFirstSet("A");

	delete[] status;
	delete[] myFirstSets;
	delete[] myFollowSets;



	return 0;
}