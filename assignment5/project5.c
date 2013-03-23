/*
* Authors:
* Frank Nielson
* Caleb Kruger

* Class: CS 3060
* Project: Assignment 5

* This code has be written by the authors
* listed above.
*/

#include <stdio.h>	// stdin, etc
#include <stdlib.h>	// exit()
#include <string.h>	// strlen()
#include <signal.h>	// sigaction and sigset_t
#include <unistd.h> // write()

#define SLEEPTIME (3)
#define ERR_SIGUSR1 ("_sigReceived != 0: SIGUSR1")
#define ERR_SIGUSR2 ("_sigReceived != 0: SIGUSR2")

void Child();
void Parent();
void SetupHandler(int, void(*)(int));

// Signal handlers
void OnCtrlC(int);
void Child_onUsr1(int);
void Child_onUsr2(int);
void Parent_onUsr1(int);
void Parent_onUsr2(int);

/*
* Set to '1' while in signal handler to avoid race conditions with
	sigsuspend(). Right after sigsuspend(), reset to '0'. 
*/
static volatile sig_atomic_t _sigReceived = 0;

// Global Vars
pid_t _forkPid = -1;


/**/
int main()
{
	// print out student info here

	sigset_t maskAll; // All bits are set
	
	// Block all signals (handlers aren't set up yet)
	sigfillset(&maskAll);
	sigprocmask(SIG_BLOCK, &maskAll, NULL);

	SetupHandler(SIGINT, OnCtrlC); // Handle SIGINT

	// Start 
	_forkPid = fork(); // Returns once for the child and for parent process
	if(_forkPid >= 0) // Fork was successful
	{
		// Check if we are a child process
		if(_forkPid == 0)
		{
			// fork() always returns 0 in the child process
			Child();
		}
		else
		{
			// We are the parent process
			Parent();
		}
	}

	// END
	return 0;
}

/*
** SetupHandler(int, void*)

* Sets a handler for the given signal using 
	a pointer to a function.

* int signum: The signal to have the given function handle.
* void *funcPtr: A pointer to the function that will handle the signal.
*/
void SetupHandler(int signum, void (*funcPtr)(int))
{
	struct sigaction act;
	sigset_t signalMask;

	sigfillset(&act.sa_mask); // Block all signals while in handler
	act.sa_flags = SA_RESTART; // Restart system calls
	act.sa_handler = funcPtr;
	if(sigaction(signum, &act, NULL) != 0)
	{
		fprintf(stderr, "%s%d\n", "Couldn't setup handler for: ", signum);
		perror("");
		exit(1);
	}
	// Now that we have a handler for the signal, we'll unblock it
	sigemptyset(&signalMask); // Empty the set
	sigaddset(&signalMask, signum); // Add the signal to unblock
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL); // Unblock 
}

/*
** OnCtrlC(int)

* Handles the SIGINT event. Calls kill on the child
	and then on the parent.

* int signum: The signal's id number.
*/
void OnCtrlC(int signum)
{
	char *childMsg = "Child - Ctrl-c\n";
	char *parentMsg = "Parent - Ctrl-c\n";

	if(_forkPid == 0)
		write(STDERR_FILENO, childMsg, strlen(childMsg));
	else
		write(STDERR_FILENO, parentMsg, strlen(parentMsg));
}

/*
** Error(const char*)

* When an error occurs, call this to print the 
	message desired and exit.

* const char *msg: The message to print.
*/
void Error(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}


/** Parent Functions **/

/*********************************************************
*
*	Func: void Parent()
*	Desc: handles signals to and from a child process
*	Param: None
*	Return: None
*
*********************************************************/
void Parent()
{
	// just letting everyone know
	printf("%s", "Parent - Child process has been created.\n");

	// giving the child some time to grow
	sleep(SLEEPTIME);

	// setting up sigsets
	sigset_t maskAllUsr1;
	sigset_t maskAllUsr2;

	// blocking all signals and then unblocking SIGUSR1 and SIGUSR2
	sigfillset( &maskAllUsr1 );
	sigfillset( &maskAllUsr2 );
	sigdelset( &maskAllUsr1, SIGUSR1 );
	sigdelset( &maskAllUsr2, SIGUSR2 );

	// setting up signal handlers
	SetupHandler( SIGUSR1, Parent_onUsr1 );
	SetupHandler( SIGUSR2, Parent_onUsr2 );

	// sedning SIGUSR1 to child
	printf("%s", "Parent - Start the child working.\n");
	kill(_forkPid, SIGUSR1);
	if( _sigReceived == 0 )
		sigsuspend(&maskAllUsr1);
	else
		Error(ERR_SIGUSR1);
	
	_sigReceived = 0; 	// resetting 
	sleep(SLEEPTIME);

	// sending SIGUSR2 to child
	printf("%s", "Parent - Sending SIGUSR2...\n");
	kill(_forkPid, SIGUSR2);
	if( _sigReceived == 0 )
		sigsuspend( &maskAllUsr2 );
	else
		Error(ERR_SIGUSR2);

	_sigReceived = 0;

	printf("%s", "Parent - Terminating...\n");
}

