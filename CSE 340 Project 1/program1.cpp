//Name: Thomas Derr
//Class: CSE 340 - Mark Fisher
//Assignment: Project 1
//Description: Parses a CFG and returns statistcs and other usefull information about it

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
#include <unordered_map>

using namespace std;

//Global Variables 
bool grammarValid = false; 								//tells other functions if grammar was properly loaded (false by default)
unordered_map<string, vector<vector<string>>> rules;    //global rules struct maps nonTerminal keys to rule vectors holding string vectors for a given right side
vector<string> nonTerminals;							//holds all nonterminals in order gathered
vector<string> terminals;								//holds all terminls sorted 
unordered_map<string, vector<string>> firstSets;   		//struct to hold first sets
unordered_map<string, vector<string>> followSets;		//struct to hold follow sets

//Macros
#define ISBI istreambuf_iterator						//made loading in grammar easier

bool isId(string token) //Checks if a token is an id or not
{
	if (token.size() > 0) //not empty string
	{
		if (isalpha(token[0])) //starts with letter
		{
			if (token.size() > 1) //if there more after the letter
			{
				for(int i = 1; i < token.size(); i++)
				{
					if (!isalpha(token[i]) && !isdigit(token[i]))
					{
						return false; //part of token not number or digit
					}
				}//checks if the rest is numbers and digits
			}
			return true;//just a letter so its good
		}
		return false;//doesn't start with a letter
	}	
	return false;//is empty
}// end isID


void getFirsts() //Helper Function called by LoadGramar, loads firsts so GetFirstsets and GetFirstSet can just grab from the data we gather
{
	if (grammarValid == false)
	{
		return;
	}//if the grammar was incorrectly loaded

	for (int i = 0; i < terminals.size(); i++)
	{
		firstSets[terminals[i]].push_back(terminals[i]);  
	}// FIRST(a) = {a} for all nonterminals a

	firstSets["#"].push_back("#"); //first epsilon equals epsilon

	for (int i=0; i < nonTerminals.size(); i++)
	{
		firstSets.insert(make_pair(nonTerminals[i], vector<string>{} ));
	}//FIRST(A) = {} for all non terminals A
	
	bool changed = true; //checks any first sets have been changed in the last iteration
	while (changed == true) //while we have made changes
	{
		changed = false; // set to false at beginning of loop. if we change somthing make it true

		for (int i = 0; i < nonTerminals.size(); i++)// for every nonTerminal T
		{
			for (int j = 0; j < rules[nonTerminals[i]].size(); j++) //for every rule for T
			{
				bool forAll = true; //set to false if all terminals in this rule have epsilon in their first set
				for(int k = 0; k < rules[nonTerminals[i]][j].size(); k++) //for all B_0... b_k in that rule
				{
					string tempBi = rules[nonTerminals[i]][j][k]; //temp right hand side B_i
					string tempA = nonTerminals[i]; //temp left hand side for easier developent
	
					for (int l = 0; l < firstSets[tempBi].size(); l++)  //set union FIRST(A) U FIRST(B_i) - {epsilon}..... for all tokens in FIRST(B_i)
					{
							if (find(firstSets[tempA].begin(), firstSets[tempA].end(), firstSets[tempBi][l]) == firstSets[tempA].end()) //if not already in FIRST(A)
							{
								if (firstSets[tempBi][l] != "#") //if not epsilon
								{							
									firstSets[tempA].push_back(firstSets[tempBi][l]);  //add to FIRST(A)
									changed = true; // we made change so we need to go through again
								} 
							} 
					} //end set union

					if (find(firstSets[tempBi].begin(), firstSets[tempBi].end(), "#") == firstSets[tempBi].end())// If epsilon not in first(B_i)
					{
						forAll = false; //dont add # to the FIRST(A)
						break;   //leave loop and skip set union
					} 
				}

				if (forAll == true)  //if all terminals in this rule have epsilon in their first set
				{
					if (find(firstSets[nonTerminals[i]].begin(), firstSets[nonTerminals[i]].end(), "#") == firstSets[nonTerminals[i]].end()) //if FIRST(A) doesnt have epsilon
					{
						firstSets[nonTerminals[i]].push_back("#"); //add epsilon to FIRST(A)
						changed = true; // we made change so we need to go through again
					}		
				}//end for all
			}//end all rules for nonterminal	
		}//end for all nonterminals	
	} //end while we have changed

	for (int i = 0; i < nonTerminals.size(); i++) //for all nonterminals A
	{
		vector<string> tempVec = firstSets[nonTerminals[i]];  //copy FIRST(A)
		sort(tempVec.begin(), tempVec.end());   //sort the copy
		firstSets[nonTerminals[i]] = tempVec;   //set FIRST(A) to the copy
	}//sorts each nonterminals vector

	return;
}//end getFirsts


