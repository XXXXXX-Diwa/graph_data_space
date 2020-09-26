#include "DataException.h"

void DataException::AllException(){
    ExceptionPrint(std::string("graph_data_space v1.0 by XXXXXX-Diwa 2020/9/20"));
}

void DataException::AllException(const uint32_t p,const int n){
    switch(n){
    case 0:
        std::cerr<<"����ĵ�ַ: "<<std::hex<<p;
        break;
    case 1:
        std::cerr<<"ͼƬ����: "<<std::hex<<p<<"�쳣!";
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
        ExceptionPrint("�ļ�: \""+s+"\" ��ȡʧ��!");
        break;
    case 1:
        ExceptionPrint("�ļ�: \""+s+"\" ����ʧ��!");
        break;
    case 2:
        ExceptionPrint("�ļ���: \""+s+"\" ����ʧ��!");
        break;
    default:
        ExceptionPrint(std::string("�޷�Ԥ�ϵĴ���!"));
        break;
    }
}

DataException::DataException(){

}

DataException::~DataException(){

}