/************************************************************
*
*	Func: Parent_onUsr1
*	Desc: Handles any SIGUSR1 signals sent to this process
*	Param: The signal 
*	Return: None
*
*************************************************************/
void Parent_onUsr1( int sig )
{
	_sigReceived = 1;
	
	char *msg = "**** Parent SIGUSR1 handler - handler - Recieved a 'task started' signal from child ****\n";

	write( STDERR_FILENO, msg, strlen(msg) );
}

/***************************************************************
*
*	Func: Parent_onUsr2
*	Desc: Handles any SIGUSR2 signals sent to this process
*	Param: The signal
*	Return: None
*
***************************************************************/
void Parent_onUsr2( int sig )
{
	_sigReceived = 1;

	char *msg = "**** Parent SIGUSR2 handler - Received a 'task completed' signal from the child ****\n";

	write( STDERR_FILENO, msg, strlen(msg) );
}

/** Child functions **/


/*
** void Child()

* This is the function that is called by the child.
	It is the "main" for the child process. It handles
	setting up the signal handlers and unblocking
	the signals after the handlers are set up.
*/
void Child()
{
	sigset_t maskAllUsr1;
	sigset_t maskAllUsr2;

	// Set up masks to block all but the two signals
	sigfillset(&maskAllUsr1);
	sigfillset(&maskAllUsr2);
	sigdelset(&maskAllUsr1, SIGUSR1);
	sigdelset(&maskAllUsr2, SIGUSR2);

	// Set up signal handlers
	// SIGUSR1
	SetupHandler(SIGUSR1, Child_onUsr1);
	// SIGUSR2
	SetupHandler(SIGUSR2, Child_onUsr2);

	// Print that child is running
	printf("\n%s\n\n", "Child - Child is running.\nWaiting for 'task start' signal from parent.");

	// sigsuspend() to wait for SIGUSR1 from parent
	if(_sigReceived == 0)
		sigsuspend(&maskAllUsr1);
	else
		Error(ERR_SIGUSR1);
	_sigReceived = 0; // Reset as the signal is over

	// Print message and notify parent that child is starting
	printf("%s\n", "Child - Notifying parent that I'm starting my task.");
	sleep(SLEEPTIME); // Give parent time to set up for signal
	kill(getppid(), SIGUSR1); // Send signal to parent
	
	// Have child do some work
	int count = 0;
	while(count++ < SLEEPTIME)
	{
		printf("Child - WORKING HARD: %d\n", count);
	}

	// Print that we are done working
	printf("%s\n\n", "Child - Done working! Waiting for SIGUSR2 from parent.");
	// Wait for SIGUSR2
	if(_sigReceived == 0)
		sigsuspend(&maskAllUsr2);
	else
		Error(ERR_SIGUSR2);
	_sigReceived = 0; // Reset

	// Print that we're gonna tell parent we are done with work
	printf("%s\n\n", "Child - Notifying parent that our work is done.");
	kill(getppid(), SIGUSR2);
	printf("%s\n\n", "Child - Termination is imminent!"); // Print we're done
}

/*
** Child_onUsr1(int)

* The handler for the child when it receives a 
	SIGUSR1 signal. 

* int signum: The signal's id number.
*/
void Child_onUsr1(int signum)
{
	_sigReceived = 1; // Mark that we are in a signal handler

	char *msg = "**** Child SIGUSR1 handler - Received a 'task start' signal from the parent process ****\n";
	write(STDERR_FILENO, msg, strlen(msg));
}

/*
** Child_onUsr2(int)

* The handler for the child when it receives a 
	SIGUSR2 signal.

* int signum: The signal's id number.
*/
void Child_onUsr2(int signum)
{
	_sigReceived = 1; // Mark that we are in a signal handler

	char *msg = "**** Child SIGUSR2 handler - Received a 'task complete verification' signal from the parent ****\n";
	write(STDERR_FILENO, msg, strlen(msg));
}
