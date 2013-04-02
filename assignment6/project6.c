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

#define MAX_INPUT_LEN (200)
#define BASE_TEN (10)
#define PRINT_HEADER (1)
#define PRINT_NORMAL (0)
#define PRINT_FOOTER (2)


void Print(int, int, int, int);
void InputToArray(char*,int*, int*, int*);
void FirstCome(const int*, const int*, const int*, const int);
int StrToInt(char*);


// GLOBAL
// Keep track of the indexes on the 
// arrays (one job added after another.
int _pidIndex = 0;


int main()
{
	char input[MAX_INPUT_LEN];
	int arrPid[MAX_INPUT_LEN];
	int arrArrival[MAX_INPUT_LEN];
	int arrService[MAX_INPUT_LEN];

	// Read data from stdin
	while(fgets(input, MAX_INPUT_LEN, stdin) != 0)
	{
		InputToArray(input, arrPid, arrArrival, arrService);
	}
	// Call function to print the 3 arrays
	FirstCome(arrPid, arrArrival, arrService, _pidIndex);
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
* int* arrService: The time for the job to complete (ms)
*/
void InputToArray(char* input, int* arrPid, int* arrArrival, int* arrService)
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
			arrService[_pidIndex] = number;
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

/*
** FirstCome

* Takes 3 arrays that simulate jobs entering the
	waiting queue. It then 'runs' each of the jobs,
	keeping track of various calculations.

* const int* arrPid: The PIDs of the jobs.
* const int* arrArrival: The time that the job 
	arrived on the ready list.
* const int* arrService: How long the job will 
	take to complete.
* const int jobNum: The length of the job list (arrays).
*/
void FirstCome(const int* arrPid, const int*  arrArrival, 
	const int* arrService, const int jobNum)
{
	/*
	Calculate:
    1. The wait time for each process
    2. The turnaround time for each process
    3. The average wait time for all processes given to the simulation
    4. The average turnaround time for all processes given to the simulation
	*/

	int curPid = 0;
	int curArrivalTime = 0;
	int curServiceTime = 0;
	int curWaitTime = 0;
	int curTurnaroundTime = 0;
	int index = 0;

	int currentTime = 0; // Keeps track of the time 
	int totalWaitTime = 0; // Total wait time for all the jobs
	int totalTurnaroundTime = 0; // Total turnaround time for all the jobs
	int avgWaitTime = 0; // The average wait time
	int avgTurnaroundTime = 0; // The average turnaround time

	// Print which function is running
	printf("%s\n", "First Come First Serve");

	// Loop through each job, calculating everything
	for(index = 0; index < jobNum; ++index)
	{
		// Wait time = how long process is waiting until first run.
		// Turnaround time = time from process  first enters ready-state
		// 	till he exits the running state for the last time.
		curPid = arrPid[index];
		curArrivalTime = arrArrival[index];
		curServiceTime = arrService[index];

		// Calculate the wait time
		curWaitTime = abs(curArrivalTime - currentTime);
		// Calculate the turnaround time
		curTurnaroundTime = curWaitTime + curServiceTime;
		// Increment our current time by how the the process ran (service time)
		currentTime += curServiceTime; // Non-preemtive, service time = run time
		totalWaitTime += curWaitTime;
		totalTurnaroundTime += curTurnaroundTime;

		// Print the PID, wait time, and turnaround time for the job
		if(index == 0)
			Print(curPid, curWaitTime, curTurnaroundTime, PRINT_HEADER);
		else
			Print(curPid, curWaitTime, curTurnaroundTime, PRINT_NORMAL);
	}
	// Print the Average wait time and turnaround time for all the jobs
	avgWaitTime = totalWaitTime / jobNum;
	avgTurnaroundTime = totalTurnaroundTime / jobNum;
	Print(0, avgWaitTime, avgTurnaroundTime, PRINT_FOOTER);
}

/*
** Print

* Prints the job PID, how much time he waited to
	start, and the turnaround time.

* int header: Whether to print the header or not.
* int pid: The PID of the job.
* int waitTime: How long the job was in the read queue.
* int turnaroundTime: Total time from first ready to done running.
*/
void Print(int pid, int waitTime, int turnaroundTime, int header)
{
	char *format = "%-15d %-15d %-15d \n"; // How to format the printing
	char *headerFormat = "%-15s %-15s %-15s \n"; 
	char *footerFormat = "\n%-20s %-20s \n";

	if(header == PRINT_HEADER)
		printf(headerFormat, "Process ID", "Wait Time", "Turnaround Time");
	if(header == PRINT_FOOTER)
	{
		printf(footerFormat, "Average Wait Time", "Average Turnaround Time");
		printf("%-20d %-20d \n", waitTime, turnaroundTime);
		return; // Don't print the normal data
	}	
	printf(format, pid, waitTime, turnaroundTime);
}
