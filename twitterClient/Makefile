all: twitterFacade.cpp twitterFetch.cpp Tweet.cpp
	g++ twitterFacade.cpp twitterFetch.cpp Tweet.cpp -ltwitcurl -o twitterFacade

dmd: main.cpp twitterFetch.cpp Tweet.cpp comm.cpp
	g++ main.cpp twitterFetch.cpp Tweet.cpp comm.cpp -ltwitcurl -o displayTwit


clean:
	rm -f twitterFetch