void getFollows()
{
	if (grammarValid == false) //If grammar incorrectly loaded
	{
		return;  //do nothing
	}

	for (int i=0; i < nonTerminals.size(); i++)
	{
		followSets.insert(make_pair(nonTerminals[i], vector<string>{} ));
	}//set first(A) = {} for all nonterminal A 

	
	followSets[nonTerminals[0]].push_back("##");   //follow S = $    (## in this case)

	bool changed = true; //keeps track if follow sets have been changed, set to false each iteration then set true if change is made

	while (changed == true) //while we have changed last iteration
	{
		changed = false; // we will set back to true if we make any changes

		for (int i = 0; i < nonTerminals.size(); i++) //for all nonterminals B
		{
			string tempB = nonTerminals[i];     //keeps track of B name easier

			for (int j = 0; j < rules[nonTerminals[i]].size(); j++) //for each rule C for nonterminal B 
			{
				for(int k = 0; k < rules[nonTerminals[i]][j].size(); k++) //for tokens in right side of rule C (this goes through all C_i)
				{
					string tempCi = rules[nonTerminals[i]][j][k]; //temp token 			

					bool forAll = true; //keeps track if all c_i have epsilon

					for (int l = k+1; l < rules[nonTerminals[i]][j].size(); l++)  //for C_j,   j = i+1 to k
					{
						string tempCj = rules[nonTerminals[i]][j][l];   //keeps track of C_j name easier

						for (int m = 0; m < firstSets[tempCj].size(); m++) //FOLLOW(C_i) = FOLLOW(C_i) U FIRST(C_j) - epsilon    (for each item in FIRST(C_j))
						{
								if (find(followSets[tempCi].begin(), followSets[tempCi].end(), firstSets[tempCj][m]) == followSets[tempCi].end()) //if not in FOLLOW(C_i)
								{
									if (firstSets[tempCj][m] != "#")  //if not epsilon
									{
										followSets[tempCi].push_back(firstSets[tempCj][m]);   //add to FOLLOW(C_i)
										changed = true; // we made change so we need to go through again
									} 
								} 
						} //end set union

						if (find(firstSets[tempCj].begin(), firstSets[tempCj].end(), "#") == firstSets[tempCj].end()) //if FIRST(C_j) doesnt contain epsillon
						{
							forAll = false;  //not all C_j > C_i have epsilon in their first set
							break;  //exit loop
						} 
					} //end for j = i+1 to k
	
					if (forAll == true) // if all C_j > C_i have epsilon in their first set
					{
						for (int m = 0; m < followSets[tempB].size(); m++) //FOLLOW(C_i) = FOLLOW(C_i) U FOLLOW(B)    for all items in FOLLOW(B)
						{
							if (find(followSets[tempCi].begin(), followSets[tempCi].end(), followSets[tempB][m]) == followSets[tempCi].end())  //if not in FOLLOW(C_i)
							{
								followSets[tempCi].push_back(followSets[tempB][m]); //add to C_i
								changed = true; // we made change so we need to go through again
							} 
						}//end set union
					}// end forAll
				}//end rule token iterator
			}//end for all rules
		} //end for all symbols
	} // end while we have changed

	for (int i = 0; i < nonTerminals.size(); i++)  //sort FOLLOW(A) for each nonterminal A 
	{
		vector<string> tempVec = followSets[nonTerminals[i]];  //TEMPFOLLOW_A = FOLLOW(A)
		sort(tempVec.begin(), tempVec.end());  //Sort TEMPFOLLOW_A
		followSets[nonTerminals[i]] = tempVec; //FOLLOW(A) = TEMPFOLLOW_A
	}//end sort Follow Sets
}//end getFollows


