// Header file for serial communication between PC and arduino

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string>

using std::string;

class Serial
{
public:
	Serial();
	// Initialise the port for use
	bool Initialise( char *port );
	//Reads in from arduino then prints up to the first \n
	bool PrintInput();
	// prompt arduino for input, response is placed into "response"
	// currently only sends a single byte, followed by a \n
	string GetInput( const char *prompt, int numExpected );
	//send string to arduino
	bool Send( string string );
	// Flush input
	void Flush();
	// Close the port
	void Disconnect();
	
private:
	int fd;
	struct termios toptions;
	char *portName;
	char buf[256];
};


