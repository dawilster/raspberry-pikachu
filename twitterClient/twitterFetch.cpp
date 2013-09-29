#include "twitterFetch.h"
void printUsage()
{
    printf( "\nUsage:\ntwitterClient -u username -p password\n" );
}

void authenticate()
{
    /* Get username and password from command line args */
    std::string userName( "RaspPikachu" );
    std::string passWord( "kenzo123" );
    // if( argc > 4 )
    // {
    //     for( int i = 1; i < argc; i += 2 )
    //     {
    //         if( 0 == strncmp( argv[i], "-u", strlen("-u") ) )
    //         {
    //             userName = argv[i+1];
    //         }
    //         else if( 0 == strncmp( argv[i], "-p", strlen("-p") ) )
    //         {
    //             passWord = argv[i+1];
    //         }
    //     }
    //     if( ( 0 == userName.length() ) || ( 0 == passWord.length() ) )
    //     {
    //         printUsage();
    //         return 0;
    //     }
    // }
    // else
    // {
    //     printUsage();
    //     return 0;
    // }

    std::string tmpStr, tmpStr2;
    char tmpBuf[1024];

    /* Set twitter username and password */
    self.twitterObj.setTwitterUsername( userName );
    self.twitterObj.setTwitterPassword( passWord );

    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    self.twitterObj.getOAuth().setConsumerKey( std::string( "vlC5S1NCMHHg8mD1ghPRkA" ) );
    self.twitterObj.getOAuth().setConsumerSecret( std::string( "3w4cIrHyI3IYUZW5O2ppcFXmsACDaENzFdLIKmEU84" ) );

    /* Step 1: Check if we alredy have OAuth access token from a previous run */
    std::string myOAuthAccessTokenKey("");
    std::string myOAuthAccessTokenSecret("");
    std::ifstream oAuthTokenKeyIn;
    std::ifstream oAuthTokenSecretIn; 

    oAuthTokenKeyIn.open( "twitterClient_token_key.txt" );
    oAuthTokenSecretIn.open( "twitterClient_token_secret.txt" );

    memset( tmpBuf, 0, 1024 );
    oAuthTokenKeyIn >> tmpBuf;
    myOAuthAccessTokenKey = tmpBuf;

    memset( tmpBuf, 0, 1024 );
    oAuthTokenSecretIn >> tmpBuf;
    myOAuthAccessTokenSecret = tmpBuf;

    oAuthTokenKeyIn.close();
    oAuthTokenSecretIn.close();

    if( myOAuthAccessTokenKey.size() && myOAuthAccessTokenSecret.size() )
    {
        /* If we already have these keys, then no need to go through auth again */
        printf( "\nUsing:\nKey: %s\nSecret: %s\n\n", myOAuthAccessTokenKey.c_str(), myOAuthAccessTokenSecret.c_str() );

        self.twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        self.twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    }
    else
    {
        /* Step 2: Get request token key and secret */
        std::string authUrl;
        self.twitterObj.oAuthRequestToken( authUrl );

        /* Step 3: Get PIN  */
        //edit out twitter pin fetching
        memset( tmpBuf, 0, 1024 );
        printf( "\nDo you want to visit twitter.com for PIN (0 for no; 1 for yes): " );
        gets( tmpBuf );
        tmpStr = tmpBuf;
        if( std::string::npos != tmpStr.find( "1" ) )
        {
            /* Ask user to visit twitter.com auth page and get PIN */
            memset( tmpBuf, 0, 1024 );
            printf( "\nPlease visit this link in web browser and authorize this application:\n%s", authUrl.c_str() );
            printf( "\nEnter the PIN provided by twitter: " );
            gets( tmpBuf );
            tmpStr = tmpBuf;
            self.twitterObj.getOAuth().setOAuthPin( tmpStr );
        }
        else
        {
            /* Else, pass auth url to twitCurl and get it via twitCurl PIN handling */
            self.twitterObj.oAuthHandlePIN( authUrl );
        }

        /* Step 4: Exchange request token with access token */
        self.twitterObj.oAuthAccessToken();

        /* Step 5: Now, save this access token key and secret for future use without PIN */
        self.twitterObj.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
        self.twitterObj.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

        /* Step 6: Save these keys in a file or wherever */
        std::ofstream oAuthTokenKeyOut;
        std::ofstream oAuthTokenSecretOut;

        oAuthTokenKeyOut.open( "twitterClient_token_key.txt" );
        oAuthTokenSecretOut.open( "twitterClient_token_secret.txt" );

        oAuthTokenKeyOut.clear();
        oAuthTokenSecretOut.clear();

        oAuthTokenKeyOut << myOAuthAccessTokenKey.c_str();
        oAuthTokenSecretOut << myOAuthAccessTokenSecret.c_str();

        oAuthTokenKeyOut.close();
        oAuthTokenSecretOut.close();
    }
    /* OAuth flow ends */

    /* Account credentials verification */
    if( self.twitterObj.accountVerifyCredGet() )
    {
        self.twitterObj.getLastWebResponse( replyMsg );
        //printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet web response:\n%s\n", replyMsg.c_str() );
    }
    else
    {
        self.twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet error:\n%s\n", replyMsg.c_str() );
    }
}

int main( int argc, char* argv[] )
{
    authenticate();

    if( self.twitterObj.search( "rmit", "1" ) )
    {
        self.twitterObj.getLastWebResponse( replyMsg );
        //replymsg is where the json is stored
        printf( "\ntwitterClient:: twitCurl::search web response:\n%s\n", replyMsg.c_str() );

        //going to need to pass the json and create an instance of twitter object
        //then store a pointer to each twitter object in a vector pointer array
    }
    else
    {
        self.twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::search error:\n%s\n", replyMsg.c_str() );
    }

    return 0;
}