void LoadGrammar(char *grammarFileName)// Loads the grammar
{
	string fileContents;			 // define empty string to hold grammar
	ifstream afile(grammarFileName); // file name

	if (afile.is_open())
	{
		fileContents = string((ISBI<char>(afile)), ISBI<char>());  //get the file
		afile.close(); //close file
	} // read in file

	string text = "";   //empty string to prepare for tokenizing (remove consecutive whitespace)

	for (int i = 0; i < fileContents.size(); i++)  //for each character fileContents
	{
		if (fileContents[i] == ' ') //if it is a space
		{
			if (i + 1 < fileContents.size()) //if is not the end
			{
				if (fileContents[i+1] != ' ') //if the next character is not a space
				{
					text += fileContents[i];  //add to text
				}
			}
		}
		else 
		{
			text += fileContents[i]; //add to text
		}
	}//consecutive space checker (iterates and adds each character to new string in order but skips spaces with a space after them)

	istringstream content(text); 	//make for easier tokenizing (easier spliting by line and space)
	vector<string> tokens;			//vector of strings for tokens
	string token;					//holds temporary token
	string line;					//holds temporary line

	while (getline(content, line, '\n')) //while we have lines to read
	{
		stringstream space(line);		//grab our line
		while (getline(space, token, ' '))  //while we have spaces to read on our line (for each item between spaces)
		{
			if (!isspace(token[0])) //if the first index is not a space
			{
				tokens.push_back(token); //add it to tokens list
			}
		}//end line
	}//end file


	vector<string> tempNonterminals;  	//will hold nonterminals
	int tmpTokInd = 0;					//will hold where the nonterminals end in the tokens list
	string tempToken = tokens[tmpTokInd];	//temporary token to march through tokens
	
	while (tempToken != "@") //read until first @ (grab the nontermials)
	{
		if (isId(tempToken) == false || tmpTokInd == (tokens.size() - 1)) //if token is not a valid id
		{
			grammarValid = false;  //gramar is invalid
			return;
		}
		tempNonterminals.push_back(tempToken);  //add to the nonterminals list
		
		tmpTokInd += 1;			//add to index
		tempToken = tokens[tmpTokInd]; //march token forward
	}
	
	if (tokens.size() > tmpTokInd + 2)//if there is more after the nonterminals list (atleast a rule and @@)
	{
		tmpTokInd += 1;
		tempToken = tokens[tmpTokInd];  //move to next symbol (this will set token and tokens[tmptokInd] to lefthandside of first rule in grammar)
	}
	else
	{
		grammarValid = false; //grammar doesnt contain rules or end grammar
		return;
	}

	while (tempToken != "@@") //while not at end of grammar   -- parses rules  -- starts at right left side of first nonterminal
	{				
		while (tempToken != "@")  //while not at end of line 
		{
			if (find(tempNonterminals.begin(), tempNonterminals.end(), tempToken) == tempNonterminals.end())  //if first token on line not in nonTerminals
			{
				grammarValid = false;  //left hand side of a rule has something other than non terminal
				return;
			}

			if (tokens.size() < tmpTokInd + 2) //if there are atleast two more items in the grammar 
			{		
				grammarValid = false; // needs to be atleast a -> and a @
				return;
			}
				
			tmpTokInd += 1;  //move token index
			tempToken = tokens[tmpTokInd]; //march forward 

			if (tempToken != "->") //arrow should be next
			{
				grammarValid = false;
				return;
			}

			tmpTokInd += 1;  //move token index
			tempToken = tokens[tmpTokInd];  //march forward

			if (tempToken == "#") //if right hand side starts with epsilon
			{	
				tmpTokInd += 1;  //move index
				tempToken = tokens[tmpTokInd];  //march token

				if (tempToken != "@")  //endline needs to come next as rightside is IDs OR epsilon
				{
					grammarValid = false;
					return;
				}
			}
			else //if rhs not start with epsilon
			{
				if (tempToken == "@") //needs to be atleast one id
				{
					grammarValid = false;
					return;
				}

				while (tempToken != "@")  //until end line
				{					
					if (isId(tempToken))//checi if token is id
					{						
						tmpTokInd += 1;  //move index
						tempToken = tokens[tmpTokInd];	//march forward 
					}
					else    //if run into anything else return false
					{
						grammarValid = false;
						return;
					}					
				}//end Ids
			}//end rhs
		}//end line

		tmpTokInd += 1;  //move index
		tempToken = tokens[tmpTokInd];	//march forward to either new nonTerminal or end grammar	
	}//end grammar

	if (tmpTokInd == tokens.size() - 1)//if end grammar came at the end of tokens list (we made it to end)
	{
		grammarValid = true;
	}
	else
	{
		grammarValid = false;
		return;
	}

	//rule storing
	int endNonTerminals = 0; //marker for where nonterminals end in our token stream
    for (endNonTerminals; endNonTerminals < tokens.size(); endNonTerminals++) //get nonterminals list
	{
		string tk = tokens[endNonTerminals];
		if (tk != "@")  //unil first end section
		{
			nonTerminals.push_back(tk); //add to list
		}
		else
		{
			break;  //leave loop when run into @
		}
    }//now we have marker

	for (int i=0; i < nonTerminals.size(); i++)
	{
		rules.insert(make_pair(nonTerminals[i], vector<vector<string>>{} ));
	}//make a key value pair of string vector vectors in the rules map for each nonterminal

	int rulesIndex = endNonTerminals + 1;  //rules begin directly after the end of the nonTerminals
	string tmp = tokens[rulesIndex];    //set temp token to value of our first index in rules

	while (tmp != "@@") //until endfile
	{
		vector<string> tempVec; //temporary rule vector
		string name = tmp;  //set name to first id

		rulesIndex += 2;   //skip index over arrow
		tmp = tokens[rulesIndex]; //march forward

		while (tmp != "@")  //until end line
		{
			tempVec.push_back(tmp); //push the rest of the tokens on the line except for an @ (this makes the right hand side of a single rule)
			rulesIndex += 1;  //move index
			tmp = tokens[rulesIndex]; //march token forward
		}

		rules[name].push_back(tempVec);  //push our temp rule into the vector in rules dictionary at that nonterminal

		rulesIndex += 1; //add to index to go to the next beginning of line (this runs when on an @)
		tmp = tokens[rulesIndex]; //change tmp accordingly
	}//runs once for each line in the rules of a file

	GetStats();     //call getstats  (also gets terminals so it is important we call it here for the firsts and follows)
	getFirsts();    //get first sets
	getFollows();   //get follow sets
}


