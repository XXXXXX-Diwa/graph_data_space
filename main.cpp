#include <iostream>
#include "File.h"
#include "DataException.h"
#include <memory>

using namespace std;

int main(const int argc,const char* argv[])
{
    ios::sync_with_stdio(false);
    std::shared_ptr<File>curFile(new File(argc,argv));
    curFile->GraphDataSpace();
    curFile.reset();
    return 0;
}
