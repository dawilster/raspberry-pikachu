#include <iostream>
class Tweet{
	std::string name, tweetContent;

public:
	Tweet();
	Tweet(std::string tweetName, std::string tweetC);
	std::string getName();
	std::string getTweet();
};