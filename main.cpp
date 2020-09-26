#include <iostream>
#include "File.h"

using namespace std;

int main(const int argc,const char** argv)
{
    std::ios::sync_with_stdio(false);
    File* curFile=new File(argc,argv);
    curFile->GraphDataSpace();
    if(NULL!=curFile){
        delete curFile;
        curFile=NULL;
    }
    return 0;
}
