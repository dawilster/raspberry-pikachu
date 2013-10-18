#ifndef MESSENGER_H
#define MESSENGER_H

#include <vector>
#include <string>
#include <cstring>
#include <iostream>

enum ThreadState 
{
	Running,
	Sleeping,
	Stopped
};

class Messenger
{
	public:
		// Initailise class
		// maxQueueSize: max number of messages in queue
		// delay: delay between displaying messages
		Messenger( int maxQueueSize, unsigned int delay );

		// Adds a string to the queue 
		// Returns false if queue is full
		bool AddString( std::string, bool allowDuplicates = false );

		// Get number of strings in queue
		int GetNumStrings();

		// Get next string
		// if passed true, string is removed from the list 
		std::string GetNextString( bool remove = false );

		// Remove the first string from the list
		// returns false if list is empty
		bool RemoveString();

		std::string GetCurrentlyDisplayed();
		void SetCurrentlyDisplayed( std::string str );

		int GetMaxQueueSize();
		unsigned int GetDelay();

		// Sets the running flag to false, 
		// this should be checked by thread
		void StopRunning();

		// Tell the tweet and display threads to stop (for exiting)
		void TweetStop();
		void DisplayStop();
		
		// Tell the tweet and display threads to sleep
		// threads should keep checking to see if they should exit
		void TweetSleep();
		void DisplaySleep();

		// When called, restarts sleeping threads.
		// Currently doesn't restart stopped threads
		void TweetStart();
		void DisplayStart();

		bool IsRunning();	
		ThreadState GetTweetState();
		ThreadState GetDisplayState();


	private:
		const int MAX_QUEUE_SIZE_;
		const unsigned int DELAY_;
		

		bool isRunning_;
		ThreadState tweetState_;
		ThreadState displayState_;


		std::vector< std::string > strings_;
		std::string currentlyDisplayed_;

};


#endif
