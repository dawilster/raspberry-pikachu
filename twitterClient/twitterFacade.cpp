#include "twitterFacade.h"
#include "twitterFetch.h"
int main( int argc, char* argv[] )
{



    twitterFetch *twitter = new twitterFetch();
    twitter->search();



    return 0;
}


