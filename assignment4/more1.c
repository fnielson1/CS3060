/* 
* Frank Nielson
* CS 3060
* Assignment 4
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define HAS_FILENAME (1)
#define MAX_FILEPATH_LEN (256)
#define NUM_OF_LINES (23)
#define DECIMAL_TO_PERCENT (100.0)

void ReadFile(const char*);
void ReadStdin(FILE*);
double Percent(double, double);
int DisplayOne(FILE*);
int Display(FILE*);
int BytesInStr(size_t);

int main(int argc, char** argv)
{
    // Determine whether the stream will be stdin or from a file
    if(argc > HAS_FILENAME)
        ReadFile(argv[1]); // Filename must be here
    else
        ReadStdin(stdin);
    return 0;
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

    // TODO: Display the first 23 lines
	while(1)
	{
		bytesDisplayed = Display(fp);
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
	}

    // Function()
    // Using the total file size and the number of lines shown (get
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
