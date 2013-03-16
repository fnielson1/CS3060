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

void Child();
void Child_onUsr1(int);
void Child_onUsr2(int);


/*
* Set to '1' while in signal handler to avoid race conditions with
	sigsuspend(). Right after sigsuspend(), reset to '0'. 
*/
static volatile sig_atomic_t _sigReceived = 0;

/**/
int main()
{
	sigset_t maskAll; // Has all signals set

	// Block all signals (handlers aren't set up yet)
	sigfillset(&maskAll);
	sigprocmask(SIG_BLOCK, &maskAll, NULL);

	// Check if we are a child process

	// If so, set up signal handlers and goto function
	Child();

	// Else, we are parent; set up different handlers and goto function

	// END
	sigprocmask(SIG_UNBLOCK, &maskAll, NULL); // Unblock all signals
	return 0;
}

/** Parent Functions **/


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
	struct sigaction act;
	sigset_t maskAllUsr1;
	sigset_t maskAllUsr2;

	// Set up masks to block all but the two signals
	sigfillset(&maskAllUsr1);
	sigfillset(&maskAllUsr2);
	sigdelset(&maskAllUsr1, SIGUSR1);
	sigdelset(&maskAllUsr2, SIGUSR2);

	// Set up signal handlers
	sigfillset(&act.sa_mask); // Block all signals while in signal handler
	act.sa_flags = SA_RESTART;

	// SIGUSR1
	act.sa_handler = Child_onUsr1;
	if(sigaction(SIGUSR1, &act, NULL) != 0)
	{
		perror("Couldn't setup signal handler: SIGUSR1");
		exit(1);
	}
	// SIGUSR2
	act.sa_handler = Child_onUsr2;
	if(sigaction(SIGUSR2, &act, NULL) != 0)
	{
		perror("Couldn't setup signal handler: SIGUSR2");
		exit(1);
	}

	//

	// Print that child is running
	printf("%s\n", "Child is running.\nWaiting for 'task start' signal from parent");

	// sigsuspend() to wait for SIGUSR1 from parent
	if(_sigReceived == 0)
		sigsuspend(&maskAllUsr1);
	else
	{
		fprintf(stderr, "%s\n", "_sigReceived != 0: SIGUSR1");
		exit(1);
	}
	_sigReceived = 0; // Reset as the signal is over

	// Print message and notify parent that child is starting
	printf("%s\n", "Notifying parent - Starting task");
	sleep(SLEEPTIME); // Give parent time to set up for signal
	//kill(getppid(), SIGUSR1); // Send signal to parent
	
	// Have child do some work
	int count = 0;
	while(count++ < SLEEPTIME)
	{
		printf("WORKING - %d\n", count);
	}

	// Print that we are done working
	printf("%s\n", "Done working!");
	// Wait for SIGUSR2
	if(_sigReceived == 0)
		sigsuspend(&maskAllUsr2);
	else
	{
		fprintf(stderr, "%s\n", "_sigReceived != 0: SIGUSR1");
		exit(1);
	}
	_sigReceived = 0; // Reset

	// Print that we're gonna tell parent we are done with work
	printf("%s\n", "Notifying parent that our work is done.");
	//kill(getppid(), SIGUSR2);
	printf("%s\n", "Termination is imminent!"); // Print we're done
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
