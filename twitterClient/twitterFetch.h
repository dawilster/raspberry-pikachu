
#include "twitterClient.h"
class twitterFetch
{
	twitCurl twitterObj;
    std::string replyMsg;
public:
	twitterFetch();
	void authenticate(twitCurl &twitterObj, std::string replyMsg);
	std::string search();
	std::string *getReplyMsg();
	twitCurl *getTwitterObj();
};