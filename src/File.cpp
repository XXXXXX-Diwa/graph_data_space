#include "File.h"
#include "DataException.h"
#include <direct.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

uint32_t File::save=0;
File::~File(){
    //dtor
    std::cout<<"导出完成!"<<std::endl;
}

File::File(const int argc,const char** argv):argNum(argc-1),args(argv){
    if(argc==1){
        DataException::AllException();
    }
    curPath=GetPath();
}

std::string File::GetPath(){
    std::string s=args[1];
    size_t pos=s.find_last_of('\\');
    if(pos>0&&pos!=std::string::npos){
        return s.substr(0,pos+1);
    }else{
        s=args[0];
        pos=s.find_last_of('\\');
        if(pos>0&&pos!=std::string::npos){
            return s.substr(0,pos+1);
        }else{
            DataException::AllException();
        }
    }
    return "";
}

void File::GraphDataSpace(){
    MakeDataFolders();
    GetPalInfo(GetGfxInfo(mzmUSpriteGfxPsP,mzmSpritesLen),mzmUSpritePalPsP,mzmSpritesLen);
    MakeSpritesGraphDataPointersArmFile();
    MakeSpritesGraphDataArmFile();
    DataPointerFusion();
    MakeGraphDataOldSpaceUseArmFile();
    MakeTestArmFile();
    AnsiFileToUf8File(std::vector<std::string>(armFiles,armFiles+4));
}

void File::MakeTestArmFile(){
    std::ofstream ouf;
    MakeFile(ouf,curPath+armFiles[3],false);
    ouf<<".gba\n.open \""<<OnlyName(args[1])<<"\",\"spriteGraphDataMove.gba\",0x8000000\n"
    <<"\n.include \""<<armFiles[0]<<"\"\n"<<"\n.include \""<<armFiles[1]
    <<"\"\n\n.close"<<std::endl;
}

std::string File::OnlyName(std::string s){
    size_t pos=s.find_last_of('\\');
    if(pos>0&&pos!=std::string::npos){
        return s.substr(pos+1);
    }
    return s;
}

void File::AnsiFileToUf8File(std::vector<std::string>armf){
    std::vector<std::string>::iterator iter=armf.begin();
    std::string str,tes;
    std::stringstream ss;
    std::ifstream inf;
    std::ofstream ouf;
    const uint8_t bom[3]={0xEF,0xBB,0xBF};
    uint8_t cbom[3]={};
    for(;iter!=armf.end();++iter){
        std::string fs=curPath+*iter;
        OpenFile(inf,fs,true);
        inf.read((char*)cbom,3);
        if(cbom[0]==0xEF&&cbom[1]==0xBB&&cbom[2]==0xBF){
            inf.close();
            continue;
        }
        inf.seekg(0,ios::beg);
        ss<<inf.rdbuf();
        str=ss.str();
        ss.str("");
        inf.close();
        AnsiToUtf8(str);
        MakeFile(ouf,fs,true);
        ouf.write((char*)bom,3);
        ouf.write((char*)str.c_str(),str.size());
        ouf.close();
    }
}

void File::AnsiToUtf8(std::string &s){
    if(s.empty()){
        DataException::ExceptionPrint(std::string("文件0字节?"));
    }
    auto const wlen=::MultiByteToWideChar(CP_ACP,0,s.c_str(),s.size(),nullptr,0);
    wchar_t* wstr=new wchar_t[wlen+1];
    wstr[wlen]='\0';
    ::MultiByteToWideChar(CP_ACP,0,s.c_str(),s.size(),wstr,wlen);
    auto len=UTF8Length(wstr, wlen);
    char* str=new char[len+1];
    UTF8FromUCS2(wstr, wlen,str,len);
    s=string(str,len);
    delete[]str;
    delete[]wstr;
}

uint32_t File::UTF8Length(const wchar_t *uptr, unsigned int tlen){
    unsigned int len = 0;
    for (unsigned int i = 0; i < tlen && uptr[i]; ++i) {
        unsigned int uch = uptr[i];
        if (uch < 0x80)
            ++len;
        else if (uch < 0x800)
            len += 2;
        else
            len +=3;
    }
    return len;
}

void File::UTF8FromUCS2(const wchar_t *uptr, unsigned int tlen, char *putf, unsigned int len){
    int k = 0;
    for (unsigned int i = 0; i < tlen && uptr[i]; ++i) {
        unsigned int uch = uptr[i];
        if (uch < 0x80) {
            putf[k++] = static_cast<char>(uch);
        } else if (uch < 0x800) {
            putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
            putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
        } else {
            putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
            putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
            putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
        }
    }
    putf[len] = '\0';
}

void File::MakeGraphDataOldSpaceUseArmFile(){
    std::ofstream ouf;
    MakeFile(ouf,curPath+armFiles[2],false);
    std::map<uint32_t,uint32_t>::iterator iter=pointerFusion.begin();
    ouf<<std::hex<<setiosflags(ios::uppercase)<<setfill('0');
    for(;iter!=pointerFusion.end();++iter){
        ouf<<".org 0x"<<std::setw(7)<<static_cast<int>(iter->first|0x8000000)
        <<"\n.area 0x"<<static_cast<int>(iter->second)
        <<",0xA\n\n.endarea\n"<<std::endl;
    }
    ouf.close();
}

