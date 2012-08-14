/*
This file is part of CS framework.

CS framework is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CS framework is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with CS framework.  If not, see <http://www.gnu.org/licenses/>.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "ServerConfig.h"
#include "LogMacros.h"
#include "appserver.h"


using namespace LOGUTIL;
using namespace CSFRAME;
#if 0
void error(char* str)
{
    fprintf(stderr, "%s\n", str);
    exit(1);
}

void init(int &sockfd, struct sockaddr_in &serv_addr, int portno)
{

    // TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // accept connection from any addr
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    // bind socket to addr
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd,5);
}
#endif

int main(int argc, char** argv)
{
    FUNCTRACE

    // Ignore SIGPIPE
    signal(SIGPIPE,SIG_IGN);
    printf("Version %s\n", VERSION);
    printf("Maintainer %s\n", MAINTAINER);

    int portno = 1777;
    if (argc > 1)
    {
        portno = atoi(argv[1]);
        AppServer appServ(portno);
    }
    else
        AppServer appServ;
    return 0;
#if 0
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2)
        error("ERROR, no port provided");
    // Get port number
    portno = atoi(argv[1]);
    init(sockfd, serv_addr, portno);

    clilen = sizeof(cli_addr);

    int sockfd2;
    init(sockfd2, serv_addr, 1888);

    for (;;)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        ///////////////////////////////////////////
        if (newsockfd < 0) 
            error("ERROR on accept");
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0)
            error("ERROR reading from socket");
        printf("Here is the message: %s\n",buffer);
        n = write(newsockfd,"I got your message",18);
        if (n < 0)
            error("ERROR writing to socket");
    }

    close(newsockfd);
    close(sockfd);
#endif
}
