#include "Tweet.h"

Tweet::Tweet(){

}

Tweet::Tweet(std::string tweetName, std::string tweetC){
	name = tweetName;
	tweetContent = tweetC;
}
std::string Tweet::getName(){
	return name;
}
std::string Tweet::getTweet(){
	return tweetContent;
}