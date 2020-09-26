#include "DataException.h"

void DataException::AllException(){
    ExceptionPrint(std::string("graph_data_space v1.0 by XXXXXX-Diwa 2020/9/20"));
}

void DataException::AllException(const uint32_t p,const int n){
    switch(n){
    case 0:
        std::cerr<<"错误的地址: "<<std::hex<<p;
        break;
    case 1:
        std::cerr<<"图片长度: "<<std::hex<<p<<"异常!";
        break;
    default:
        break;
    }
    ExceptionStopApp();
}

void DataException::ExceptionPrint(const std::string&exceptionInfo){
    std::cerr<<exceptionInfo;
    ExceptionStopApp();
}

void DataException::ExceptionStopApp(){
    std::cin.get();
    exit(-1);
}

void DataException::AllException(const int n,const std::string&s){
    switch(n){
    case 0:
        ExceptionPrint("文件: \""+s+"\" 读取失败!");
        break;
    case 1:
        ExceptionPrint("文件: \""+s+"\" 创建失败!");
        break;
    case 2:
        ExceptionPrint("文件夹: \""+s+"\" 创建失败!");
        break;
    default:
        ExceptionPrint(std::string("无法预料的错误!"));
        break;
    }
}

DataException::DataException(){

}

DataException::~DataException(){

}
