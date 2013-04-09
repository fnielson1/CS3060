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


void FirstCome(const int*, const int*, const int*, const int);
void ShortestJobFirst( const int*, const int*, const int*, const int);
void ShortestNext(const int*, const int*, const int*, const int);
int GetShortestServiceJob(const int*, const int*, const int, 
	const int, const int, int*);
void RoundRobin(const int*, const int*, const int*, const int*);
void Print(int, int, int, int);
void InputToArray(char*,int*, int*, int*);
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
	//FirstCome(arrPid, arrArrival, arrService, _pidIndex);
	
	// Calling ShortestJobFirst
	ShortestJobFirst( arrPid, arrArrival, arrService, _pidIndex );
	//RoundRobin();
	ShortestNext(arrPid, arrArrival, arrService, _pidIndex);
	return 0;
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
* const int jobNum: The length of the job list (array length).
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
	Print(0, 0, 0, PRINT_HEADER);
	// Loop through each job, calculating everything
	for(index = 0; index < jobNum; ++index)
	{
		// Wait time = how long process is waiting until first run.
		// Turnaround time = time from process first enters ready-state
		// 	till he exits the running state for the last time.
		curPid = arrPid[index];
		curArrivalTime = arrArrival[index];
		curServiceTime = arrService[index];

		if(curArrivalTime > currentTime)
			currentTime = curArrivalTime; // Simulate us waiting for the job

		// Calculate the wait time
		curWaitTime = currentTime - curArrivalTime;
		// Calculate the turnaround time
		curTurnaroundTime = curServiceTime;
		// Increment our current time by how the the process ran (service time)
		currentTime += curServiceTime; // Non-preemtive, service time = run time
		totalWaitTime += curWaitTime;
		totalTurnaroundTime += curTurnaroundTime;

		// Print the PID, wait time, and turnaround time for the job
		Print(curPid, curWaitTime, curTurnaroundTime, PRINT_NORMAL);
	}
	// Print the Average wait time and turnaround time for all the jobs
	avgWaitTime = totalWaitTime / jobNum;
	avgTurnaroundTime = totalTurnaroundTime / jobNum;
	Print(0, avgWaitTime, avgTurnaroundTime, PRINT_FOOTER);
}