void File::DataPointerFusion(){
    std::map<uint32_t,uint32_t>::iterator iter=pointerFusion.begin();
    uint32_t oft=0;
    uint32_t len=0;
    uint32_t tail=0;
    while(true){
        tail=iter->first+iter->second;
        ++iter;
        if(iter==pointerFusion.end()){
            break;
        }
        oft=iter->first;
//        cout<
        len=iter->second;
        if(tail==iter->first||(tail+4)>=iter->first){
            pointerFusion.erase(iter--);
            tail=oft+len;
            iter->second=tail-iter->first;
        }
    }
}

void File::MakeSpritesGraphDataArmFile(){
    std::vector<Info> temv(gfxInfo);
    std::sort(temv.begin(),temv.end(),PointerCompare);
//    temv.erase(std::unique(temv.begin(),temv.end(),PointerEqual),temv.end());
    char* buffer=new char[0x4000];
    std::vector<Info>::iterator iter=temv.begin();
    std::ifstream inf;
    std::ofstream oaf;
    MakeFile(oaf,curPath+"spriteGraphData.asm",false);
    oaf<<hex<<setiosflags(ios::uppercase)<<setfill('0');
    OpenFile(inf,args[1],true);
    oaf<<".org 0x"<<setw(7)<<static_cast<int>(0x8760d38)<<endl;
    bool last=false;
    while(iter!=temv.end()){
        inf.seekg(iter->address,ios::beg);
        if(!last){
            oaf<<".align\nsprite_"<<setw(2)<<static_cast<int>(iter->order+0x11)<<"_gfx:\n";
        }
        uint32_t p=iter->address;
        ++iter;
        if(iter==temv.end()){
            --iter;
            if(last){
                break;
            }else{
                last=true;
            }
        }else if(iter->address==p){
            continue;
        }else{
            --iter;
        }
        std::stringstream ss;
        ss<<hex<<setiosflags(ios::uppercase)<<setfill('0')<<setw(7)<<iter->address;
        oaf<<"\t.import \"spritegfxs\\"<<ss.str()<<"\""<<std::endl;

        std::ofstream ouf;
        MakeFile(ouf,curPath+"spritegfxs\\"+ss.str(),true);
        ss.str("");
        inf.read(buffer,iter->len);
        ouf.write(buffer,iter->len);
        ouf.close();
        ++iter;
    }
    oaf<<"\n\n"<<endl;
    temv.assign(palInfo.begin(),palInfo.end());
    sort(temv.begin(),temv.end(),PointerCompare);
    iter=temv.begin();
    last=false;
    while(iter!=temv.end()){
        inf.seekg(iter->address,ios::beg);
        if(!last){
            oaf<<".align\nsprite_"<<setw(2)<<static_cast<int>(iter->order+0x11)<<"_pal:\n";
        }
        uint32_t p=iter->address;
        ++iter;
        if(iter==temv.end()){
            --iter;
            if(last){
                break;
            }else{
                last=true;
            }
        }else if(iter->address==p){
            continue;
        }else{
            --iter;
        }
        std::stringstream ss;
        ss<<hex<<setiosflags(ios::uppercase)<<setfill('0')<<setw(7)<<iter->address;
        oaf<<"\t.import \"spritepals\\"<<ss.str()<<"\""<<std::endl;

        std::ofstream ouf;
        MakeFile(ouf,curPath+"spritepals\\"+ss.str(),true);
        ss.str("");
        inf.read(buffer,iter->len);
        ouf.write(buffer,iter->len);
        ouf.close();
        ++iter;
    }
    if(buffer!=NULL){
        delete[]buffer;
        buffer=NULL;
    }
    oaf.close();
    inf.close();
}

void File::MakeSpritesGraphDataPointersArmFile(){
    std::ofstream ouf;
    MakeFile(ouf,curPath+armFiles[0],false);

    ouf<<std::hex<<setiosflags(ios::uppercase)<<setfill('0')
    <<".org 0x"<<std::setw(7)<<static_cast<int>(mzmUSpriteGfxPsP|0x8000000)<<std::endl;
    for(int i=0x11;i<mzmSpritesLen+0x11;++i){
        ouf<<std::setw(2)<<"\t.word sprite_"<<i<<"_gfx"<<std::endl;
    }
    ouf<<"\n.org 0x"<<std::setw(7)<<static_cast<int>(mzmUSpritePalPsP|0x8000000)<<std::endl;
    for(int i=0x11;i<mzmSpritesLen+0x11;++i){
        ouf<<std::setw(2)<<"\t.word sprite_"<<i<<"_pal"<<std::endl;
    }
    ouf.close();
}