char* GetStats()  // Returns a string with some statistics for the grammar.
{
	if (grammarValid == false)
	{
		string ret = "";
		char* myRet = new char[0]; 
    	strcpy(myRet, ret.c_str()); 
		return myRet;
	}//returns empty char *

	unordered_map<string, int> leftCounts;  //first line of stats
	unordered_map<string, int> rightCounts; //second line of stats

	for (const auto kv : rules) //for all rules map keys (for all nonerminals)
	{
		leftCounts[kv.first] = rules[kv.first].size(); //get the size of the rule vector (how many rules there are)
	}


	//assembles first line
	string leftStr = nonTerminals[0]+ " - " + to_string(leftCounts[nonTerminals[0]]);
	for (int i = 1; i < nonTerminals.size(); i++)
	{
		leftStr = leftStr + ", " + nonTerminals[i] + " - " + to_string(leftCounts[nonTerminals[i]]) ;
	}
	leftStr = leftStr + "\n";
	

	for (auto token : nonTerminals) //for all tokens in nonTerminls
	{
		for (int i = 0; i < rules[token].size(); i++)  //for all rules in a given nonterminal
		{
			for (int j = 0; j < rules[token][i].size(); j++) //for all tokens in said rule
			{
				if (leftCounts.find(rules[token][i][j]) == leftCounts.end()) // if the token doesnt appear on a left side (its a terminal)
				{
					if (rightCounts.find(rules[token][i][j]) == rightCounts.end()) // if the token hasnt been seen yet
					{
						rightCounts[rules[token][i][j]] = 1;  //map the count of that token to 1 in the right counts vector

						if (find(terminals.begin(), terminals.end(), rules[token][i][j]) == terminals.end())  //if its not in the terminals vector
						{
							terminals.push_back(rules[token][i][j]); //push token to terminals vector so we can keep track
						}		
					}
					else  //we have seen it before so we need to add to the count for the given terminal
					{
						rightCounts[rules[token][i][j]] += 1; //add to the count
					}
				}// end of if the token is a terminal (do nothing if not)
			}//end for all tokens in rule
		}//end for all rules in nonterminal
    }//end for all nonTerminals

	sort(terminals.begin(), terminals.end());  //sort the terminals


	//Assembles second line
	string rightStr = "";
	if (terminals.size() > 0)
	{
		rightStr = terminals[0] + " - " + to_string(rightCounts[terminals[0]]);

		for (int i = 1; i < terminals.size(); i++)
		{
			rightStr = rightStr  + ", " + terminals[i] + " - " + to_string(rightCounts[terminals[i]]);
		}	
	}
	rightStr += "\n";
	

	//Assembles return value of GetStats and returns
	string ret = leftStr + rightStr;
	char* myRet = new char[ret.length() + 1]; 
    strcpy(myRet, ret.c_str()); 
	return myRet;
}


