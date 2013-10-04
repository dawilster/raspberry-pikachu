#include "twitterFacade.h"
#include "twitterFetch.h"
int main( int argc, char* argv[] )
{

	std::string json, name, tweet;
	std::ifstream jsonIn;
	std::size_t found = 0;
	std::size_t end=0;

    twitterFetch *twitter = new twitterFetch();
    twitter->search();

	jsonIn.open("currentTweet.txt", std::ios::in);
	getline(jsonIn, json); 

	found = json.find("text");
	found = found +7;

	end = json.find("\",", found);
	end = end - found;
	tweet = json.substr(found, end);


	found = json.find("\"name\"");
	found = found +7;

	end = json.find("\",", found);
	end = end - found;
	name = json.substr(found, end);


	std::cout<< "name: " << name << "\ntweet: " << tweet << "\n";

    return 0;

}


