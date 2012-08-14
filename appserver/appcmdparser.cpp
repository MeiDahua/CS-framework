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
#include <string.h>
#include "appcmdparser.h"
#include "LogMacros.h"

namespace CSFRAME {

    using namespace LOGUTIL;

    AppCmdParser::AppCmdParser(char *buf, unsigned int &size) : mRDBuf(buf), mInSize(size)
    {
    }

    AppCmdParser::~AppCmdParser()
    {
    }

    void AppCmdParser::parse()
    {
        FUNCTRACE
        LOGDEBUG("input cmd: " << mRDBuf << "size: " << mInSize);
        mWRBuf.clear();
        if (strncmp(mRDBuf, "lshw", 4) == 0)
        {
            LOGDEBUG("CMD lshw");
            FILE *pi = popen("lshw", "r");
            if (pi != NULL)
            {
                char ch;
                while (!feof(pi))
                {
                    fflush(pi);
                    fread(&ch, sizeof(char), sizeof(char), pi);
                    mWRBuf.push_back(ch);
                }
                
                pclose(pi);
            }
        }
    }

    unsigned int AppCmdParser::availableBytes()
    {
        return mWRBuf.size();
    }
    
    const char *AppCmdParser::outputBuf()
    {
        return mWRBuf.c_str();
    }

    void AppCmdParser::clearOutput()
    {
        mWRBuf.clear();
        mWRBuf.resize(0);
    }

}
