#ifndef DECOMPRESS_H_INCLUDED
#define DECOMPRESS_H_INCLUDED

#include <iostream>
#include <fstream>
/**
*从arm7(32 mode)汇编数据解析的lz777压缩数据解压方式,压缩方式不明
*
*从thumb(16 mode)汇编数据解析的rel压缩数据解压方式,压缩方式不明
**/

/*
;r0是lz77数据地址(跳4字节) r1是要写入的内存地址

00000BA4>E35C0000 cmp     r12,0h		;检查r12是否为0 也就是5-8字节
00000BA8 0A000003 beq     @@end
00000BAC E3CCC4FE bic     r12,r12,0FE000000h	;bic该值 去除8000000 但是本来就没有啊
00000BB0 E080C00C add     r12,r0,r12			;加上数据地址
00000BB4 E310040E tst     r0,0E000000h			;按位与不保存结果,改变标志
00000BB8 131C040E tstne   r12,0E000000h			;同上,相当于检查是否有0x8000000,没有则Z标记1

@@end:
00000BBC E12FFF1E bx      r14


000010FC>E92D4070 stmfd   [r13]!,r4-r6,r14	;压栈 保存寄存器
00001100 E4905004 ldr     r5,[r0],4h		;r5读取r0 r0+4h
00001104 E1A02425 mov     r2,r5,lsr 8h		;r5给r2并且右移8h	解压后的总长度
00001108 E1B0C002 movs    r12,r2			;给r12	解压后的总长度
0000110C EBFFFEA4 bl      0BA4h
00001110 0A00001D beq     @@end				;一般来说不会跳

@@loop:
00001114 E3520000 cmp     r2,0h				;检查减压长度
00001118 DA00001B ble     @@end
0000111C E4D0E001 ldrb    r14,[r0],1h		;读取lz77数据并指针下移
00001120 E3A04008 mov     r4,8h				;r4=8 默认数据读写的长度

@@loop2:
00001124 E2544001 subs    r4,r4,1h			;然后减1
00001128 BAFFFFF9 blt     @@loop			;只有小于0才会跳转
0000112C E31E0080 tst     r14,80h			;读取的数据与80按位与仅改变标记
00001130 1A000003 bne     @@hava80			;当与80不为0则跳转 零标志位不为0跳转
00001134 E4D06001 ldrb    r6,[r0],1h		;读取数据并指针下移
00001138 E4C16001 strb    r6,[r1],1h		;数据写入内存并指针下移
0000113C E2422001 sub     r2,r2,1h			;总长度减1
00001140 EA00000D b       @@goto

@@hava80:
00001144 E5D05000 ldrb    r5,[r0]			;读取数据
00001148 E3A06003 mov     r6,3h
0000114C E0863245 add     r3,r6,r5,asr 4h	;数据算术右移4然后加上3给r3
00001150 E4D06001 ldrb    r6,[r0],1h		;再次读取上一次的数据
00001154 E206500F and     r5,r6,0Fh			;数据and0F给r5
00001158 E1A0C405 mov     r12,r5,lsl 8h		;r5左移8h给r12
0000115C E4D06001 ldrb    r6,[r0],1h		;读取数据
00001160 E186500C orr     r5,r6,r12			;和r12 orr给 r5
00001164 E285C001 add     r12,r5,1h			;r5加1给r12
00001168 E0422003 sub     r2,r2,r3			;总数-r3

@@loop3:
0000116C E751500C ldrb    r5,[r1,-r12]		;读取内存地址给r5
00001170 E4C15001 strb    r5,[r1],1h		;写进当前的内存地址然后加1
00001174 E2533001 subs    r3,r3,1h			;r3递减
00001178 CAFFFFFB bgt     @@loop3			;r3不为0就会跳转

@@goto:
0000117C E3520000 cmp     r2,0h				;检查是否为零
00001180 C1A0E08E movgt   r14,r14,lsl 1h	;首字节左移1位
00001184 CAFFFFE6 bgt     @@loop2
00001188 EAFFFFE1 b       @@loop

@@end:
0000118C E8BD4070 ldmfd   [r13]!,r4-r6,r14
00001190 E12FFF1E bx      r14
*/

