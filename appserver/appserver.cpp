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
#include <errno.h>
#include "LogMacros.h"
#include "appserver.h"
#include "sockutil.h"
#include "servant.h"

using namespace LOGUTIL;

namespace CSFRAME {
    static struct timeval SERVINTVAL;

    vector<sockinfo> AppServer::mClients;
    map<int, Servant*> AppServer::mServants;
    const char* AppServer::mMyName = "AppServer";

    void *AppServer::waiterThread(void *arg)
    {
        FUNCTRACE
        
        for (;;)
        {
            usleep(1000);
            
            int r, nfds = 0;
            fd_set rd;//, wr;
            FD_ZERO(&rd);
            //FD_ZERO(&wr);
            for (int i = 0; i < mClients.size(); i++)
            {
                FD_SET(mClients[i].fd, &rd);
                //FD_SET(mClients[i].fd, &wr);
                nfds = max(nfds, mClients[i].fd);
            }

            if (nfds == 0)
                continue;

            //r = select(nfds + 1, &rd, &wr, NULL, &SERVINTVAL);
            r = select(nfds + 1, &rd, NULL, NULL, &SERVINTVAL);
            if (r == 0)
                continue;

            if (r == -1 && errno == EINTR)
                continue;

            if (r == -1)
            {
                LOGERROR("select() failed");
                exit(-1);
            }

            for (int i = 0; i < mClients.size(); i++)
            {
                if (FD_ISSET(mClients[i].fd, &rd))
                {
                    //read
                    LOGDEBUG("fd " << mClients[i].fd << " ready to read");
                    
                    if (mServants.find(mClients[i].fd) != mServants.end())
                    {
                        mServants[mClients[i].fd]->takeIn();
                    }
                }
                /*
                if (FD_ISSET(mClients[i].fd, &wr))
                {
                    //write
                    //LOGDEBUG("fd" << mClients[i].fd << "ready to write");
                    if (mServants.find(mClients[i].fd) != mServants.end())
                    {
                        mServants[mClients[i].fd]->putOut();
                    }
                }*/
            }
        }
        return 0;
    }

    void *AppServer::heartBeat(void *arg)
    {
        FUNCTRACE

        for (;;)
        {
            LOGDEBUG("AppServer thread");
            LOGDEBUG("Number of clients: "<< mClients.size());
            int r, nfds = 0;
            fd_set wr;
            FD_ZERO(&wr);
            for (int i = 0; i < mClients.size(); i++)
            {
                FD_SET(mClients[i].fd, &wr);
                nfds = max(nfds, mClients[i].fd);
            }

            if (nfds == 0)
            {
                sleep(1);
                continue;
            }

            r = select(nfds + 1, NULL, &wr, NULL, &SERVINTVAL);
            if (r == 0)
            {
                LOGDEBUG("continued");
                continue;
            }

            if (r == -1 && errno == EINTR)
                continue;

            if (r == -1)
            {
                LOGERROR("select() failed");
                exit(-1);
            }

            for (int i = 0; i < mClients.size(); i++)
            {
                if (FD_ISSET(mClients[i].fd, &wr))
                {
                    //write
                    LOGDEBUG("sending heartbeat to " << mClients[i].fd);
                    r = write(mClients[i].fd, mMyName, strlen(mMyName) + 1);
                    // if client is down, recycle all alocated resource
                    if (r < 1)
                    {
                        LOGWARNING("client fd " << mClients[i].fd << " is down");
                        delete mServants[mClients[i].fd];
                        mServants.erase(mClients[i].fd);
                        SHUT_FD(mClients[i].fd);
                        vector<sockinfo>::iterator itr = mClients.begin();
                        advance(itr, i);
                        mClients.erase(itr);
                        --i;
                    }
                }
            }

            if (mClients.size() != 0)
                sleep(10);
        }

        return 0;
    }

    AppServer::AppServer(int port): mPort(port)
    {
        FUNCTRACE
        if (init() != 0)
            return;

        start();
    }

    AppServer::~AppServer()
    {
        FUNCTRACE
        close(mServerFd);
        for (int i = 0; i < mClients.size(); i++)
            close(mClients[i].fd);
    }

    int AppServer::init()
    {
        FUNCTRACE
        int re = 0;
        SERVINTVAL.tv_sec = 0;
        SERVINTVAL.tv_usec = 2000;  /* 0.002 seconds */
        // TCP socket
        mServerFd = socket(AF_INET, SOCK_STREAM, 0);
        if (mServerFd < 0)
        {
            LOGERROR("ERROR opening socket");
            re = 1;
        }

        bzero((char *) &mServerAddr, sizeof(mServerAddr));
        mServerAddr.sin_family = AF_INET;
        // accept connection from any addr
        mServerAddr.sin_addr.s_addr = INADDR_ANY;
        mServerAddr.sin_port = htons(mPort);
        // bind socket to addr
        if (bind(mServerFd, (struct sockaddr *) &mServerAddr, sizeof(mServerAddr)) < 0)
        {
            LOGERROR("ERROR on binding");
            re = 1;
        }

        return re;

    }

    void AppServer::start()
    {
        FUNCTRACE
        int clientFd;
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        pthread_t thread_t;

        listen(mServerFd, 5);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&thread_t, &attr, &AppServer::waiterThread, 0);
        pthread_create(&thread_t, &attr, &AppServer::heartBeat, 0);
        pthread_attr_destroy(&attr);
        

        for (;;)
        {
            clientFd = accept(mServerFd, (sockaddr *)&clientAddr, &clientLen);
            sockinfo info;
            info.fd = clientFd;
            info.addr = clientAddr;
            mClients.push_back(info);
            mServants[clientFd] = new Servant(info);
        }
    }

}
