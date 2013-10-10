#include "messenger.h"

Messenger::Messenger( int maxQueueSize, unsigned int delay )
: MAX_QUEUE_SIZE_( maxQueueSize ), DELAY_( delay )
{
	isRunning_ = true;
}


bool Messenger::AddString( std::string str, bool allowDuplicates )
{
	if( strings_.size() == MAX_QUEUE_SIZE_ )
	{
		return false;
	}

	// if string is the same as previously added, it is not readded
	if( strings_.size() != 0 )
	{
			if( strcmp( str.c_str(), strings_.back().c_str() ) == 0 && !allowDuplicates )
			{
					//std::cout << "String is the same...Not adding" << std::endl;
					return true;
			}
	}
	strings_.push_back( str );	
	return true;
}


int Messenger::GetNumStrings()
{
	return strings_.size();
}


std::string Messenger::GetNextString( bool remove )
{
	if( remove )
	{
		std::string str = strings_.front();
		strings_.erase( strings_.begin() );
		return str;
	}
	else
	{
		return strings_.front();
	}
}


bool Messenger::RemoveString()
{
	if( strings_.size() == 0 )
	{
		return false;
	}
	else
	{
		strings_.erase( strings_.begin() );
		return true;
	}
}


int Messenger::GetMaxQueueSize()
{
	return MAX_QUEUE_SIZE_;
}


unsigned int Messenger::GetDelay()
{
	return DELAY_;
}


void Messenger::StopRunning()
{
	isRunning_ = false;
}


bool Messenger::IsRunning()
{
	return isRunning_;
}

void Messenger::SetCurrentlyDisplayed( std::string str )
{
	currentlyDisplayed_ = str;	
}

std::string Messenger::GetCurrentlyDisplayed()
{
	return currentlyDisplayed_;
}