char* GetFirstSets()  // Returns the first sets of all nonterminals.
{
	if (grammarValid == false)//if grammar not valid return blank
	{
		string ret = "";
		char* myRet = new char[0]; 
    	strcpy(myRet, ret.c_str()); 
		return myRet;
	}


	//Assembles The return value by grabbing from the firstSets Map
	string ret = "";
	for (int i = 0; i < nonTerminals.size(); i++)
	{
		ret += "FIRST(";
		ret += nonTerminals[i];
		ret += ") = {";
		
		for (int j = 0; j < firstSets[nonTerminals[i]].size(); j++)  
		{
			ret += " ";
			ret += firstSets[nonTerminals[i]][j];

			if (j < firstSets[nonTerminals[i]].size() - 1)
			{
				ret += ",";
			}
		}//for every token in that nonterminal

		if (nonTerminals.size() == 1)
		{
			ret += " }";
		}
		else
		{
			ret += " }\n";
		}
	}//for every nonterminal

	//Makes Char * from string and returns
	char* myRet = new char[ret.length() + 1]; 
    memcpy(myRet, ret.c_str(), ret.size() +1); 
	return myRet;
}//End GetFirstSets


char* GetFirstSet(char *nonTerminal)  // Returns the first set of the specified nonterminal.
{
	if (grammarValid == false || find(nonTerminals.begin(), nonTerminals.end(), nonTerminal) == nonTerminals.end())//if grammar not loaded or nonTerminal not exist
	{
		string ret = "";
		char* myRet = new char[0]; 
    	strcpy(myRet, ret.c_str()); 
		return myRet;
	}//return empty char*

	//Assembles Return from firstSets map
	string ret = "FIRST(";
	ret += nonTerminal;
	ret += ") = { ";	
	for (int j = 0; j < firstSets[nonTerminal].size(); j++)
	{
		ret += firstSets[nonTerminal][j];
		if (j < firstSets[nonTerminal].size() - 1)
		{
			ret += ", ";
		}
	}//for every token in that nonterminal
	ret += " }";

	//Converts string to char * an returns
	char* myRet = new char[ret.length() + 1]; 
    memcpy(myRet, ret.c_str(), ret.size() +1); // 
	return myRet;
}// end GetFirstSet


char* GetFollowSets() // Returns the follow sets of all nonterminals.
{
	if (grammarValid == false) //if grammar improperly loaded
	{
		string ret = "";
		char* myRet = new char[0]; 
    	strcpy(myRet, ret.c_str()); 
		return myRet;
	}// return empty char *


	//Assembles string to return from followSets map
	string ret = "";
	for (int i = 0; i < nonTerminals.size(); i++)//for each nonterminal
	{
		ret += "FOLLOW(";
		ret += nonTerminals[i];
		ret += ") = {";
		for (int j = 0; j < followSets[nonTerminals[i]].size(); j++) //for each instance in the followsets vecctor for that
		{
				ret += " ";
				ret += followSets[nonTerminals[i]][j];
				if (j < followSets[nonTerminals[i]].size() - 1)
				{
					ret += ",";
				}
		}//for every token in that nonterminal
		if (nonTerminals.size() == 1)
		{
			ret += " }";
		}
		else
		{
		ret += " }\n";
		}
	}

	//Convert string to char * and return 
	char* myRet = new char[ret.length() + 1]; 
    memcpy(myRet, ret.c_str(), ret.size() +1); // 
	return myRet;
}//end GetFollowSets


char* GetFollowSet(char *nonTerminal) // Returns the follow set of the specified nonterminal.
{
	if (grammarValid == false || find(nonTerminals.begin(), nonTerminals.end(), nonTerminal) == nonTerminals.end()) //if grammar invalid or nonTerminal not exist
	{
		string ret = "";
		char* myRet = new char[0]; 
    	strcpy(myRet, ret.c_str()); 
		return myRet;
	}

	//Assembles return string from followSets map
	string ret = "";//
	ret += "FOLLOW(";
	ret += nonTerminal;
	ret += ") = { ";	
	for (int j = 0; j < followSets[nonTerminal].size(); j++)
	{
		ret += followSets[nonTerminal][j];
		if (j < followSets[nonTerminal].size() - 1)
		{
			ret += ", ";
		}
	}//for every token in that nonterminal
	ret += " }";

	//Converts String To Char * and returns
	char* myRet = new char[ret.length() + 1]; 
    memcpy(myRet, ret.c_str(), ret.size() +1); // 
	return myRet;
}//end Get FollowSet