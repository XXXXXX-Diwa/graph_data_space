#ifndef FILE_H_INCLUDED
#define FILE_H_INCLUDED

#include <iostream>
#include <fstream>
#include <memory>
#include "Decompress.h"
#include <map>
#include <vector>

class File{
public:
    File(const int n,const char** fns);
    void GraphDataSpace();
    File();
    ~File();
private:
    void OpenFile(std::ifstream&,std::string,bool);
    void MakeFile(std::ofstream&,std::string,bool);
    std::string OnlyPath(std::string);
    uint32_t GetGraphDataPointer();
    uint8_t CheckFileType();
    bool CheckHeaderData(uint8_t*,int);
    void GetPointers();
    void GetArgs();
    void GetCin(std::string&);
    bool PointerCheckError(const uint32_t,const uint8_t);
    void GetPalData(std::vector<uint32_t>);
    std::vector<uint32_t>GetGfxData();
    void MakeDataFiles();
    std::string& PointerFileName(uint32_t,uint32_t);
    void MapDataFusion();
    void MakeGraphDataOldSpaceUseArmFile();
    void MakeSpritesGraphDataPointersArmFile();
    void MakeSpritesGraphDataFilesArmFile();
    void MakeDataFolders();
    uint8_t PointerCheckType(std::ifstream&);
    std::vector<std::string> GetAsmFileName(std::string);
    void AnsiFileToUf8File(std::vector<std::string>);
    void AnsiToUtf8(string &s);
    uint32_t UTF8Length(const wchar_t *,unsigned int);
    void UTF8FromUCS2(const wchar_t *,unsigned int, char *, unsigned int);

    std::string fileName;
    std::string appDir;
    uint8_t fileType;//0:mf 1:mzm 2:other
    const uint8_t mfUHeaderData[0x10]={0x4D,0x45,0x54,0x52,0x4F,0x49,0x44,0x34,0x55,0x53,0x41,0x00,0x41,0x4D,0x54,0x45};
    const uint8_t mzmUHeaderData[0x10]={0x5A,0x45,0x52,0x4F,0x4D,0x49,0x53,0x53,0x49,0x4F,0x4E,0x45,0x42,0x4D,0x58,0x45};
    const uint32_t mzmUSpriteGfxPsP=0x75EBF8 + 4;//第一个图特别奇葩,竟然不足0x800长度,故跳过
    const uint32_t mzmUSpritePalPsP=0x75EEF0 + 4;
    const uint32_t mfUSpriteGfxPsP=0x79A5D8;
    const uint32_t mfUSpritePalPsP=0x79A8D4;
    const uint8_t mzmSpritesNum=0xBE - 0x1;//总数减去第一个图
    const uint8_t mfSpritesNum=0xBF;
    std::vector<uint32_t>gfxPointers;
    std::vector<uint32_t>palPointers;
    std::map<uint32_t,uint32_t>pointerAndLen;
    std::map<uint32_t,uint32_t>pointerToNO;
    std::vector<std::string>gfxFiles;
    std::vector<std::string>palFiles;
    uint32_t gfxPsP;
    uint32_t palPsP;
    uint8_t spritesNum;
};


#endif // FILE_H_INCLUDED
