#include <string>

#include "twitterClient.h"
#include "Tweet.h"
class twitterFetch
{
	twitCurl twitterObj;
    std::string replyMsg;
public:
	twitterFetch();
	void authenticate(twitCurl &twitterObj, std::string replyMsg);
	Tweet search();
	std::string getReplyMsg();
	twitCurl getTwitterObj();
	std::string getPublicTimeline();
	Tweet parseTweet();

	void GetLastResponse( std::string& str );
};
