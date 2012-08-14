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

#ifndef APPCMDPARSER_H
#define APPCMDPARSER_H

#include <string>
#include "cmdparser.h"

namespace CSFRAME {
    using namespace std;
    class AppCmdParser : public CmdParser
    {
    public:
        AppCmdParser(char *buf, unsigned int &size);
        ~AppCmdParser();
        
        void parse();
        unsigned int availableBytes();
        const char *outputBuf();
        void clearOutput();

    private:
        const char *mRDBuf;
        string mWRBuf;
        unsigned int &mInSize;
    };
}
#endif