/*
;r0=1h,r1=rel压缩数据地址,r2=要写入的内存地址

08056D18 B570     push    r4-r6,lr
08056D1A B081     add     sp,-4h
08056D1C 1C0C     mov     r4,r1		;数据地址
08056D1E 1C15     mov     r5,r2		;内存地址
08056D20 0600     lsl     r0,r0,18h
08056D22 2600     mov     r6,0h
08056D24 23C0     mov     r3,0C0h
08056D26 019B     lsl     r3,r3,6h	;3000h
08056D28 2800     cmp     r0,0h
08056D2A D10D     bne     @@NoZero
08056D2C 7820     ldrb    r0,[r4]
08056D2E 2680     mov     r6,80h
08056D30 0136     lsl     r6,r6,4h	;800
08056D32 2800     cmp     r0,0h
08056D34 D005     beq     @@Zero
08056D36 2680     mov     r6,80h
08056D38 0176     lsl     r6,r6,5h	;1000
08056D3A 2803     cmp     r0,3h
08056D3C D101     bne     @@Zero
08056D3E 2680     mov     r6,80h
08056D40 01B6     lsl     r6,r6,6h	;2000

@@Zero:
08056D42 3401     add     r4,1h
08056D44 2380     mov     r3,80h
08056D46 019B     lsl     r3,r3,6h	;2000

@@NoZero:
08056D48 2010     mov     r0,10h
08056D4A 9000     str     r0,[sp]	;sp中写入10h bitsize?
08056D4C 2003     mov     r0,3h		;DMA channel
08056D4E 2100     mov     r1,0h		;值..
08056D50 1C2A     mov     r2,r5		;内存地址
08056D52 F7ACFAAF bl      80032B4h	;fill
08056D56 2300     mov     r3,0h

@@restart:
08056D58 1C2A     mov     r2,r5		;内存地址
08056D5A 2B00     cmp     r3,0h
08056D5C D000     beq     @@Pass
08056D5E 3201     add     r2,1h		;内存地址递进1??一般都是加2的

@@Pass:
08056D60 7820     ldrb    r0,[r4]	;读取压缩数据1
08056D62 3401     add     r4,1h		;地址递进  rel数据头都是01开始
08056D64 2801     cmp     r0,1h		;值不为1
08056D66 D126     bne     @@firstno1
08056D68 7821     ldrb    r1,[r4]	;读取压缩数据2
08056D6A 3401     add     r4,1h		;地址递进
08056D6C 3301     add     r3,1h		;若压缩数据第一个字节为1 r3加1
08056D6E 2900     cmp     r1,0h		;rel数据头第二个字节一般不为0
08056D70 D053     beq     @@secondzero ;基本相当于直接结束了

@@lastbecomesecond:
08056D72 2080     mov     r0,80h
08056D74 4008     and     r0,r1		;读取的数据检查是否有80
08056D76 2800     cmp     r0,0h
08056D78 D016     beq     @@secondNo80
08056D7A 207F     mov     r0,7Fh	;有80的话
08056D7C 4001     and     r1,r0		;只取小于80的部分
08056D7E 7820     ldrb    r0,[r4]	;读取第三个字节
08056D80 2800     cmp     r0,0h
08056D82 D008     beq     @@thirdzero
08056D84 2900     cmp     r1,0h		;第二字节只取小于80h的部分为0
08056D86 D008     beq     @@secondand7Fzero

@@loop:
08056D88 7820     ldrb    r0,[r4]	;再次读取第三个字节
08056D8A 7010     strb    r0,[r2]	;写入内存地址
08056D8C>3202     add     r2,2h		;内存地址递增2
08056D8E 3901     sub     r1,1h		;第二个字节and80后检查是否为1
08056D90 2900     cmp     r1,0h
08056D92 D1F9     bne     @@loop
08056D94 E001     b       @@secondand7Fzero

@@thirdzero:
08056D96 0048     lsl     r0,r1,1h	;第三个字节为0,则第二个字节(有80)代表跳过多少个内存
08056D98 1812     add     r2,r2,r0

@@secondand7Fzero:					;第二个字节有80但是仅仅只是80,没有其它的值
08056D9A 3401     add     r4,1h		;这样的话就递增到第四个值,然后第三个值如同第二个值一样被检查
08056D9C E006     b       @@secondpass

@@copy:
08056D9E 7820     ldrb    r0,[r4]	;读取第三个字节
08056DA0 7010     strb    r0,[r2]	;写进内存
08056DA2 3401     add     r4,1h		;数据地址递增
08056DA4 3202     add     r2,2h		;内存+2
08056DA6 3901     sub     r1,1h		;第二个字节减1

@@secondNo80:
08056DA8 2900     cmp     r1,0h		;检查没有80的第二个字节是否为0
08056DAA D1F8     bne     @@copy

@@secondpass:
08056DAC 7821     ldrb    r1,[r4]	;为0的话读取第三个字节
08056DAE 3401     add     r4,1h		;数据递增
08056DB0 2900     cmp     r1,0h		;检查第三个字节是否为0
08056DB2 D1DE     bne     @@lastbecomesecond	;像检查第二个字节一样检查第三个字节是否有80
08056DB4 E031     b       @@secondzero

@@firstno1:
08056DB6 7821     ldrb    r1,[r4]		;读取第二个字节
08056DB8 3401     add     r4,1h			;数据递增
08056DBA 0209     lsl     r1,r1,8h		;向左移位8bit
08056DBC 7820     ldrb    r0,[r4]		;读取第三个字节
08056DBE 4301     orr     r1,r0			;第二个字节高位和第三个字节低位合并为16bit
08056DC0 3401     add     r4,1h			;数据递增
08056DC2 3301     add     r3,1h			;若进行了2字节的读取,r3也递增
08056DC4 2900     cmp     r1,0h			;两者若都为0
08056DC6 D028     beq     @@secondzero	;基本相当于直接结束了

@@bit16re:
08056DC8 2080     mov     r0,80h
08056DCA 0200     lsl     r0,r0,8h		;8000
08056DCC 4008     and     r0,r1			;第二个字节是否有80
08056DCE 2800     cmp     r0,0h
08056DD0 D019     beq     @@SecondNo802
08056DD2 4807     ldr     r0,=7FFFh
08056DD4 4001     and     r1,r0			;去掉第二个字节的80
08056DD6 7820     ldrb    r0,[r4]		;读取第四个字节
08056DD8 2800     cmp     r0,0h
08056DDA D00B     beq     @@fourthzero
08056DDC 2900     cmp     r1,0h			;合并的去掉8000若为0
08056DDE D00B     beq     @@secondandthirdorr7Fzero

@@bit16loop:
08056DE0 7820     ldrb    r0,[r4]		;读取第四个字节
08056DE2 7010     strb    r0,[r2]		;写进内存
08056DE4 3202     add     r2,2h			;内存+2
08056DE6 3901     sub     r1,1h			;合并的数递减
08056DE8 2900     cmp     r1,0h
08056DEA D1F9     bne     @@bit16loop
08056DEC E004     b       @@secondandthirdorr7Fzero
.pool

@@fourthzero:
08056DF4 0048     lsl     r0,r1,1h
08056DF6 1812     add     r2,r2,r0

@@secondandthirdorr7Fzero:
08056DF8 3401     add     r4,1h
08056DFA E006     b       @@bit16Zero

@@loop2:
08056DFC 7820     ldrb    r0,[r4]
08056DFE 7010     strb    r0,[r2]
08056E00 3401     add     r4,1h
08056E02 3202     add     r2,2h
08056E04 3901     sub     r1,1h

@@SecondNo802:
08056E06 2900     cmp     r1,0h
08056E08 D1F8     bne     @@loop2

@@bit16Zero:
08056E0A 7821     ldrb    r1,[r4]
08056E0C 3401     add     r4,1h
08056E0E 0209     lsl     r1,r1,8h
08056E10 7820     ldrb    r0,[r4]
08056E12 4301     orr     r1,r0
08056E14 3401     add     r4,1h
08056E16 2900     cmp     r1,0h
08056E18 D1D6     bne     @@bit16re

@@secondzero:
08056E1A 2B01     cmp     r3,1h		;如果中途切换了单字节和双字节则结束
08056E1C DD9C     ble     @@restart
08056E1E 1C30     mov     r0,r6
08056E20 B001     add     sp,4h
08056E22 BC70     pop     r4-r6
08056E24 BC02     pop     r1
08056E26 4708     bx      r1
*/

using namespace std;
class RelBgData{       //无需对齐
public:
    uint8_t roomWidth;
    uint8_t roomHeigh;
    uint8_t relCompressedData[0xF218];
    uint32_t length;
    RelBgData();
    ~RelBgData();
    void getRelCompressData(ifstream &inf);
};

class Lz77BgData{      //首位最好与4对齐
public:
    uint32_t bgSize;    //0 = 256x256, 1 = 512x256, 2 = 256x512 只用了一字节
    uint32_t definelen;
    uint32_t decompressedLen;   //用了三字节
    uint8_t lz77CompressedTileTable[0x8000];
    uint32_t length;
    Lz77BgData();
    ~Lz77BgData();
    void getLz77CompressData(ifstream &inf,bool byte2flag);
};

#endif // DECOMPRESS_H_INCLUDED
