#include "twitterFetch.h"
void twitterFetch::authenticate(twitCurl &twitterObj, std::string replyMsg)
{
    /* Get username and password from command line args */
    std::string userName( "RaspPikachu" );
    std::string passWord( "kenzo123" );

    std::string tmpStr, tmpStr2;
    char tmpBuf[1024];

    /* Set twitter username and password */
    twitterObj.setTwitterUsername( userName );
    twitterObj.setTwitterPassword( passWord );

    /* OAuth flow begins */
    /* Step 0: Set OAuth related params. These are got by registering your app at twitter.com */
    twitterObj.getOAuth().setConsumerKey( std::string( "C7t7bmPUbPQo9bwCxeU5FA" ) );
    twitterObj.getOAuth().setConsumerSecret( std::string( "owpP8c3tLElQJawXo6MBivVN4LlpyMQArGUO1wAVhiw" ) );

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

        twitterObj.getOAuth().setOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().setOAuthTokenSecret( myOAuthAccessTokenSecret );
    }
    else
    {
        /* Step 2: Get request token key and secret */
        std::string authUrl;
        twitterObj.oAuthRequestToken( authUrl );

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
            twitterObj.getOAuth().setOAuthPin( tmpStr );
        }
        else
        {
            /* Else, pass auth url to twitCurl and get it via twitCurl PIN handling */
            twitterObj.oAuthHandlePIN( authUrl );
        }

        /* Step 4: Exchange request token with access token */
        twitterObj.oAuthAccessToken();

        /* Step 5: Now, save this access token key and secret for future use without PIN */
        twitterObj.getOAuth().getOAuthTokenKey( myOAuthAccessTokenKey );
        twitterObj.getOAuth().getOAuthTokenSecret( myOAuthAccessTokenSecret );

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
    if( twitterObj.accountVerifyCredGet() )
    {
        twitterObj.getLastWebResponse( replyMsg );
        //printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet web response:\n%s\n", replyMsg.c_str() );
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::accountVerifyCredGet error:\n%s\n", replyMsg.c_str() );
    }
}

std::string twitterFetch::getPublicTimeline(){
    /* Get public timeline */
    replyMsg = "";
    printf( "\nGetting public timeline\n" );
    if( twitterObj.timelinePublicGet() )
    {
        twitterObj.getLastWebResponse( replyMsg );
        return replyMsg;
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::timelinePublicGet error:\n%s\n", replyMsg.c_str() );
        return "";
    }

}

std::string twitterFetch::searchTerm(){
    replyMsg = "";
    std::string searchTerm("rmit"); 
    std::string fetchCount("1"); 
    if( twitterObj.search( searchTerm, fetchCount ) )
    {
        twitterObj.getLastWebResponse( replyMsg );
        return replyMsg;
    }
    else
    {
        twitterObj.getLastCurlError( replyMsg );
        printf( "\ntwitterClient:: twitCurl::search error:\n%s\n", replyMsg.c_str() );
        return "";
    } 
}

std::string twitterFetch::getReplyMsg(){
    return replyMsg;
}

twitCurl twitterFetch::getTwitterObj(){
    return twitterObj;
}

twitterFetch::twitterFetch()
{
    authenticate(twitterObj, replyMsg);
}
