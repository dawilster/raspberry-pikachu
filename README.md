Twitter Fetch
=================
Took me all day to get it working on Arch Linux for RPi. I've got a barebones working version that just fetches a hardcoded keyword.
Working on a facade at the moment

Implemented some threading to have a menu that isn't blocked by the IO of the tweet and display threads.
It will display and error and exit if the display isn't connected,but it doesn't check for an internet connection.

Twitter client needs the Twitcurl 
https://code.google.com/p/twitcurl/wiki/WikiHowToUseTwitcurlLibrary

Linux/Ubuntu need these installed (if libcurl4-dev doesn't just make sure curl is installed)
```
sudo apt-get install g++ libcurl4-dev subversion
```
To get it working your going to need to compile and import the library
```
svn co http://twitcurl.googlecode.com/svn/trunk/libtwitcurl
```
Build its and installs the lib files to the usr/lib directory (may need root privileges)
```
make install
```
And to compile the application which runs all the code in the twitterFacade
```
cd twitterClient && make all
```
