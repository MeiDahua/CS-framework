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

#ifndef APPSERVER_H
#define APPSERVER_H
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <map>

#include "info.h"

namespace CSFRAME {

    using namespace std;

    class Servant;
    class AppServer
    {
    public:
        AppServer(int port = 1777);
        ~AppServer();
    private:
        int init();
        void start();
        static void *waiterThread(void *arg);
        static void *heartBeat(void *arg);

        static const char* mMyName;
        int mPort;
        int mServerFd;
        sockaddr_in mServerAddr;
        static vector<sockinfo> mClients;
        static map<int, Servant*> mServants;
    };
}
#endif
