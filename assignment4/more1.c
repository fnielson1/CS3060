/* 
* Authors:
* Frank Nielson
* Caleb Kruger

* Class: CS 3060
* Project: Assignment 4

* This code has be written by the authors
* listed above.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>

#define HAS_FILENAME (1)
#define MAX_FILEPATH_LEN (256)
#define MAX_PROMPT_LEN (100)
#define NUM_OF_LINES (23)
#define DECIMAL_TO_PERCENT (100.0)

#define ENTER ('\n')
#define SPACE (' ')
#define QUIT ('q')

//
void GetTerminalAttr(struct termios*);
void SetTerminalAttr(struct termios);
void SetupTerminal();
void ResetTerminal();

void ReadFile(FILE*, const char*);
void ReadStdin(FILE*, FILE*);

double Percent(double, double);
char Prompt(FILE*);
char GetResponse(FILE*);
void ErasePrompt(int);
int DisplayOne(FILE*);
int Display(FILE*);
int BytesInStr(size_t);
void ctrl_C_Handler(int);

// GLOBAL VARS
struct termios _origTerm;
int fd_tty;
FILE *fp_tty;


/**/
int main(int argc, char** argv)
{
    // Block and setup signal handlers
    struct sigaction sigHandler;
	sigset_t blocked;

	sigHandler.sa_handler = ctrl_C_Handler;
	sigHandler.sa_flags = SA_RESTART;
	
	// blocking all signals, but CTRL-C
	sigfillset( &blocked );
	sigdelset( &blocked, SIGINT );
	sigHandler.sa_mask = blocked;
	
	// if error trying to call signal handler
	if( sigaction(SIGINT, &sigHandler, NULL ) == -1 )
		perror("sigaction error.\n");
	
	// Get a handle on the terminal
    fd_tty = open("/dev/tty", O_RDONLY);
    fp_tty = fdopen(fd_tty, "r");

    // Setup the terminal attributes
    SetupTerminal();

    // Determine whether the stream will be stdin or from a file
    if(argc > HAS_FILENAME)
        ReadFile(fp_tty, argv[1]); // Filename must be here
    else
        ReadStdin(fp_tty, stdin);

    // Restore the terminal to it's original settings
    ResetTerminal();
    return 0;
}

void ctrl_C_Handler(int s)
{
	// testing signal handler
	printf("This is a test\n.");
	// need to reset term settings
	exit(1);
}
/*
** GetTerminalAttr(struct termios*)

* Get the orignal attributes of stdin and return them.

* termios termAttr: The struct to store the orignal 
    terminal attributes in.
*/
void GetTerminalAttr(struct termios *termAttr)
{
    if(tcgetattr(fd_tty, termAttr) != 0)
    {
        perror("Get Terminal attributes");
        exit(1);
    }
}

/*
** SetTerminal(struct termios)

* Sets the attributes of stdin using the argument 
    without waiting.

* termios termAttr: The attributes to the stdin to.
*/
void SetTerminalAttr(struct termios termAttr)
{
    if(tcsetattr(fd_tty, TCSANOW, &termAttr) != 0)
    {
        perror("Set Terminal attributes");
        exit(1);
    }
}

/*
** SetupTerminal()

* Get the original terminal attributes and then
    set the current ones to non-echo.
*/
void SetupTerminal()
{
    struct termios tmp;
    
    // Get the current terminal attributes
    GetTerminalAttr(&_origTerm);

    // Turn off echo
    GetTerminalAttr(&tmp);
    tmp.c_lflag &= ~ECHO;
    SetTerminalAttr(tmp);

    // When the function exits normally, it'll reset the
    // terminal automatically
    atexit(&ResetTerminal);

    // Turn off blocking (REFERENCE ONLY!)
    //fcntl(STDIN, F_SETFL, O_NONBLOCK);
}

/*
** ResetTerminal()

* Resets the terminal attributes to the way they were.
*/
void ResetTerminal()
{
    SetTerminalAttr(_origTerm);
}

/*
** ToggleICanon()

* If canonical mode is on, turn it off and vice versa.
*/
void ToggleICanon()
{
    struct termios tmp;

    GetTerminalAttr(&tmp); // Get current attributes
    if(tmp.c_lflag & ICANON) // Check if ICANON is set
        tmp.c_lflag &= ~ICANON;
    else
        tmp.c_lflag |= ICANON;
    SetTerminalAttr(tmp); // Set the new attribute value
}

/*
** void ReadStdin(FILE*, FILE*)

* Reads input from *fp (should be stdin).
    It then displays what has
    been read (up to 23 lines) and how many bytes have
    been displayed.

* FILE *in: A pointer to the stream to read input from.
* FILE *fp: A pointer to stdin
*/
void ReadStdin(FILE *in, FILE *fp)
{

	// declarations
	int bytesRead = 0;
	int totalBytes = 0;	// bytes read so far
	char input;		// to test user input		

	// calculating total bytes read in so far
	bytesRead = Display(fp);
	
	// loop until end of file or 'q'
	while( 1 ){
		if( bytesRead == 0 )
			break;
		
		totalBytes += bytesRead;
		printf("%d\n", totalBytes);

		// getting input command
		input = Prompt(in);
		
		// print out only one line
		if( input == ENTER )
			bytesRead = DisplayOne(fp);
		
		// print out 23 lines
		else if ( input == SPACE )
			bytesRead = Display( fp );
		
		// quit
		else if ( input == QUIT )
			break;
		
		// if no valid command was entered
		else {
			fprintf(stderr, "Error reading input! CODE: %d\n", input);
			exit(1);
		}
		
	}
	fclose(fp);
}

/*
** void ReadFile(FILE*, char*)

* Reads input from the given file one line at a time.
    It then displayes what has been read (up to 23 lines)
    and the percentage of the file that has been displayed.

* FILE *in: The stream where we'll read input from.
* char *filename: The name of the file.
*/

