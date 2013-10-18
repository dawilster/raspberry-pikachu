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

#define SLEEP_DELAY 5

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

	// Setting twitter thread to sleep
	// hopefully this negates the lack of network connection for presentation
	messenger.TweetSleep();

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
	messenger.TweetSleep();

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
	messenger.DisplayStart();


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
		if( messenger.GetTweetState() == Sleeping )
			cout << "2. Start Tweet thread" << endl;
		else if( messenger.GetTweetState() == Stopped )
			//cout << "2. Restart Tweet thread" << endl;
			cout << "2. UNAVAILABLE" << endl;
		else
			cout << "2. Sleep Tweet thread" << endl;
		cout << "3. Display message" << endl;
		cout << "0. Exit" << endl << endl;
	
		getline( cin, input );

		if( input == "status" || input == "1" )
		{
			cout << "Status: " << endl;
			cout << "Currently displaying string: " << messenger.GetCurrentlyDisplayed() << endl;
			cout << "Number of strings in queue: " << messenger.GetNumStrings() << endl;
			cout << "Delay between displaying tweets: " << messenger.GetDelay() << " seconds" << endl;
			cout << "Max queue size: " << messenger.GetMaxQueueSize() << endl;
			cout << "Thread status: ";
			
			cout << " Display - ";
			switch( messenger.GetDisplayState() )
			{
				case Running:
					cout << "Running";
					break;
				case Sleeping:
					cout << "Sleeping";
					break;
				case Stopped:
					cout << "Stopping";
					break;
				default:
					cout << "Status Error";
					break;
			}

			cout << "\n\t\tTweet - ";
			switch( messenger.GetTweetState() )
			{
				case Running:
					cout << "Running";
					break;
				case Sleeping:
					cout << "Sleeping";
					break;
				case Stopped:
					cout << "Stopping";
					break;
				default:
					cout << "Status Error";
					break;
			}
			cout << endl;

		}
		else if( input == "tweet" || input == "2" )
		{
			if( messenger.GetTweetState() == Running )
			{
				messenger.TweetSleep();
				cout << "Tweet Sleeping" << endl;
			}
			else if( messenger.GetTweetState() == Stopped )
			{
				// Do nothing, thread stopped from error
				cout << "An error has occured, restart program to fetch tweets.\n";
				cout << "This is likely caused by a network problem" << endl;
			}
			else
			{
				messenger.TweetStart();	
				cout << "Tweet Started" << endl;
			}
		}
		else if( input == "message" || input == "3" )
		{
			cout << "Enter a string to display.\n> ";
			string str;
			getline( cin, str );
			if( !messenger.AddString( str, true ) )
				cout << "Queue full, unable to add message" << endl;
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
	
	bool running = true;

	string str;
	twitter->GetLastResponse( str );
	if( str != "" )
	{
		cout << "Tweet: Error - thread will exit, restart program for twitter feed" << endl;
		cout << str << endl;
		running = false;
	}
	else
	{
		cout << "Connected" << endl;
	}

	while( running )
	{
		//cout << "Tweet: Waiting for mutex" << endl;
		pthread_mutex_lock( &messenger_mutex );

		switch( mes->GetTweetState() )
		{
			case Running:
				tweet = twitter->search();
				mes->AddString( tweet.getTweet() );
				break;
			
			// while sleeping, the thread still needs to loop
			// checking thread status
			case Sleeping:
				// Do Nothing
				break;

			case Stopped:
				// thread exiting
				running = false;
				break;
		
			default:
				cout << "Tweet: State error, thread exiting." << endl;
				mes->TweetStop();
				running = false;
				break;
		}

		pthread_mutex_unlock( &messenger_mutex );
		sleep( mes->GetDelay() );
	}
	
	delete twitter;

	mes->TweetStop();
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

	bool running = true;

	while( running )
	{
			pthread_mutex_lock( &messenger_mutex );

			switch( mes->GetDisplayState() )
			{
				case Running:
					currentTime = time( NULL );
					if( difftime( currentTime, displayTime ) > ( baseDelay + displayDelay ) || display )
					{
						display = false;
	
						if( mes->GetNumStrings() == 0 )
						{
							//cout << "Display: No string to display" << endl;
							string str_greet = "Hello, display a message and/or turn on the twitter feed";
							displayDelay = BASE_DELAY + DELAY_PER_CHAR * str_greet.length();
							mes->SetCurrentlyDisplayed( str_greet );
							serial.Send( str_greet.c_str() );
							displayTime = time( NULL );
						}
						else if( mes->GetNumStrings() == 1 )
						{
							// Only one string to displapy, get string without removing it from the queue
							displayDelay = BASE_DELAY + DELAY_PER_CHAR * mes->GetNextString().length();
							mes->SetCurrentlyDisplayed( mes->GetNextString() );
							serial.Send( mes->GetNextString().c_str() );
							displayTime = time( NULL );
						}
						else
						{
							// More than one string to display, get string and remove it from queue
							displayDelay = BASE_DELAY + DELAY_PER_CHAR * mes->GetNextString().length();
							mes->SetCurrentlyDisplayed( mes->GetNextString() );
							serial.Send( mes->GetNextString( true ).c_str() );
							displayTime = time( NULL );
						}
					}
					break;

					// while sleeping, the thread still needs to loop
					// checking thread status
				case Sleeping:
					// Do Nothing
					break;

				case Stopped:
					// thread exiting
					running = false;
					break;

				// Shouldn't get here, but ya know...
				default:
					cout << "Display: State error (unhandled state), thread exiting." << endl;
					mes->DisplayStop();
					running = false;
					break;
			}

			pthread_mutex_unlock( &messenger_mutex );

			sleep( 1 );
	}

	cout << "Display: thread exiting" << endl;
	pthread_exit( NULL );
}
