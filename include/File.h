#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Decompress.h"
#include <map>
#include <wchar.h>
#include <windows.h>


class File{
    public:
        File(const int,const char**);
        virtual ~File();
        void GraphDataSpace();
        struct Info{
            uint8_t order;
            uint32_t address;
            uint32_t len;
        };
    protected:

    private:

//        class AddressCompare{
//            AddressCompare(uint32_t get):save(get){}
//
//            uint32_t save;
//        };
        bool operator()(Info&);
        File(const File&)=delete;
        void operator=(const File&)=delete;
        void OpenFile(std::ifstream&,std::string,bool);
        void MakeFile(std::ofstream&,std::string,bool);
        void MakeDataFolders();
        std::string GetPath();
        uint32_t PalDataLen(std::ifstream&,Lz77BgData*,std::vector<uint32_t>&);
        void GetPalInfo(std::vector<uint32_t>,uint32_t,uint8_t);
        void MakeSpritesGraphDataPointersArmFile();
        void MakeSpritesGraphDataArmFile();
        void MakeGraphDataOldSpaceUseArmFile();
        void DataPointerFusion();
//        std::vector<std::string> GetAsmFileName(std::string);

        static bool PointerCompare(const Info&,const Info&);
        static bool PointerSame(const Info&);
        static bool PointerEqual(const Info&,const Info&);

        std::vector<uint32_t> GetGfxInfo(uint32_t,uint8_t);
        void AnsiFileToUf8File(std::vector<std::string>);
        void AnsiToUtf8(string &s);
        uint32_t UTF8Length(const wchar_t *,unsigned int);
        void UTF8FromUCS2(const wchar_t *,unsigned int, char *, unsigned int);

        void MakeTestArmFile();
        std::string OnlyName(std::string);

        const uint32_t mzmUSpriteGfxPsP=0x75EBF8 + 4;//第一个图特别奇葩,竟然不足0x800长度,故跳过
        const uint32_t mzmUSpritePalPsP=0x75EEF0 + 4;
        const uint8_t mzmSpritesLen=0xBE - 0x1;//总数减去第一个图


        std::string curPath;
        std::string armFiles[4]={
            "spriteGraphDataPointer.asm",
            "spriteGraphData.asm",
            "graphDataOldSpaceUse.asm",
            "test.asm"
        };
        std::vector<Info>gfxInfo;
        std::vector<Info>palInfo;
        std::map<uint32_t,uint32_t>pointerFusion;
        const int argNum;
        const char** args;
//        static uint32_t save;
        struct finder_t
        {
            finder_t(uint32_t n) : save(n){}
            bool operator()(const std::vector<File::Info>::value_type&p){
              return (save == p.address);
            }
            uint32_t save;
        };
};


#endif // FILE_H
