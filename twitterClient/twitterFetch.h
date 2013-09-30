
#include "twitterClient.h"
class twitterFetch
{
	twitCurl twitterObj;
    std::string replyMsg;
public:
	twitterFetch();
	void authenticate(twitCurl &twitterObj, std::string replyMsg);
	void search();
};