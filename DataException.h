#ifndef DATAEXCEPTION_H_INCLUDED
#define DATAEXCEPTION_H_INCLUDED

#include <iostream>

class DataException{
private:
    DataException();
    ~DataException();
    static void ExceptionStopApp();
public:
    static void ExceptionPrint(const std::string&);
    static void AllException();
    static void AllException(const uint32_t,const int);
    static void AllException(const std::string&);
    static void AllException(const int,const std::string&);

};

#endif // DATAEXCEPTION_H_INCLUDED
