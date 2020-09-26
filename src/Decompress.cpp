#include "Decompress.h"

RelBgData::RelBgData(){
}
RelBgData::~RelBgData(){
//    delete[]RelBgData::relCompressedData;
}
/**
*���ֽ��Ǵ����Ƿ��ǵ��ֽ�flag ��Ϊ1,��Ϊ���ֽ�,������Ϊ˫�ֽ�

*�ڶ����ֽڵĵ�7λ���ظ�flag,���б��,��ȥ����7λ�ĵڶ�λ�ǵ���λ����

*������
*		���ޱ��,��ڶ�λ�����˴˺��ж��ٸ����ظ����ֽ�

*���ڶ����ֽ�Ϊ00��ʱ��Ϳ��������л����Ҫ������flag

*����Ϊ��һ������00(���Ѿ��л���˫�ֽ�ģʽ����Ҫ����00 00)�ͻ����

*�ڶ��ֽ��״�����00��ô��һ���ֽھ��������ֽ�,����ݴ��ֽ��л���˫

**/
void RelBgData::getRelCompressData(ifstream &inf){
    roomWidth=inf.get();
    roomHeigh=inf.get();
    length=roomHeigh*roomWidth;
    length<<=1;//����2
    inf.read((char*)relCompressedData,length);
    bool swibit=false;
    uint8_t ut8,bit8,byte1;
    uint16_t bit16;
    uint32_t total=0,len=0;//һ����¼��ѹ�ۼƳ���һ����¼��ȡ������
    ut8=relCompressedData[len];
    while(true){//total<length){
        if(ut8==1){
            ++len;
            bit8=relCompressedData[len];
            if(bit8==0){
                if(swibit){

                    break;
                }
                swibit=true;
                ++len;
                ut8=relCompressedData[len];//�˴�����תΪ2bit����,ֻҪ��ֵ��Ϊ1
					//2bit���͵ĵڶ��ֽں͵����ֽ�ͬʱΪ0�Ż����,�ʽ�βΪ00 00
					//����ά��1bit����,��ڶ����ֽ��ٴ�Ϊ0�ͻ����
            }else if((bit8&0x80)==0){
                total+=bit8;
                len+=bit8;
            }else{
                bit8^=0x80;
                if(bit8==0){
                    continue;
                }
                total+=bit8;
                ++len;
            }
        }else{
            ++len;
            bit8=relCompressedData[len];
            ++len;
            byte1=relCompressedData[len];
            bit16=bit8;
            bit16<<=8;
            bit16|=byte1;
            if(bit16==0){
                if(swibit){
                    break;
                }

                swibit=true;
                ++len;
                ut8=relCompressedData[len];
            }else if((bit16&0x8000)==0){
                total+=bit16;
                len+=bit16;
            }else{
                bit16^=0x8000;
                if(bit16==0){
                    continue;
                }
                total+=bit16;
                ++len;
            }
        }
    }
    length=len+1;
}


Lz77BgData::Lz77BgData(){}
Lz77BgData::~Lz77BgData(){
//    delete[]Lz77BgData::lz77CompressedTileTable;
}
/**
*���ȵõ���ѹ����,Ȼ�����ȡ�����ݵ�ַ�Ƿ���0x8000000

*��ȡ��һ���ֽ�,Ȼ����������=8,������һ,����Ƿ�Ϊ��,
*��Ϊ��,���ٴζ�ȡ��һ���ֽ�,Ȼ��������=8 ����
*����һ���ֽ��Ƿ�û��7λ
*û�еĻ���ȡ�ڶ����ֽ�д���ڴ� �ܳ���-1 ����ܳ����Ƿ�Ϊ0
*��Ϊ0�Ļ���һ���ֽ�����1h Ȼ���������-1,����Ƿ�Ϊ��
*����һ���ֽ��Ƿ�û��7λ...

*����һ���ֽ���7λ,��ȡ�ڶ����ֽ�,��������4�ټ�3 (ȥ����λ��+3)(len)
*�ٴζ�ȡ�ڶ����ֽ�ȥ����λ,Ȼ������8h,��ȡ�������ֽ�,orrǰ��
*����ĵڶ��ֽ�,��ֵ��1.(�ڴ���ǰƫ��ֵ)
*��ȡ��ǰ�ڴ��ǰ��(�ڴ���ǰƫ��),д����ǰ���ڴ��ַ,��ָ������
*len�ݼ�,ֻҪlen��Ϊ0,������д

*lenΪ���,��һ���ֽ�����1λ����ѭ��,ֻҪ8��ѭ����û����

*ÿ��д����ѭ��8��,ֱ���ﵽ1000h����
*���ֽ�λ�������˴�ʲôʱ��ʼ����֮ǰ���ڴ渴д.��Ȼ����Ϊ����ѭ��һ��

*�ڴ治��д�Ĳ���ֻ�ǵ��ֽڵĶ�дѭ��.���8��,Ȼ���ٴζ�ȡ��һ���ֽ�
**/

//���ݿ���rom�е���βҪ��4���� ����۲�����˵�(δȷ��)

void Lz77BgData::getLz77CompressData(ifstream &inf,bool byte2flag){
    if(byte2flag){
        bgSize=0;
    }else{
        inf.read((char*)&bgSize,4);
    }
    inf.read((char*)&decompressedLen,4);
    definelen=decompressedLen>>8;//��ע�Ľ�ѹ�ܳ���
    inf.read((char*)lz77CompressedTileTable,definelen);
    uint16_t bit16;
    uint32_t total=0,len=0;//��ѹ�ۼƳ��Ⱥ͵�ǰ��ȡλ��
    uint8_t bit8,byte1,ut8;
    while(total<definelen){
        bit8=lz77CompressedTileTable[len];    //��һ���ֽ�
        ++len;
        int i=8;
        while((--i)>=0){
            if((bit8&0x80)==0){
                byte1=lz77CompressedTileTable[len];
                ++len;
                ++total;
            }else{
                ut8=lz77CompressedTileTable[len];
                ++len;
                byte1=ut8;
                ut8>>=4;
                ut8+=3;
                byte1&=0xF;
                bit16=byte1;
                bit16<<=8;
                byte1=lz77CompressedTileTable[len];
                ++len;
                bit16|=byte1;
                total+=ut8;
            }
            bit8<<=1;
            if(total>=definelen){
                break;
            }
        }
    }
    length=len;
}
