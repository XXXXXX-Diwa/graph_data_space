#include "File.h"
#include "DataException.h"
#include <sstream>
#include <iterator>
#include <iomanip>
#include <direct.h>
#include <wchar.h>
#include <windows.h>

File::File(const int n,const char** fns){
    if(n==1||n>2){
        DataException::AllException();
    }
    appDir=OnlyPath(fns[0]);
    fileName=fns[1];
    fileType=CheckFileType();
}

void File::GraphDataSpace(){
    if(fileType==0){
        //查看数据才发现mf的graph_data非压缩,因此头数据没有解压长度数据
        //故放弃对mf的支持(毕竟本来就是附带的支持)
        DataException::ExceptionPrint(std::string("不支持非lz777压缩的gfx类型数据!"));
    }
    GetPointers();
    GetPalData(GetGfxData());
    MakeDataFolders();
    MakeDataFiles();
    MapDataFusion();
    MakeSpritesGraphDataPointersArmFile();
    MakeSpritesGraphDataFilesArmFile();
    MakeGraphDataOldSpaceUseArmFile();
    AnsiFileToUf8File(GetAsmFileName(appDir));
}

void File::MakeDataFolders(){
    std::string s=appDir+"\\spritegfxs";
    if(0!=access(s.c_str(),0)){
        if(mkdir(s.c_str())){
            DataException::AllException(2,s);
        }
    }
    s=appDir+"\\spritepals";
    if(0!=access(s.c_str(),0)){
        if(mkdir(s.c_str())){
            DataException::AllException(2,s);
        }
    }
}

void File::MakeGraphDataOldSpaceUseArmFile(){
    std::ofstream ouf;
    MakeFile(ouf,appDir+"graphDataOldSpaceUse.asm",false);
    std::map<uint32_t,uint32_t>::iterator iter=pointerAndLen.begin();
    ouf<<std::hex<<setiosflags(ios::uppercase)<<setfill('0');
    for(;iter!=pointerAndLen.end();++iter){
        ouf<<".org 0x"<<std::setw(7)<<static_cast<int>(iter->first|0x8000000)
        <<"\n.area 0x"<<static_cast<int>(iter->first|0x8000000)
        <<",0\n\n.endarea\n"<<std::endl;
    }
    ouf.close();
}

void File::MakeSpritesGraphDataPointersArmFile(){
    std::ofstream ouf;
    MakeFile(ouf,appDir+"spriteGfaphDataPointer.asm",false);

    ouf<<std::hex<<setiosflags(ios::uppercase)<<setfill('0')
    <<"org 0x"<<std::setw(7)<<static_cast<int>(gfxPsP|0x8000000)<<std::endl;
    for(int i=0x10;i<spritesNum+0x10;++i){
        ouf<<std::setw(2)<<"\t.word sprite_"<<i<<"_gfx"<<std::endl;
    }
    ouf<<"\norg 0x"<<std::setw(7)<<static_cast<int>(palPsP|0x8000000)<<std::endl;
    for(int i=0x10;i<spritesNum+0x10;++i){
        ouf<<std::setw(2)<<"\t.word sprite_"<<i<<"_pal"<<std::endl;
    }
    ouf.close();
}

void File::MakeSpritesGraphDataFilesArmFile(){
    std::ofstream ouf;
    MakeFile(ouf,appDir+"spriteGraphData.asm",false);
    std::map<uint32_t,uint32_t>::iterator iter=pointerToNO.begin();
    ouf<<std::hex<<setiosflags(ios::uppercase)<<setfill('0');
    ouf<<".org 0x8760d38"<<endl;
    for(;iter!=pointerToNO.begin();++iter){
        uint32_t pos=distance(pointerToNO.begin(),iter);
        ouf<<".align\nsprite_"<<setw(2)<<static_cast<int>(iter->second)<<"_gfx:"<<std::endl;
        ouf<<"\t.import \""<<gfxFiles[pos]<<"\""<<std::endl;
        ouf<<".align\nsprite_"<<setw(2)<<static_cast<int>(iter->second)<<"_pal:"<<std::endl;
        ouf<<"\t.import \""<<palFiles[pos]<<"\""<<std::endl;
    }
}

void File::AnsiFileToUf8File(std::vector<std::string>armf){
    vector<string>::iterator iter=armf.begin();
    string str,tes;
    stringstream ss;
    ifstream inf;
    ofstream ouf;
    const uint8_t bom[3]={0xEF,0xBB,0xBF};
    uint8_t cbom[3]={};
    for(;iter!=armf.end();++iter){
        File::OpenFile(inf,*iter,true);
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
        MakeFile(ouf,*iter,true);
        ouf.write((char*)bom,3);
        ouf.write((char*)str.c_str(),str.size());
        ouf.close();
    }
}