void File::MakeDataFolders(){
    std::string s=curPath+"\\spritegfxs";
    if(0!=access(s.c_str(),0)){
        if(mkdir(s.c_str())){
            DataException::AllException(2,s);
        }
    }
    s=curPath+"\\spritepals";
    if(0!=access(s.c_str(),0)){
        if(mkdir(s.c_str())){
            DataException::AllException(2,s);
        }
    }
}

void File::GetPalInfo(std::vector<uint32_t>palen,uint32_t beg,uint8_t len){
    ifstream inf;
    OpenFile(inf,args[1],true);
    palInfo.reserve(len);
    uint32_t ps[len]={};
    Info info;
    inf.seekg(beg,ios::beg);
    inf.read((char*)ps,4*len);
    inf.close();
    std::vector<uint32_t>::iterator iter=palen.begin();
    for(;iter!=palen.end();++iter){
        info.order=distance(palen.begin(),iter);
        info.len=*iter;
        if(ps[info.order]>>25!=4){
            DataException::AllException(ps[info.order],0);
        }
        info.address=ps[info.order]^0x8000000;
        palInfo.emplace_back(info);
        pointerFusion.insert(pair<uint32_t,uint32_t>(info.address,info.len));
    }
}

std::vector<uint32_t> File::GetGfxInfo(uint32_t beg,uint8_t len){
    gfxInfo.reserve(len);
    std::vector<uint32_t> palLen;
    palLen.reserve(len);
    ifstream inf;
    OpenFile(inf,args[1],true);
    uint32_t ps[len]={};
    Info info;
    inf.seekg(beg,ios::beg);
    inf.read((char*)ps,4*len);
    Lz77BgData *lz7=new Lz77BgData();
//    std::vector<Info>::iterator iter;
    for(uint8_t i=0;i<len;++i){
        if(ps[i]>>25!=4){
            DataException::AllException(ps[i],0);
        }
        info.address=ps[i]^0x8000000;
        save=info.address;
        auto iter=std::find_if(gfxInfo.begin(),gfxInfo.end(),PointerSame);
        inf.seekg(info.address,ios::beg);
        if(iter!=gfxInfo.end()){
            info.len=iter->len;
            uint32_t bit32;
            inf.read((char*)&bit32,4);
            bit32>>=8;
            palLen.emplace_back(bit32/0x40);

        }else{
            info.len=PalDataLen(inf,lz7,palLen);
        }
        info.order=i;
        gfxInfo.emplace_back(info);
        pointerFusion.insert(pair<uint32_t,uint32_t>(info.address,info.len));
    }
    if(NULL!=lz7){
        delete lz7;
        lz7=NULL;
    }
    inf.close();
    return palLen;
}

bool File::PointerCompare(const Info& i1,const Info& i2){
    return i1.address < i2.address;
}

bool File::PointerSame(const Info &i){
    return i.address == save;
}

bool File::PointerEqual(const Info& i1,const Info& i2){
    return i1.address == i2.address;
}

uint32_t File::PalDataLen(std::ifstream&inf,Lz77BgData* lz7,std::vector<uint32_t>&palLen){
        lz7->getLz77CompressData(inf,true);
        if(lz7->definelen%0x800!=0){
            DataException::AllException(lz7->definelen,1);
        }
        palLen.emplace_back(lz7->definelen/0x40);
        return lz7->length+4;
}

void File::OpenFile(std::ifstream&inf,std::string fn,bool bin){
    if(bin){
        inf.open(fn,ios::in|ios::binary);
    }else{
        inf.open(fn,ios::in);
    }
    if(!inf.is_open()){
        DataException::AllException(0,fn);
    }
}

void File::MakeFile(std::ofstream&ouf,std::string fn,bool bin){
    if(bin){
        ouf.open(fn,ios::out|ios::binary);
    }else{
        ouf.open(fn,ios::out);
    }
    if(!ouf.is_open()){
        DataException::AllException(1,fn);
    }
}
//
//std::vector<std::string> File::GetAsmFileName(std::string path){
//    //文件句柄
//    long hFile=0;
//    //文件信息
//    struct _finddata_t fileinfo;
//    std::string p=path+"*";
//    std::vector<std::string>armf;
//    armf.reserve(3);
//
//    if((hFile=_findfirst(p.c_str(),&fileinfo)) !=  -1){
//        do{
//            //如果是目录,迭代之
//            //如果不是,加入列表
//            if((fileinfo.attrib & _A_SUBDIR)){
//                if(std::string(fileinfo.name).find_last_of('.') == std::string::npos){
//                    p=path+std::string(fileinfo.name)+"\\";
//                    GetAsmFileName(p.c_str());
//                }
//            }
//            else{
//                p=fileinfo.name;
//                size_t pos=p.find_last_of('.');
//                if(pos>0&&pos!=std::string::npos){
//                    p=p.substr(pos+1);
//                }
//                if(p=="asm"||p=="ASM"){
//                    armf.emplace_back(p.assign(path).append(fileinfo.name) );
//                }
//            }
//        }while(_findnext(hFile, &fileinfo)  == 0);
//        _findclose(hFile);
//    }
//    return armf;
//}
