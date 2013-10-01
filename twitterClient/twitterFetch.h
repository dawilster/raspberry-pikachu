
#include "twitterClient.h"
class twitterFetch
{
	twitCurl twitterObj;
    std::string replyMsg;
public:
	twitterFetch();
	void authenticate(twitCurl &twitterObj, std::string replyMsg);
	std::string searchTerm();
	std::string getReplyMsg();
	twitCurl getTwitterObj();
	std::string getPublicTimeline();
};