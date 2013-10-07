// Functions for serial communication with arduino
// Using tutorial thing from http://chrisheydrick.com/2012/06/17/how-to-read-serial-data-from-an-arduino-in-linux-with-c-part-3/
// Can also reference http://en.wikibooks.org/wiki/Serial_Programming/termios
#include "comm.h"

Serial::Serial()
{

}

bool Serial::Initialise( char *port )
{
	// initialise connection
	portName = port;
	// open file descriptor in non-blocking mode
	fd = open( port, O_RDWR | O_NOCTTY );
	
	if( fd == -1 )
		return false;
	
	// Set up control structure
	// Get currently set options for the tty
	tcgetattr( fd, &toptions );
	
	// --------Set custom options----------
	
	// 9600 baud
    cfsetispeed( &toptions, B9600 );
	cfsetospeed( &toptions, B9600 );
	
	// 8 bits, no parity, no stop bits
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	// no hardware flow control
	toptions.c_cflag &= ~CRTSCTS;
	// Enable reciever, ignore status lines
	toptions.c_cflag |= CREAD | CLOCAL;
	// disable i/o flow control, disable restart characters
	toptions.c_iflag &= ~( IXON | IXOFF | IXANY );
	// disable canonical input, disable echo, disable visually erase chars,
	// disable terminal-generated signals
	toptions.c_iflag &= ~( ICANON | ECHO | ECHOE | ISIG );
	// disable output processing
	toptions.c_oflag &= ~OPOST;
	
	// wait for 6 characters to come in before read returns
	toptions.c_cc[VMIN] = 1;
	// no minimum time to wait before read returns
	toptions.c_cc[VTIME] = 0;
	
	// commit the options
	tcsetattr( fd, TCSANOW, &toptions );
	
	// wait for the arduino to reset
	usleep( 1 * 1000000 );
	// flush anything already in the buffer
	tcflush( fd, TCIFLUSH );
	
	printf( "Port opened\n" );
	return true;
}


bool Serial::PrintInput()
{
	// Takes input then reads and prints 1 line
	// read up to 128 bytes from fd
	int n = read( fd, buf, 128 );
	
	// Only want to read one line at a time
	for( int i = 0; i < 128; i++ )
	{
		if( buf[ i ] == '\n' )
		{
			// printf( "Found n\n" );
			buf[ i ] = '\0';
		}
	}
	
	printf( "%d bytes read\n", n );
	printf( "buffer contains: \n%s\n", buf );
	return true;
}

string Serial::GetInput( const char *prompt, int numExpected )
{
	string s;
	while( true )
	{
 	s = "";
 	char send[2];
 	
 	send[0] = *prompt;
 	send[1] = '\n';
 	
 	printf( "Sending: %c\\n\n", send[0] );
 	
	write( fd, send, 2 );
	int n = read( fd, buf, 64 );
	
	printf( "input\n" );
	
	if( n == -1 )
		return NULL;
	//else
		//printf( "read %d chars\n", n );
	
	for( int i = 0; i < 128; i++ )
	{
		if( buf[ i ] == '\n' )
		{
			// printf( "Found n\n" );
			buf[ i ] = '\0';
		}
		
	}
	// this should just override the previous \0, just covering
	buf[ n - 1 ] = '\0';
	
	s.resize( n );
	
	for( int i = 0; i < n; i++ )
	{
		s.at( i ) = buf[ i ];
	}
	
	printf( "%d bytes read\n", n );
	if( n == numExpected )
	{
		break;
	}
	Flush();
	}
			
	//printf( "buffer contains: \n%s\n", buf );
	return s;
}


void Serial::Flush()
{
	printf( "Flushing\n" );
	read( fd, NULL, 64 );
}


bool Serial::Send( string str )
{
	int numBytes = write( fd, str.c_str(), str.length() );
	
	if( numBytes >= 0 )
	{
		//printf( "sending %d bytes\n", length );
		//return true;
	}
	else
	{
		printf( "Error sending string\n" );
		return false;
	}
	write( fd, "\n", 1 );
	return true;	
}


void Serial::Disconnect()
{
	printf( "Closing port to arduino\n" );
	close( fd );
}
