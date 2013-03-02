/* 
* Frank Nielson
* CS 3060
* Assignment 4
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

#define HAS_FILENAME (1)
#define MAX_FILEPATH_LEN (256)
#define NUM_OF_LINES (23)
#define DECIMAL_TO_PERCENT (100.0)
#define STDIN (0)
#define SLEEPTIME (1)

#define ENTER ('\n')
#define SPACE (' ')
#define QUIT ('q')

//
void GetTerminalAttr(struct termios*);
void SetTerminalAttr(struct termios);
void SetupTerminal();

void ReadFile(const char*);
void ReadStdin(FILE*);

double Percent(double, double);
char Prompt();
char GetResponse();
int DisplayOne(FILE*);
int Display(FILE*);
int BytesInStr(size_t);

// GLOBAL VARS
struct termios _origTerm;

/**/
int main(int argc, char** argv)
{
    // Block and setup signal handlers

    // Get the current terminal attributes
    GetTerminalAttr(&_origTerm);
    // Set the attributes so that it is non-blocking and non-echo
    SetupTerminal();    

    // Determine whether the stream will be stdin or from a file
    if(argc > HAS_FILENAME)
        ReadFile(argv[1]); // Filename must be here
    else
        ReadStdin(stdin);

    // Restore the terminal to it's original settings
    SetTerminalAttr(_origTerm);
    return 0;
}

/*
** GetTerminalAttr(struct termios*)

* Get the orignal attributes of stdin and return them.

* termios termAttr: The struct to store the orignal 
    terminal attributes in.
*/
void GetTerminalAttr(struct termios *termAttr)
{
    if(tcgetattr(STDIN, termAttr) != 0)
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
    if(tcsetattr(STDIN, TCSANOW, &termAttr) != 0)
    {
        perror("Set Terminal attributes");
        exit(1);
    }
}

/*
** SetupTerminal()

* Sets up the terminal so that it is non-blocking
    and is non-echo.
*/
void SetupTerminal()
{
    struct termios tmp;
    
    // Turn off echo
    GetTerminalAttr(&tmp);
    tmp.c_lflag &= ~ECHO;
    SetTerminalAttr(tmp);

    // Turn off blocking
    fcntl(STDIN, F_SETFL, O_NONBLOCK);
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
** void ReadStdin(FILE*)

* Reads input from *in (should be stdin)
    one (char?) at a time. It then displays what has
    been read (up to 23 lines) and how many bytes have
    been displayed.

* const FILE *fs: A pointer to stdin
*/
void ReadStdin(FILE *fs)
{

    /* Printing for stdin */
    // Read one (char?) at a time, display the line,
    // and each time incrementing the number of bytes displayed

    // Function()
    // Print 23 lines of the file plus the percentage displayed
}

/*
** void ReadFile(char*)

* Reads input from the given file one line at a time.
    It then displayes what has been read (up to 23 lines)
    and the percentage of the file that has been displayed.

* char* filename: The name of the file
*/

void ReadFile(const char *filename)
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
        input = Prompt();
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
** Prompt()

* Blocks and prompts the user for input.
    It then returns the character that
    the user entered.

* Returns: The character that the user entered.
*/
char Prompt()
{
    char input;
    
    printf("%s ", 
        "\033[7mPress 'ENTER' or 'SPACE' to continue. 'q' to quit:\033[m");
    fflush(stdout); // Print out the prompt
    while(1)
    {
        sleep(SLEEPTIME);
        input = tolower(GetResponse());
        if(input == EOF)
            continue; // User didn't enter anything
        return input; // We already know input is good
    }
}

/*
** GetResponse()

* Keeps waiting until the user enters a valid character
    or the user enters nothing (EOF).

* Returns: A char of what the user entered.
*/
char GetResponse()
{ 
    int input;
    char commands[3];
    
    ToggleICanon(); // Turn canonical mode off (no buffering)
    // Put all the available commands in a string
    sprintf(commands, "%c%c%c", ENTER, SPACE, QUIT);

    while((input = getchar()) != EOF)
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
