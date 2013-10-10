#include <iostream>
#include <ctime>
#include <cstring>

#include "messenger.h"
#include "comm.h"
#include "twitterFetch.h"

extern "C"
{
	#include <unistd.h>
	#include <pthread.h>
}

#define INITIAL_DELAY 3
#define BASE_DELAY 1
#define DELAY_PER_CHAR 0.30
// previous speed 0.21


using namespace std;

pthread_mutex_t messenger_mutex = PTHREAD_MUTEX_INITIALIZER;

void *tweet_thread( void *messenger );
void *display_thread( void *messenger );

int main( void )
{
	pthread_t th;
	pthread_t th2;

	cout << "Creating Messenger..." << endl;
	Messenger messenger( 5, 5 );

	cout << "Creating Threads..." << endl;

	int ret = pthread_create(
			&th,
			NULL,
			tweet_thread,
			reinterpret_cast< void * >( &messenger )
			);
	if( ret != 0 )
	{
		cout << "Error creating thread" << endl;
		return 1;
	}

	ret = pthread_create(
			&th2,
			NULL,
			display_thread,
			reinterpret_cast< void * >( &messenger )
			);
	if( ret != 0 )
	{
		cout << "Error creating thread" << endl;
		return 1;
	}


	cout << "Thread started..." << endl;

	sleep( 5 );

	string input;

	while( 1 )
	{
		if( !messenger.IsRunning() )
		{
			cout << "An error occured..." << endl;
			break;
		}

		cout << "1. Status" << endl;
		cout << "0. Exit" << endl << endl;
	
		getline( cin, input );

		if( input == "status" || input == "1" )
		{
			cout << "Status: " << endl;
			cout << "Currently displaying string: " << messenger.GetCurrentlyDisplayed() << endl;
			cout << "Number of strings in queue: " << messenger.GetNumStrings() << endl;
			cout << "Delay between displaying tweets: " << messenger.GetDelay() << " seconds" << endl;
			cout << "Max queue size: " << messenger.GetMaxQueueSize() << endl;
		}
		else if( input == "exit" || input == "0" )
		{
			cout << "Exiting" << endl;
			break;
		}
		else
		{
			cout << "Unrecognised input, please select an option from the menu" << endl;
		}
		cout << endl;
	}

	cout << "Telling threads to stop" << endl;
	pthread_mutex_lock( &messenger_mutex );
	messenger.StopRunning();
	pthread_mutex_unlock( &messenger_mutex );

	cout << "Wating to join..." << endl;
	pthread_join( th, NULL );
	pthread_join( th2, NULL );

	cout << "Joined... exiting" << endl;

	return 0;
}


void *tweet_thread( void *messenger )
{
	cout << "Tweet thread started..." << endl;
	Messenger *mes = reinterpret_cast< Messenger* >( messenger );

	twitterFetch *twitter = new twitterFetch();
	Tweet tweet;

	while( 1 )
	{
		//cout << "Tweet: Waiting for mutex" << endl;
		pthread_mutex_lock( &messenger_mutex );
		if( !mes->IsRunning() )
		 {	
			 pthread_mutex_unlock( &messenger_mutex );
			 break;
		 }
		
		//cout << "Tweet: Got mutex. Getting tweet" << endl;
		tweet = twitter->search();
		//cout << "Tweet: still here" << endl;
		mes->AddString( tweet.getTweet() );	
		//cout << "Tweet: Added Tweet" << endl;

		//cout << "Tweet: Releasing mutex" << endl;
		pthread_mutex_unlock( &messenger_mutex );
		sleep( mes->GetDelay() );
	}
	
	delete twitter;

	cout << "Tweet: Thread exiting" << endl;
	pthread_exit( NULL );
}

/*
 * Display thread
 * Sends strings that are stored in Messenger to the arduino to display
 * the time between strings being sent are controlled by delays by checking
 * the time between the current time and the last time a string was sent.
 * This way the thread can more quickly react to messages, such as exit.
 * and still control the time between messages being sent to the arduino
 */
void *display_thread( void *messenger )
{
	cout << "Display thread started..." << endl;
	Messenger *mes = reinterpret_cast< Messenger * >( messenger );
	const int baseDelay = mes->GetDelay();
	int displayDelay = 0;

	bool display = true;
	time_t displayTime = time( NULL );
	time_t currentTime = time( NULL );

	Serial serial;
	if( !serial.Initialise( "/dev/ttyACM0" ) )
	{
		cout << "Display: ERROR: Unable to open port" << endl;
		mes->StopRunning();
		pthread_exit( NULL );
	}
	while( 1 )
	{
			//cout << "Display: Waitign for mutex" << endl;
			pthread_mutex_lock( &messenger_mutex );

			//cout << "Display: Got mutex." << endl;

			//cout << "Display: Checking if thread should exit" << endl;
			if( !mes->IsRunning() )
			{
					pthread_mutex_unlock( &messenger_mutex );
					break;
			}

			//cout << "Display: Checking delay time has elapsed" << endl;
			currentTime = time( NULL );
			if( difftime( currentTime, displayTime ) > ( baseDelay + displayDelay ) || display )
			{
					display = false;

					//cout << "Display: Checking for string to display" << endl;
					if( mes->GetNumStrings() == 0 )
					{
							//cout << "Display: No string to display" << endl;
					}
					else if( mes->GetNumStrings() == 1 )
					{
							displayDelay = BASE_DELAY + DELAY_PER_CHAR * mes->GetNextString().length();
							mes->SetCurrentlyDisplayed( mes->GetNextString() );
							serial.Send( mes->GetNextString().c_str() );
							//cout << "Display: Sending string" << endl;
							displayTime = time( NULL );
					}
					else
					{
							displayDelay = BASE_DELAY + DELAY_PER_CHAR * mes->GetNextString().length();
							mes->SetCurrentlyDisplayed( mes->GetNextString() );
							serial.Send( mes->GetNextString( true ).c_str() );
							//cout << "Display: Sending string" << endl;
							displayTime = time( NULL );
					}
			}

			//cout << "Display: Releasing mutex" << endl;
			pthread_mutex_unlock( &messenger_mutex );

			sleep( 1 );
	}

	cout << "Display: thread exiting" << endl;
	pthread_exit( NULL );
}
