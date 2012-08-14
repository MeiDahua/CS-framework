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

#ifndef LOGMACRO_H
#define LOGMACRO_H
#include <iostream>
#include "ServerConfig.h"

namespace LOGUTIL {

    #if defined DEBUG
    using namespace std;

    class FuncObj
    {
    public:
        explicit FuncObj(const char* aFuncName);
        ~FuncObj();

    private:
        const char* mFuncName;
        static int mStackCount;
    };

    inline FuncObj::FuncObj(const char* aFuncName): mFuncName(aFuncName)
    {
        for (int i = 0; i < mStackCount; i++)
        {
            cout << ">";
        }
        cout << "Entering " << aFuncName << endl;   
        mStackCount++;
    }

    inline FuncObj::~FuncObj()
    {
        mStackCount--;
        for (int i = 0; i < mStackCount; i++)
        {
            cout << "<";
        }
        cout << "Exiting " << mFuncName << endl;
    }

    #define FUNCTRACE FuncObj obj(__PRETTY_FUNCTION__);//obj(__FUNCTION__);
    #define LOGDEBUG(a) cout<<"[Debug] "<<a<<endl;
    #define LOGWARNING(a) cout <<"[Warning] "<<a<<endl;
    #define LOGERROR(a) cout<<"[Error] "<<a<<endl;

    #else

    #define FUNCTRACE
    #define LOGDEBUG(a)
    #define LOGWARNING(a)
    #define LOGERROR(a)


    #endif


}


#endif
