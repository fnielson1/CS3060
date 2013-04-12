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
void RoundRobin(const int*, const int*, const int*, const int);
void Print(int, double, double, int);
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
	FirstCome(arrPid, arrArrival, arrService, _pidIndex);
	ShortestJobFirst( arrPid, arrArrival, arrService, _pidIndex );
	ShortestNext(arrPid, arrArrival, arrService, _pidIndex);
	RoundRobin( arrPid, arrArrival, arrService, _pidIndex );
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
	double totalWaitTime = 0; // Total wait time for all the jobs
	double totalTurnaroundTime = 0; // Total turnaround time for all the jobs
	double avgWaitTime = 0; // The average wait time
	double avgTurnaroundTime = 0; // The average turnaround time

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
		curTurnaroundTime = curServiceTime + curWaitTime;
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
	int pService[jobNumber];
	int timeTotal = 0;
	int minProcess = 0;
	int counter;
	int tempMin;
	double waitTime = 0;
	double turnAroundTime = 0; // wait time + service time
	double totalWait = 0;
	double totalTurnAround = 0;
	double aveWait = 0;
	double aveTurnAround = 0;
	int readyQueue[jobNumber];
	int next = 0;
	int process = 0;
	int toFinish = jobNumber;

	// copying arrays in case they need to be modified
	memcpy( pService, service, jobNumber * sizeof( int ) );
	for( counter = 0; counter < jobNumber; counter++ )
		readyQueue[counter] = -1;

	printf( "Shortest Job First\n" );
	//Print Header
	Print(0,0,0, PRINT_HEADER);
	// loop through all the jobs
	while( toFinish > 0 )
	{
		// loading ready queue
		for( counter = 0; arrival[counter] <= timeTotal && counter < jobNumber; counter++ )
		{
			// checking to see if the job as already been processed
			if( pService[counter] == 0 )
				continue;
			// finding first empty spot in readyQueue
			readyQueue[next] = counter;
			next++;
		}
		next = 0;
	
		// nothing on ready queue
		if( readyQueue[process] < 0 ){
			timeTotal++;
			continue;
		}
		minProcess = readyQueue[process];
		// finding minService from readyQueue
		while( readyQueue[process] >= 0 )
		{
			tempMin = readyQueue[process + 1];
			
			// shortest process has been found
			if( tempMin == -1 )
				break;
			else if ( pService[tempMin] < pService[minProcess] ){
				minProcess = tempMin;
				process++;
			}
			else
				process++;
		}
		process = 0;

		// calculating time
		waitTime = timeTotal - arrival[minProcess];
		turnAroundTime = waitTime + pService[minProcess];
		timeTotal = timeTotal + pService[minProcess];
		totalWait += waitTime;
		totalTurnAround += turnAroundTime;
		pService[minProcess] = 0;	// zeroing out it's service time because it's done
		
		// printing out info
		Print( minProcess + 1, waitTime, turnAroundTime, PRINT_NORMAL );
		
		// resetting the ready que
		for( counter = 0; counter < jobNumber; counter++ )
			readyQueue[counter] = -1;
		
		toFinish--;
	}// end of while
	
	aveWait = ( totalWait / jobNumber );
	aveTurnAround = ( totalTurnAround / jobNumber );
		
	Print( 0, aveWait, aveTurnAround, PRINT_FOOTER );
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
	double totalWaitTime = 0; 	// Total wait time for all the jobs
	double totalTurnaroundTime = 0; // Total turnaround time for all the jobs
	double avgWaitTime = 0; 	// The average wait time
	double avgTurnaroundTime = 0; // The average turnaround time

	int arrTimeLeft[jobNum]; // The time left for a specific job
	int arrWaitTime[jobNum]; // The wait time for a specific job
	memcpy(arrTimeLeft, arrService, jobNum * sizeof(int)); // Copy the service time to our array

	// Print the function that is running
	printf("%s\n", "Shortest Remaining Time Next");
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
				curTurnaroundTime = currentTime - curArrivalTime;
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

