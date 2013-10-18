#include <iostream>

#ifndef TWEET_H
#define TWEET_H

class Tweet{
	std::string name, tweetContent;

public:
	Tweet();
	Tweet(std::string tweetName, std::string tweetC);
	std::string getName();
	std::string getTweet();
};

#endif
