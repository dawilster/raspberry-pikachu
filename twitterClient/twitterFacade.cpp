#include "twitterFacade.h"
#include "twitterFetch.h"
int main( int argc, char* argv[] )
{
	std::string json;
    twitterFetch *twitter = new twitterFetch();
    json = twitter->searchTerm();

    printf( "\ntwitterClient:: twitCurl::timelineUserGet :\n%s\n", json.c_str() );

    return 0;

}


