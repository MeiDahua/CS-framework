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
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include "LogMacros.h"
#include "appserver.h"
#include "sockutil.h"
#include "servant.h"

using namespace LOGUTIL;

namespace CSFRAME {
    static struct timeval SERVINTVAL;
    static pthread_mutex_t c_torLock;

    vector<sockinfo> AppServer::mClients;
    map<int, Servant*> AppServer::mServants;
    const char* AppServer::mMyName = "AppServer";

    void *AppServer::waiterThread(void *arg)
    {
        FUNCTRACE
        
        int r, nfds = 0;
        fd_set rd;
        int readOK;

        for (;;)
        {
            usleep(1000);
            
            FD_ZERO(&rd);
            for (int i = 0; i < mClients.size(); i++)
            {
                FD_SET(mClients[i].fd, &rd);
                nfds = max(nfds, mClients[i].fd);
            }

            if (nfds == 0)
                continue;

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
                    pthread_mutex_lock(&c_torLock);
                    if (mServants.find(mClients[i].fd) == mServants.end())
                    {
                        pthread_mutex_unlock(&c_torLock);
                        continue;
                    }
                    pthread_mutex_unlock(&c_torLock);
                    readOK = mServants[mClients[i].fd]->takeIn();
                    if (readOK != 0)
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
        pthread_mutex_destroy(&c_torLock);
    }

    int AppServer::init()
    {
        FUNCTRACE
        int re = 0;
        pthread_mutex_init(&c_torLock, NULL);
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
        int keepAlive = 1;
        int keepAliveSec = 5;
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        pthread_t thread_t;

        listen(mServerFd, 5);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&thread_t, &attr, &AppServer::waiterThread, 0);
        pthread_attr_destroy(&attr);
        

        for (;;)
        {
            clientFd = accept(mServerFd, (sockaddr *)&clientAddr, &clientLen);
            setsockopt(clientFd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive));
            setsockopt(clientFd, IPPROTO_TCP, TCP_KEEPINTVL, (void*)&keepAliveSec, sizeof(keepAliveSec));
            sockinfo info;
            info.fd = clientFd;
            info.addr = clientAddr;
            mClients.push_back(info);
            pthread_mutex_lock(&c_torLock);
            mServants[clientFd] = new Servant(info);
            pthread_mutex_unlock(&c_torLock);
        }
    }

}
