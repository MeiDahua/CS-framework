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

#ifndef SERVANT_H
#define SERVANT_H
#include <queue>
#include "appcmdparser.h"
#include "info.h"

#ifdef SERVANT_BUF_SIZE
#undef SERVANT_BUF_SIZE
#endif

#define SERVANT_BUF_SIZE 1024 // in bytes - 1K
namespace CSFRAME {
    using namespace std;
    struct ServantJob
    {
        ServantJob() : mParser(mBuf, mBytesAvailable) { mBytesAvailable = 0;
                       memset(mBuf, 0, sizeof(mBuf));
        }
        char mBuf[SERVANT_BUF_SIZE];
        unsigned int mBytesAvailable;
        AppCmdParser mParser;
    };

    class Servant
    {
    public:
        friend void *ServantThread(void *arg);
        explicit Servant(const sockinfo &client);
        ~Servant();

        void takeIn();
        //void putOut();

    private:
        sockinfo mClient;
        queue<ServantJob*> mJobList;
        queue<ServantJob*> mIncomingJobs;
        pthread_mutex_t mJobLock;
        pthread_t mThread;
    };

}

#endif
