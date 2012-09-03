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

#include <string.h>
#include <arpa/inet.h>
#include "servant.h"
#include "LogMacros.h"

namespace CSFRAME {
    using namespace LOGUTIL;

    void *ServantThread(void *arg)
    {
        if (arg == NULL)
            return 0;

        Servant *servant = static_cast<Servant*>(arg);
        for (;;)
        {
            pthread_mutex_lock(&(servant->mJobLock));
            while (servant->mIncomingJobs.size() != 0)
            {
                servant->mJobList.push(servant->mIncomingJobs.front());
                servant->mIncomingJobs.pop();
            }
            pthread_mutex_unlock(&(servant->mJobLock));

            while (servant->mJobList.size() != 0)
            {
                ServantJob *job = servant->mJobList.front();
                job->mParser.parse();
                if(job->mParser.availableBytes())
                {
                    write(servant->mClient.fd, job->mParser.outputBuf(), job->mParser.availableBytes());
                    job->mParser.clearOutput();
                }
                delete job;
                servant->mJobList.pop();
            }

            usleep(1000);
        }
    }

    Servant::Servant(const sockinfo &client):
        mClient(client)//, mBytesAvailable(0), mCmdParser(mBuf, mBytesAvailable)
    {
        FUNCTRACE
        LOGDEBUG("fd = " << mClient.fd);
        LOGDEBUG("ip = " << inet_ntoa(mClient.addr.sin_addr));

        mJobLock = PTHREAD_MUTEX_INITIALIZER;
        
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        int err = pthread_create(&mThread, &attr, &ServantThread, (void*)this);
        if (err != 0)
            LOGERROR("Cannot create servant thread");
    }

    Servant::~Servant()
    {
        FUNCTRACE
        LOGDEBUG("fd = " << mClient.fd);
        int err = pthread_cancel(mThread);
        if (err != 0)
            LOGERROR("Error canceling thread");

        pthread_mutex_destroy(&mJobLock);
        while (mJobList.size() != 0)
        {
            ServantJob *job = mJobList.front();
            delete job;
            mJobList.pop();
        }

        while (mIncomingJobs.size() != 0)
        {
            ServantJob *job = mIncomingJobs.front();
            delete job;
            mIncomingJobs.pop();
        }
    }

    int Servant::takeIn()
    {
        ServantJob *job = new ServantJob;
        job->mBytesAvailable = read(mClient.fd, job->mBuf, sizeof(job->mBuf) - 1);
        if (job->mBytesAvailable)
        {
            pthread_mutex_lock(&mJobLock);
            mIncomingJobs.push(job);
            pthread_mutex_unlock(&mJobLock);
            return 0;
        }
        else
        {
            delete job;
            return 1;
        }
    }
#if 0
    void Servant::putOut()
    {
        /*
        vector<ServantJob*>::iterator it = mJobList.begin();
        while (it != mJobList.end())
        {
            if((*it)->mParser.availableBytes())
            {
                write(mClient.fd, (*it)->mParser.outputBuf(), (*it)->mParser.availableBytes());
                (*it)->mParser.clearOutput();
                mJobList.erase(it);
                --it;
            }
            ++it;
        }
        */
        /*
        if (mCmdParser.availableBytes())
        {
            write(mClient.fd, mCmdParser.outputBuf(), mCmdParser.availableBytes());
            mCmdParser.clearOutput();
        }*/
    }
#endif

}