void ReadFile(FILE* in, const char *filename)
{
    /* Printing for file */
    struct stat st;
    FILE *fp;
    char input;
    double percent = 0;
    int fileSize = 0;
    int bytesDisplayed = 0;
	int totalBytes = 0;
	int firstLoop = 1; // Are we looping through the first time?

    // Get the stat of the file
    if(stat(filename, &st) != 0)
    {
        perror("stat");
        exit(1);
    }
    // Open the file
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        perror("fopen");
        exit(1);
    }

    // Determine the number of bytes in the file
    fileSize = st.st_size;

    // Display 23 lines and then prompt for user
    bytesDisplayed = Display(fp);
	while(1)
	{
		if(bytesDisplayed == 0)
			break;
		totalBytes += bytesDisplayed;
		percent = Percent(totalBytes, fileSize);
		
		// Print the percent (and file name if first time looping)
		if(firstLoop)
		{
			printf("%s %.2f%%\n", filename, percent);
			firstLoop = 0;
		}
		else
			printf("%.2f%%\n", percent);

        // Now wait for user input
        input = Prompt(in);
        if(input == ENTER)
            bytesDisplayed = DisplayOne(fp);
        else if(input == SPACE)
            bytesDisplayed = Display(fp);
        else if(input == QUIT)
            break;
        else
        {
            fprintf(stderr, "%s %d\n", "Error reading input! CODE: ", input);
            exit(1);
        }
	}
    fclose(fp); // Close the file
}

/*
** Prompt(FILE*)

* Blocks and prompts the user for input.
    It then returns the character that
    the user entered.

* FILE *in: A pointer to the stream where we'll 
    read the input.
* Returns: The character that the user entered.
*/
char Prompt(FILE *in)
{
    char prompt[MAX_PROMPT_LEN];
    char input;
    
    sprintf(prompt, "%s ", 
        "\033[7mPress 'ENTER' or 'SPACE' to continue. 'q' to quit:\033[m");
    printf("%s", prompt);
    fflush(stdout); // Print out the prompt
    
    // Get user input 
    while(1)
    {
        input = tolower(GetResponse(in));
        if(input == EOF)
            continue; // User didn't enter anything
        
        // Clear the inverse prompt (so that the user can read easily)
        // Must add 5 because it otherwise won't include the inverse
        // video characters at the end because there is a '\0' before them
        ErasePrompt(strlen(prompt) + 5);
        return input;
    }
}

/*
** GetResponse(FILE*)

* Keeps waiting until the user enters a valid character
    or the user enters nothing (EOF).

* FILE *in: A pointer to the stream where we'll
    read the input.
* Returns: A char of what the user entered.
*/
char GetResponse(FILE *in)
{ 
    int input;
    char commands[4]; // One extra for the null char
    
    ToggleICanon(); // Turn canonical mode off (no buffering)
    // Put all the available commands in a string
    sprintf(commands, "%c%c%c", ENTER, SPACE, QUIT);
    
    while((input = fgetc(in)))
    {
        if(strchr(commands, input) != NULL)
        {
            if(input == QUIT)
                printf("%c\r\n", QUIT); // Show user pressed 'q'
            break;
        }
    }
    ToggleICanon(); // Turn canonical mode on
    return (char)input;
}

/*
** ErasePrompt()

* Erases the inverse video prompt from the screen.
    This allows the user to read the file more easily.

* int length: The length of the prompt string.
*/
void ErasePrompt(int length)
{
    char erasePrompt[MAX_PROMPT_LEN];
    
    // Create enough spaces to erase the prompt
    memset(erasePrompt, ' ', length + 5);

    ToggleICanon(); // Turn off canonical mode
    printf("%c", '\r'); // Return the beginning of the line
    printf("%s", erasePrompt); // Erase the prompt
    printf("%c", '\r'); // Return to the beginning to display next line
    ToggleICanon(); // Turn canonical mode on again
}

/*
** Display(FILE*)

* Displays one line from the stream and returns the number
	of bytes displayed

* FILE *fp: Pointer to the stream
* Returns: The number of bytes displayed (0 if none)
*/
int DisplayOne(FILE* fp)
{
	char line[MAX_FILEPATH_LEN];
	int bytesDisplayed = 0;
	
	if(fgets(line, sizeof(line), fp))
	{
		bytesDisplayed = BytesInStr(strlen(line));
		printf("%s", line);
	}
	else if(ferror(fp))
	{
		perror("DisplayOne");
		exit(1);
	}
	return bytesDisplayed; // EOF
}

/*
** Display(FILE*)

* Calls DisplayOne NUM_OF_LINES times and returns
    the number of bytes displayed

* FILE *fp: The pointer to the file
* Returns: The number of bytes displayed
*/
int Display(FILE* fp)
{
    int totalDisplayed = 0;
    int bytesDisplayed = 0;
    int index = 0;
       
	while(index < NUM_OF_LINES)
	{
        bytesDisplayed = DisplayOne(fp);
        if(bytesDisplayed == 0)
            return totalDisplayed; // EOF
        totalDisplayed += bytesDisplayed;

        index++; 
    }
    return totalDisplayed;
}

/*
** BytesInStr(size_t length)

* Returns the number of bytes of a string 
	of a given length

* size_t length: The length of the string
* Returns: The number of bytes in the string
*/
int BytesInStr(size_t length)
{
	return length * sizeof(char);
}

/*
** Percent(double, double)

* Returns part / total * 100

* double part: The denomiator
* double total: The numerator
* Returns: The percentage of part in total
*/
double Percent(double part, double total)
{
	return (part / total) * DECIMAL_TO_PERCENT;
}
