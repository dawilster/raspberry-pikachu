Twitter demo guide
=================
Took me all day to get it working on Arch Linux for RPi. I'll scrap the demo code and start building my own custom functions tomorrow.


Twitter client needs the Twitcurl 
https://code.google.com/p/twitcurl/wiki/WikiHowToUseTwitcurlLibrary

Linux/Ubuntu need these installed
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
And to compile the demo application use
```
g++ twitterClient.cpp -ltwitcurl
```