/**********************************************************************
* Func: ShortestJobFirst
* Desc: From a list of jobs does the one which will complete
*	    quickest first
* Param: the process ID, when it arrived, the time for it to complete,
*	     the number of jobs it has to complete
* Rtrn: None
***********************************************************************/
void ShortestJobFirst( const int* pid, const int* arrival, const int* service, const int jobNumber )
{
	int pArrival[jobNumber];
	int pService[jobNumber];
	int timeTotal = 0;
	int minProcess = 0;
	int counter = 0;
	int i = 0;
	int waitTime = 0;
	int turnAroundTime = 0; // wait time + service time
	
	// copying arrays in case they need to be modified
	memcpy( pArrival, arrival, jobNumber * sizeof( int ) );
	memcpy( pService, service, jobNumber * sizeof( int ) );
	
	printf( "Shortest Job First\n" );
	Print(0,0,0, PRINT_HEADER);
	// loop through all the jobs
	for( counter; counter < jobNumber; counter++ )
	{
		// checking to see if the job as already been processed
		if( pService[counter] == 0 )
			continue;
		minProcess = pid[counter];
		// finding the shortest job	
		for( i; i < jobNumber; i++ )
		{
			if( pService[i] < pService[minProcess - 1] && ( pArrival[i] <= timeTotal ) )
				minProcess = pid[i];
		}
		// adding the time it took to do the job
		timeTotal += pService[minProcess - 1];
		pService[minProcess - 1] = 0;	// zeroing out it's service time because it's done
	 	if( pArrival[minProcess - 1] != 0 && pArrival[minProcess - 1] < timeTotal )
			waitTime = timeTotal - pArrival[minProcess - 1];
		else
			waitTime = 0;
		turnAroundTime = waitTime + pService[minProcess - 1];
		Print( minProcess, waitTime, turnAroundTime, PRINT_NORMAL );
	}
}
/*
** ShortestNext

* Simulates a scheduler that uses the Shortest 
	Remaining Time Next algorithim.

* const int* arrPid: The PIDs of the jobs.
* const int* arrArrival: The time that the job 
	arrived on the ready list.
* const int* arrService: How long the job will 
	take to complete.
* const int jobNum: The length of the job list (array length).
*/
void ShortestNext(const int* arrPid, const int*  arrArrival, 
	const int* arrService, const int jobNum)
{
	int curPid = 0;
	int curArrivalTime = 0;
	int curJobIndex = 0;
	int curWaitTime = 0;
	int curTurnaroundTime = 0;
	int allJobsDone = 0; 	// Whether all the jobs are done or not

	int currentTime = 0; 	// Keeps track of the time 
	int totalWaitTime = 0; 	// Total wait time for all the jobs
	int totalTurnaroundTime = 0; // Total turnaround time for all the jobs
	int avgWaitTime = 0; 	// The average wait time
	int avgTurnaroundTime = 0; // The average turnaround time

	int arrTimeLeft[jobNum]; // The time left for a specific job
	int arrWaitTime[jobNum]; // The wait time for a specific job
	memcpy(arrTimeLeft, arrService, jobNum * sizeof(int)); // Copy the service time to our array

	// Print the function that is running
	printf("%s\n", "Shortest Time Remaining Time");
	Print(0, 0, 0, PRINT_HEADER);
	// Loop through each Job
	while(1)
    {
        // Wait time = how long process is waiting until first run.
        // Turnaround time = time from process first enters ready-state
        //  till he exits the running state for the last time.
		
		// Get the shortest job to work on
		curJobIndex = GetShortestServiceJob(arrArrival, arrTimeLeft, curJobIndex,
			currentTime, jobNum, &allJobsDone);
		if(allJobsDone)
			break; // All the jobs are done

		curArrivalTime = arrArrival[curJobIndex];

        // Check if the job has arrived
        if(curArrivalTime <= currentTime)
        {
			// Calculate the wait time for the job if this is its first time run
			if(arrTimeLeft[curJobIndex] == arrService[curJobIndex])
				arrWaitTime[curJobIndex] = abs(curArrivalTime - currentTime);

            // The job has arrived. Run it for 1 ms
			++currentTime; // 1 ms has passed
			arrTimeLeft[curJobIndex]--;

			// Check if the job has completed. If so, print.
			if(arrTimeLeft[curJobIndex] == 0)
			{
				// turnaround time is the current time minus our arrival time 
				// plus how long we had to wait in the queue the first time.
				curPid = arrPid[curJobIndex];
				curWaitTime = arrWaitTime[curJobIndex];
				curTurnaroundTime = currentTime - (curWaitTime + curArrivalTime);
				totalWaitTime += curWaitTime;
				totalTurnaroundTime += curTurnaroundTime;

				// Print the PID, wait time, and turnaround time for the job
				Print(curPid, curWaitTime, curTurnaroundTime, PRINT_NORMAL);
			}
        } // END ARRIVAL CHECK
		else
	        ++currentTime; // No jobs have arrived. Wait for 1 ms.
    }
	// Print the Average wait time and turnaround time for all the jobs
	avgWaitTime = totalWaitTime / jobNum;
	avgTurnaroundTime = totalTurnaroundTime / jobNum;
	Print(0, avgWaitTime, avgTurnaroundTime, PRINT_FOOTER);
}

/*
** GetShortestServiceJob

* Returns the job index of the job that has the 
	shortest remaining service time.

* const int* arrArrival: The array that has the arrival times
	of the jobs.
* const int* arrTimeLeft: The array that has the remaining 
	service times of the jobs.
* const int curJobIndex: The index of the current job 
	that is running.
* const int currentTime: The current time (in ms).
* int* allJobsDone: If this is changed to a 1, than all the 
	jobs have been done.
*/
int GetShortestServiceJob(const int *arrArrival, const int *arrTimeLeft, 
	const int curJobIndex, const int currentTime, const int jobNum, 
	int *allJobsDone)
{
	int i = 0;
	int newJobIndex = curJobIndex;
	int jobsExist = 0;

	// If the current job has a time of 0 (it's done),
	// find another job that might be the shortest one.
	if(arrTimeLeft[curJobIndex] == 0)
	{
		for(i = 0; i < jobNum; ++i)
		{
			if(arrTimeLeft[i] != 0 && arrArrival[i] <= currentTime)
			{
				newJobIndex = i;
				jobsExist = 1; // A new job was found. 
				break;
			}
		}
	}
	else
		jobsExist = 1; // Our current job is still good

	// See if there is another job with a shorter service time
	for(i = 0; i < jobNum; ++i)
	{
		// Make sure if a job isn't already 'done'
		if(arrTimeLeft[i] == 0)
		{
			if(jobsExist == 0)
				*allJobsDone = 1; // There is a chance there are no more jobs
			continue;
		}
		
		if(arrTimeLeft[i] < arrTimeLeft[newJobIndex] &&
			arrArrival[i] <= currentTime)
		{
			// Another job is shorter and it's arrived.
			newJobIndex = i; // Change the current job
			*allJobsDone = 0; // There are still more jobs to do
			break;
		}
	}
	return newJobIndex; // If no new job is found, return the current one
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
	else if(header == PRINT_FOOTER)
	{
		printf(footerFormat, "Average Wait Time", "Average Turnaround Time");
		printf("%-20d %-20d \n\n\n", waitTime, turnaroundTime);
	}
	else
		printf(format, pid, waitTime, turnaroundTime);
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
