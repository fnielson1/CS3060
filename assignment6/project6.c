/*
* Authors:
* Frank Nielson
* Caleb Kruger

* Class: CS 3060
* Project: Assignment 6

* This code has be written by the authors
* listed above.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LEN (100)
#define BASE_TEN (10)


void InputToArray(char*,int*, int*, int*);
int StrToInt(char*);

// GLOBAL
// Keep track of the indexes on the arrays
int _pidIndex = 0;


int main()
{
	char input[MAX_INPUT_LEN];
	int arrPid[MAX_INPUT_LEN];
	int arrArrival[MAX_INPUT_LEN];
	int arrTime[MAX_INPUT_LEN];

	// Read data from stdin
	while(fgets(input, MAX_INPUT_LEN, stdin) != 0)
	{
		InputToArray(input, arrPid, arrArrival, arrTime);
	}
	// Call function to print the 3 arrays

	return 0;
}

/*
** InputToArray

* Takes an input string and seperates it 
	on whitespace and puts each value into
	it's repective array.

* const char* input: The input string to parse.
* int* arrPid: The array that will have the PIDs.
* int* arrArrival: The time the job arrived (ms)
* int* arrTime: The time for the job to complete (ms)
*/
void InputToArray(char* input, int* arrPid, int* arrArrival, int* arrTime)
{
	char *splitStr;
	char *delim = "\t "; // Split on tabs and spaces
	int number = 0;
	int firstLoop = 0;
	
	splitStr = strtok(input, delim); // Get the first number
	number = StrToInt(splitStr); // Convert it to an int
	arrPid[_pidIndex] = number; // It's the PID
	while(1)
	{
		splitStr = strtok(NULL, delim);
		if(splitStr == NULL)
			break;
		number = StrToInt(splitStr);
		// Check if this is the Arrival or the Time
		if(firstLoop == 0)
		{
			arrArrival[_pidIndex] = number;
			firstLoop = 1;
		}	
		else
			arrTime[_pidIndex] = number;
	}
	++_pidIndex; // Increment our index
}


/*
** StrToInt

* Converts the given string to an integer.

* char* buffer: The string to convert
* returns: An integer
*/
int StrToInt(char *buffer)
{
	char *ibuffer;
	int number = 0;

	// If the last character is a newline; end it
	if(buffer[strlen(buffer) - 1] == '\n')
		buffer[strlen(buffer) - 1] = '\0';

	number = (int)strtol(buffer, &ibuffer, BASE_TEN);
	if(strlen(ibuffer) != 0)
	{
		fprintf(stderr, "%s\n", "Invalid integer!");
		exit(1);
	}
	return number;
}

