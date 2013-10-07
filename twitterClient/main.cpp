#include <unistd.h>
#include <iostream>
#include <string>

#include "comm.h"
#include "twitterFetch.h"

#define PORT "/dev/ttyACM0"
#define INITIAL_DELAY 3
#define BASE_DELAY 1
#define DELAY_PER_CHAR 0.25
// previous speed 0.21

using namespace std;

int main( void )
{
	Serial serial;

	if( !serial.Initialise( PORT ) )
	{
		cout << "Failed to connect to port\n";
		return EXIT_FAILURE;
	}
	else
	{
		cout << "Connected over port " << PORT << endl;
		usleep( INITIAL_DELAY * 1000000 );
	}
	cout << "-\n";
	string str = "Connected!";
	if(	!serial.Send( str ) )
	{
		cout << "failed to send\n";
	}
	
	sleep( 10 );
	int times = 0;
	while ( times < 10 ){

    twitterFetch *twitter = new twitterFetch();
    Tweet tweet = twitter->search();

	//str = "This about the size of a tweet that can be displayed,";
	//str.append( "they have a 140 character limit. And these can be fetched using the " );
	//str.append( "Twitter API, #RASPI" );
	//cout << str.length() << endl;
	serial.Send( tweet.getTweet() );

	sleep( BASE_DELAY + DELAY_PER_CHAR * tweet.getName().length() );
	serial.Send( tweet.getName() );
	cout << tweet.getName() << endl;
	sleep( 10 );
	times++;
	}
	str = "1";
	serial.Send( str );

	cout << "Exiting\n";
	serial.Disconnect();
	return EXIT_SUCCESS;
}