void File::AnsiToUtf8(string &s){
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

void File::MapDataFusion(){
//    pointerFusion=pointerAndLen;
    std::map<uint32_t,uint32_t>::iterator iter=pointerAndLen.begin();
    uint32_t oft=0;
    uint32_t len=0;
    uint32_t tail=0;
    while(true){
        tail=iter->first+iter->second;
        ++iter;
        if(iter==pointerAndLen.end()){
            break;
        }
        oft=iter->first;
        len=iter->second;
        if(tail==iter->first||(tail+4)>=iter->first){
            pointerAndLen.erase(iter--);
            tail=oft+len;
            iter->second=tail-iter->first;
        }
    }
}

std::vector<std::string> File::GetAsmFileName(std::string path){
    //文件句柄
    long hFile=0;
    //文件信息
    struct _finddata_t fileinfo;
    std::string p=path+"*";
    std::vector<std::string>armf;
    armf.reserve(3);

    if((hFile=_findfirst(p.c_str(),&fileinfo)) !=  -1){
        do{
            //如果是目录,迭代之
            //如果不是,加入列表
            if((fileinfo.attrib & _A_SUBDIR)){
                if(std::string(fileinfo.name).find_last_of('.') == std::string::npos){
                    p=path+std::string(fileinfo.name)+"\\";
                    GetAsmFileName(p.c_str());
                }
            }
            else{
                p=fileinfo.name;
                size_t pos=p.find_last_of('.');
                if(pos>0&&pos!=std::string::npos){
                    p=p.substr(pos+1);
                }
                if(p=="asm"||p=="ASM"){
                    armf.emplace_back(p.assign(path).append(fileinfo.name) );
                }
            }
        }while(_findnext(hFile, &fileinfo)  == 0);
        _findclose(hFile);
    }
    return armf;
}

void File::MakeDataFiles(){
    std::ifstream inf;
    OpenFile(inf,fileName,true);
    std::ofstream ouf;
    std::map<uint32_t,uint32_t>::iterator iter=pointerAndLen.begin();
    char *buffer=new char[0x4000];//提升效率懒得用智能指针了
    gfxFiles.reserve(spritesNum);
    palFiles.reserve(spritesNum);
    for(;iter!=pointerAndLen.end();++iter){
        MakeFile(ouf,PointerFileName(iter->first,iter->second),true);
        inf.seekg(iter->first,ios::beg);
        inf.read(buffer,iter->second);
        ouf.write(buffer,iter->second);
        ouf.close();
    }
    delete[]buffer;
}

std::string& File::PointerFileName(uint32_t oft,uint32_t len){
    std::stringstream ss;
    ss<<std::hex<<setw(2)<<setfill('0')<<setiosflags(ios::uppercase)<<oft;
    if(len>=0x800){
        gfxFiles.emplace_back(appDir+"spritegfxs\\spritegfx_"+ss.str()+".lz7");
        return gfxFiles.back();
    }else{
        palFiles.emplace_back(appDir+"spritepals\\spritepal_"+ss.str()+".pal");
        return palFiles.back();
    }
}

std::vector<uint32_t> File::GetGfxData(){
    std::ifstream inf;
    std::vector<uint32_t>palens(spritesNum);
    OpenFile(inf,fileName,true);
    std::shared_ptr<Lz77BgData>lz7(new Lz77BgData());//创建Lz777解压类shared智能指针
    std::vector<uint32_t>::iterator iter=gfxPointers.begin();
    for(;iter!=gfxPointers.end();++iter){
        inf.seekg(*iter,ios::beg);
        lz7->getLz77CompressData(inf,true);
        if(lz7->definelen%0x800!=0){
            DataException::AllException(*iter,1);
        }
        pointerToNO.insert(pair<uint32_t,uint32_t>(*iter,distance(gfxPointers.begin(),iter)));
        pointerAndLen.insert(pair<uint32_t,uint32_t>(*iter,lz7->length+4));
        palens.emplace_back(lz7->definelen/0x40);
    }
    lz7.reset();
    inf.close();

    return palens;
}

void File::GetPalData(std::vector<uint32_t>lens){
    std::vector<uint32_t>::iterator iterl=lens.begin();
    std::vector<uint32_t>::iterator iterp=palPointers.begin();
    for(;iterl!=lens.end();++iterl,++iterp){
        pointerToNO.insert(pair<uint32_t,uint32_t>(*iterp,distance(palPointers.begin(),iterp)));
        pointerAndLen.insert(pair<uint32_t,uint32_t>(*iterp,*iterl));
    }
}

void File::GetPointers(){
    if(2==fileType){
        void GetArgs();
    }else if(1==fileType){
        gfxPsP=mzmUSpriteGfxPsP;
        palPsP=mzmUSpritePalPsP;
        spritesNum=mzmSpritesNum;
    }else{
        gfxPsP=mfUSpriteGfxPsP;
        palPsP=mfUSpritePalPsP;
        spritesNum=mfSpritesNum;
    }
    gfxPointers.reserve(spritesNum);//扩容提升效率
    palPointers.reserve(spritesNum);
    uint32_t pointers[spritesNum]={};
    std::ifstream inf;
    OpenFile(inf,fileName,true);

    inf.seekg(gfxPsP,ios::beg);
    inf.read((char*)pointers,spritesNum*4);
    for(int i=0;i<spritesNum;++i){
        gfxPointers.emplace_back(pointers[i]^0x8000000);
    }
    inf.seekg(palPsP,ios::beg);
    inf.read((char*)pointers,spritesNum*4);
    for(int i=0;i<spritesNum;++i){
        palPointers.emplace_back(pointers[i]^0x8000000);
    }
    inf.close();
}

void File::GetArgs(){
    std::string s;
    std::cout<<"请输入Sprite数量:";
    GetCin(s);
    uint32_t n=0;
    std::stringstream ss;
    ss<<s;
    ss>>n;
    if(n>0xFF){
        DataException::ExceptionPrint(std::string("数量不可大于256!"));
    }else if(n==0){
        DataException::ExceptionPrint(std::string("数目无效!"));
    }
    spritesNum=static_cast<uint8_t>(n);
    std::cout<<"请输入Gfx数据地址(16进制):";
    GetCin(s);
    ss<<s;
    ss>>std::hex>>n;
    if(n>0x1FFFFFF||n==0){
        DataException::AllException(n,0);
    }
    if(PointerCheckError(n,spritesNum)){
        DataException::ExceptionPrint(std::string("指针表中有非指针!"));
    }
    gfxPsP=n;

    std::cout<<"请输入Pal数据地址(16进制):";
    GetCin(s);
    ss<<s;
    ss>>std::hex>>n;
    if(n>0x1FFFFFF||n==0){
        DataException::AllException(n,0);
    }
    if(PointerCheckError(n,spritesNum)){
        DataException::ExceptionPrint(std::string("指针表中有非指针!"));
    }
    palPsP=n;
}
void File::GetCin(std::string& s){
    std::cin>>s;
    std::cin.clear();
    std::cin.sync();
}

bool File::PointerCheckError(const uint32_t p,const uint8_t n){
    std::ifstream inf;
    uint32_t pointers[n]={};
    OpenFile(inf,fileName,true);
    inf.seekg(p,ios::beg);
    inf.read((char*)pointers,n*4);
    for(uint8_t i=0;i<n;++i){
        if(4!=(pointers[i]>>25)){
            inf.close();
            return true;
        }
    }
    inf.close();
    return false;
}

std::string File::OnlyPath(std::string s){
    size_t pos=s.find_last_of('\\');
    if(pos>0&&pos!=std::string::npos){
        return s.substr(0,pos+1);
    }
    return s;
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

uint8_t File::CheckFileType(){
    std::ifstream inf;
    uint8_t type;

    OpenFile(inf,fileName,true);
    uint8_t headData[0x10];
    inf.seekg(0xA0,ios::beg);
    inf.read((char*)headData,0x10);
    if(CheckHeaderData(headData,1)){
        type=1;
    }else if(CheckHeaderData(headData,0)){
        type=0;
    }else{
        type=PointerCheckType(inf);
    }
    inf.close();
    return type;
}

uint8_t File::PointerCheckType(std::ifstream&inf){
    inf.seekg(mzmUSpriteGfxPsP,ios::beg);
    uint32_t pointers[0xBF]={};

    inf.read((char*)pointers,mzmSpritesNum*4);
    for(uint8_t i=0;i<mzmSpritesNum;++i){
        if(4!=(pointers[i]>>25)){
            inf.seekg(mfUSpriteGfxPsP,ios::beg);
            inf.read((char*)pointers,mfSpritesNum*4);
            for(uint8_t j=0;j<mfSpritesNum;++j){
                if(4!=(pointers[i]>>25)){
                    return 2;
                }
            }
            inf.seekg(mfUSpritePalPsP,ios::beg);
            inf.read((char*)pointers,mfSpritesNum*4);
            for(uint8_t j=0;j<mfSpritesNum;++j){
                if(4!=(pointers[i]>>25)){
                    return 2;
                }
            }
            return 0;
        }
    }
    inf.seekg(mzmUSpritePalPsP,ios::beg);
    inf.read((char*)pointers,mzmSpritesNum*4);
    for(uint8_t i=0;i<mzmSpritesNum;++i){
        if(4!=(pointers[i]>>25)){
            return 2;
        }
    }
    return 1;
}

bool File::CheckHeaderData(uint8_t *hd,const int n){
    const uint8_t* headat;
    if(n){
        headat=mzmUHeaderData;
    }else{
        headat=mfUHeaderData;
    }
    for(int i=0;i<0x10;++i){
        if(hd[i]!=headat[i]){
            return false;
        }
    }
    return true;
}

File::File(){}
File::~File(){}