/**********************************************************************
* Func: RoundRobin()
* Desc: Scheduling algorithm where each process is given a certain 
*       amount of time before switching to the next process
* Param: int array of processes, int array of their wait times,
*        int array of their expected time to completion, and an int 
*		 for the number of processes
* Return: None
********************************************************************/
void RoundRobin( const int *pid, const int *arrival, const int *service, int jobNumber )
{
	int pService[jobNumber];
	int priority[jobNumber];	// used to determine if a process has run before
	double contextSwitch = 0;
	double timeTotal = 0.0;
	double waitTime = 0.0;
	double turnAroundTime = 0.0;
	int toProcessNext[jobNumber];	// array to hold queue processes
	int stillRunning = 0;	// keeps track if the process still needs to run
	double completionTime[jobNumber];	// keeps track of how long a process takes to complete
	double processWaitTime[jobNumber];	// keepts track of how long each process waits time is
	int toProcess = 0;		// the next process ready to run
	int next;
	int nextRunning = 0;
	int counter;
	int timeSlice = 4;
	int toFinish = jobNumber;	// remaining processes
	int inQueue;
	int switching = 0;	// used to determine if there is a price for context switching
	double aveWait = 0;
	double aveTurnAround = 0;

	// loop until all jobs are finished
	while( switching < 2 )
	{
		// copying service into pService for editing
		memcpy( pService, service, jobNumber * sizeof(int) );
		
		// initializing  arrays
		for( counter = 0; counter < jobNumber; counter++ ){
			priority[counter] = 0;
			toProcessNext[counter] = -1;
			completionTime[counter] = 0;
			processWaitTime[counter] = 0;
		}
		
		if( switching == 0 ){
			contextSwitch = 0;
			// No contextSwitching
			printf("Round Robin, no context switching\n");
		}
		else{
			printf("Round Robin, with context switching\n");
			contextSwitch = .4;
		}
		Print(0,0,0, PRINT_HEADER);

		while( toFinish != 0 )
		{

			// get all the processes that have arrived
			// or have yet to be completed
			// and store them in toProcessNext[]
			for( next = 0; ( arrival[next] <= timeTotal ) && ( next < jobNumber ); next++ ){	
				if( priority[next] != 0 )
					continue;
			
				if( pService[next] == 0 )
					continue;
			
				// already in queue
				inQueue = 0;
				while( toProcessNext[nextRunning] >= 0 ){
					if( toProcessNext[nextRunning] == next ){
						nextRunning = 0;
						inQueue = 1;
						break;
					}
					nextRunning++;
				}
				if( inQueue )
					continue;
				toProcessNext[nextRunning] = next;
			}

			// check to see if there are any processes that 
			// haven't been completed yet
			next = 0;
			if( stillRunning > 0 ){
				while( toProcessNext[next] != -1 )
					next++;
				toProcessNext[next] = stillRunning - 1;
				stillRunning = 0;
			}
			next = 0;
			// if no processes are ready
			if( toProcessNext[next] < 0 ){
				timeTotal++;
				continue;
			}

			toProcess = toProcessNext[next];
			// checking to see if the process has been run before and setting the wait time
			if( priority[toProcess] == 0 )
				processWaitTime[toProcess] = timeTotal - arrival[toProcess];

			// check to see if the process will finish in the timeSlice
			if( pService[toProcess] - timeSlice <= 0 ){
				timeTotal = timeTotal + pService[toProcess];
				completionTime[toProcess] += pService[toProcess];
				turnAroundTime = processWaitTime[toProcess] + completionTime[toProcess];

				// remove and shift processes up in toProcessNext[]
				while( toProcessNext[next] >= 0 ){
					if( toProcessNext[next + 1] != -1 && priority[toProcessNext[next + 1]] != 0 )
						completionTime[toProcessNext[next + 1]] += pService[toProcess];
					toProcessNext[next] = toProcessNext[next + 1];
					next++;
				}
				next = 0;	// resetting next
				timeTotal += contextSwitch;

				// setting process service time to zero because it's finished running
				pService[toProcess] = 0;
				priority[toProcess] = 1;
				nextRunning = 0;
				toFinish = toFinish - 1;
				Print( pid[toProcess], processWaitTime[toProcess], turnAroundTime, PRINT_NORMAL ); 
			}
			else {
				timeTotal = timeTotal + timeSlice;
				pService[toProcess] = pService[toProcess] - timeSlice;
				// check to see if priority flag has been set
				if( priority[toProcess] == 0 )
					priority[toProcess] = 1;

				// incrementing the time to complete
				completionTime[toProcess] = completionTime[toProcess] + timeSlice;
				stillRunning = pid[toProcess];
				nextRunning = 0;
				// removing and shifting processes up in toProcessNext[]
				while( toProcessNext[next] >= 0 ){
					if( toProcessNext[next + 1] != -1 && priority[toProcessNext[next + 1]] == 1 )
						completionTime[toProcessNext[next + 1]] += timeSlice;
					toProcessNext[next] = toProcessNext[next + 1];
					next++;
				}
				timeTotal += contextSwitch;
				next = 0;	// resetting next
			}
		}// end of while
		
		// resetting values
		waitTime = 0.0;
		turnAroundTime = 0.0;

		for( counter = 0; counter < jobNumber; counter ++){
			waitTime += processWaitTime[counter];
			turnAroundTime += processWaitTime[counter] + completionTime[counter];
		}
		
		aveWait = ( waitTime / jobNumber );
		aveTurnAround = ( turnAroundTime / jobNumber );

		Print( 0, aveWait, aveTurnAround, PRINT_FOOTER );

		toFinish = jobNumber;
		timeTotal = 0.0;	// setting timer back to 0
		switching++;	// changing to context switching						
	}
}

/*
** Print

* Prints the job PID, how much time he waited to
	start, and the turnaround time.

* int pid: The PID of the job.
* double waitTime: How long the job was in the read queue.
* double turnaroundTime: Total time from first ready to done running.
* int header: Whether to print the header, footer, or just data.
*/
void Print(int pid, double waitTime, double turnaroundTime, int header)
{
	char *format = "%-15d %-15.2f %-15.2f \n"; // How to format the printing
	char *headerFormat = "%-15s %-15s %-15s \n"; 
	char *footerFormat = "\n%-20s %-20s \n";

	if(header == PRINT_HEADER)
		printf(headerFormat, "Process ID", "Wait Time", "Turnaround Time");
	else if(header == PRINT_FOOTER)
	{
		printf(footerFormat, "Average Wait Time", "Average Turnaround Time");
		printf("%-20.2f %-20.2f \n\n\n", waitTime, turnaroundTime);
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
