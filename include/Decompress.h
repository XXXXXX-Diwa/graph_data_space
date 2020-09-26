#ifndef DECOMPRESS_H_INCLUDED
#define DECOMPRESS_H_INCLUDED

#include <iostream>
#include <fstream>
/**
*��arm7(32 mode)������ݽ�����lz777ѹ�����ݽ�ѹ��ʽ,ѹ����ʽ����
*
*��thumb(16 mode)������ݽ�����relѹ�����ݽ�ѹ��ʽ,ѹ����ʽ����
**/

/*
;r0��lz77���ݵ�ַ(��4�ֽ�) r1��Ҫд����ڴ��ַ

00000BA4>E35C0000 cmp     r12,0h		;���r12�Ƿ�Ϊ0 Ҳ����5-8�ֽ�
00000BA8 0A000003 beq     @@end
00000BAC E3CCC4FE bic     r12,r12,0FE000000h	;bic��ֵ ȥ��8000000 ���Ǳ�����û�а�
00000BB0 E080C00C add     r12,r0,r12			;�������ݵ�ַ
00000BB4 E310040E tst     r0,0E000000h			;��λ�벻������,�ı��־
00000BB8 131C040E tstne   r12,0E000000h			;ͬ��,�൱�ڼ���Ƿ���0x8000000,û����Z���1

@@end:
00000BBC E12FFF1E bx      r14


000010FC>E92D4070 stmfd   [r13]!,r4-r6,r14	;ѹջ ����Ĵ���
00001100 E4905004 ldr     r5,[r0],4h		;r5��ȡr0 r0+4h
00001104 E1A02425 mov     r2,r5,lsr 8h		;r5��r2��������8h	��ѹ����ܳ���
00001108 E1B0C002 movs    r12,r2			;��r12	��ѹ����ܳ���
0000110C EBFFFEA4 bl      0BA4h
00001110 0A00001D beq     @@end				;һ����˵������

@@loop:
00001114 E3520000 cmp     r2,0h				;����ѹ����
00001118 DA00001B ble     @@end
0000111C E4D0E001 ldrb    r14,[r0],1h		;��ȡlz77���ݲ�ָ������
00001120 E3A04008 mov     r4,8h				;r4=8 Ĭ�����ݶ�д�ĳ���

@@loop2:
00001124 E2544001 subs    r4,r4,1h			;Ȼ���1
00001128 BAFFFFF9 blt     @@loop			;ֻ��С��0�Ż���ת
0000112C E31E0080 tst     r14,80h			;��ȡ��������80��λ����ı���
00001130 1A000003 bne     @@hava80			;����80��Ϊ0����ת ���־λ��Ϊ0��ת
00001134 E4D06001 ldrb    r6,[r0],1h		;��ȡ���ݲ�ָ������
00001138 E4C16001 strb    r6,[r1],1h		;����д���ڴ沢ָ������
0000113C E2422001 sub     r2,r2,1h			;�ܳ��ȼ�1
00001140 EA00000D b       @@goto

@@hava80:
00001144 E5D05000 ldrb    r5,[r0]			;��ȡ����
00001148 E3A06003 mov     r6,3h
0000114C E0863245 add     r3,r6,r5,asr 4h	;������������4Ȼ�����3��r3
00001150 E4D06001 ldrb    r6,[r0],1h		;�ٴζ�ȡ��һ�ε�����
00001154 E206500F and     r5,r6,0Fh			;����and0F��r5
00001158 E1A0C405 mov     r12,r5,lsl 8h		;r5����8h��r12
0000115C E4D06001 ldrb    r6,[r0],1h		;��ȡ����
00001160 E186500C orr     r5,r6,r12			;��r12 orr�� r5
00001164 E285C001 add     r12,r5,1h			;r5��1��r12
00001168 E0422003 sub     r2,r2,r3			;����-r3

@@loop3:
0000116C E751500C ldrb    r5,[r1,-r12]		;��ȡ�ڴ��ַ��r5
00001170 E4C15001 strb    r5,[r1],1h		;д����ǰ���ڴ��ַȻ���1
00001174 E2533001 subs    r3,r3,1h			;r3�ݼ�
00001178 CAFFFFFB bgt     @@loop3			;r3��Ϊ0�ͻ���ת

@@goto:
0000117C E3520000 cmp     r2,0h				;����Ƿ�Ϊ��
00001180 C1A0E08E movgt   r14,r14,lsl 1h	;���ֽ�����1λ
00001184 CAFFFFE6 bgt     @@loop2
00001188 EAFFFFE1 b       @@loop

@@end:
0000118C E8BD4070 ldmfd   [r13]!,r4-r6,r14
00001190 E12FFF1E bx      r14
*/

/*
;r0=1h,r1=relѹ�����ݵ�ַ,r2=Ҫд����ڴ��ַ

08056D18 B570     push    r4-r6,lr
08056D1A B081     add     sp,-4h
08056D1C 1C0C     mov     r4,r1		;���ݵ�ַ
08056D1E 1C15     mov     r5,r2		;�ڴ��ַ
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
08056D4A 9000     str     r0,[sp]	;sp��д��10h bitsize?
08056D4C 2003     mov     r0,3h		;DMA channel
08056D4E 2100     mov     r1,0h		;ֵ..
08056D50 1C2A     mov     r2,r5		;�ڴ��ַ
08056D52 F7ACFAAF bl      80032B4h	;fill
08056D56 2300     mov     r3,0h

@@restart:
08056D58 1C2A     mov     r2,r5		;�ڴ��ַ
08056D5A 2B00     cmp     r3,0h
08056D5C D000     beq     @@Pass
08056D5E 3201     add     r2,1h		;�ڴ��ַ�ݽ�1??һ�㶼�Ǽ�2��

@@Pass:
08056D60 7820     ldrb    r0,[r4]	;��ȡѹ������1
08056D62 3401     add     r4,1h		;��ַ�ݽ�  rel����ͷ����01��ʼ
08056D64 2801     cmp     r0,1h		;ֵ��Ϊ1
08056D66 D126     bne     @@firstno1
08056D68 7821     ldrb    r1,[r4]	;��ȡѹ������2
08056D6A 3401     add     r4,1h		;��ַ�ݽ�
08056D6C 3301     add     r3,1h		;��ѹ�����ݵ�һ���ֽ�Ϊ1 r3��1
08056D6E 2900     cmp     r1,0h		;rel����ͷ�ڶ����ֽ�һ�㲻Ϊ0
08056D70 D053     beq     @@secondzero ;�����൱��ֱ�ӽ�����

@@lastbecomesecond:
08056D72 2080     mov     r0,80h
08056D74 4008     and     r0,r1		;��ȡ�����ݼ���Ƿ���80
08056D76 2800     cmp     r0,0h
08056D78 D016     beq     @@secondNo80
08056D7A 207F     mov     r0,7Fh	;��80�Ļ�
08056D7C 4001     and     r1,r0		;ֻȡС��80�Ĳ���
08056D7E 7820     ldrb    r0,[r4]	;��ȡ�������ֽ�
08056D80 2800     cmp     r0,0h
08056D82 D008     beq     @@thirdzero
08056D84 2900     cmp     r1,0h		;�ڶ��ֽ�ֻȡС��80h�Ĳ���Ϊ0
08056D86 D008     beq     @@secondand7Fzero

@@loop:
08056D88 7820     ldrb    r0,[r4]	;�ٴζ�ȡ�������ֽ�
08056D8A 7010     strb    r0,[r2]	;д���ڴ��ַ
08056D8C>3202     add     r2,2h		;�ڴ��ַ����2
08056D8E 3901     sub     r1,1h		;�ڶ����ֽ�and80�����Ƿ�Ϊ1
08056D90 2900     cmp     r1,0h
08056D92 D1F9     bne     @@loop
08056D94 E001     b       @@secondand7Fzero

@@thirdzero:
08056D96 0048     lsl     r0,r1,1h	;�������ֽ�Ϊ0,��ڶ����ֽ�(��80)�����������ٸ��ڴ�
08056D98 1812     add     r2,r2,r0

@@secondand7Fzero:					;�ڶ����ֽ���80���ǽ���ֻ��80,û��������ֵ
08056D9A 3401     add     r4,1h		;�����Ļ��͵��������ĸ�ֵ,Ȼ�������ֵ��ͬ�ڶ���ֵһ�������
08056D9C E006     b       @@secondpass

@@copy:
08056D9E 7820     ldrb    r0,[r4]	;��ȡ�������ֽ�
08056DA0 7010     strb    r0,[r2]	;д���ڴ�
08056DA2 3401     add     r4,1h		;���ݵ�ַ����
08056DA4 3202     add     r2,2h		;�ڴ�+2
08056DA6 3901     sub     r1,1h		;�ڶ����ֽڼ�1

@@secondNo80:
08056DA8 2900     cmp     r1,0h		;���û��80�ĵڶ����ֽ��Ƿ�Ϊ0
08056DAA D1F8     bne     @@copy

@@secondpass:
08056DAC 7821     ldrb    r1,[r4]	;Ϊ0�Ļ���ȡ�������ֽ�
08056DAE 3401     add     r4,1h		;���ݵ���
08056DB0 2900     cmp     r1,0h		;���������ֽ��Ƿ�Ϊ0
08056DB2 D1DE     bne     @@lastbecomesecond	;����ڶ����ֽ�һ�����������ֽ��Ƿ���80
08056DB4 E031     b       @@secondzero

@@firstno1:
08056DB6 7821     ldrb    r1,[r4]		;��ȡ�ڶ����ֽ�
08056DB8 3401     add     r4,1h			;���ݵ���
08056DBA 0209     lsl     r1,r1,8h		;������λ8bit
08056DBC 7820     ldrb    r0,[r4]		;��ȡ�������ֽ�
08056DBE 4301     orr     r1,r0			;�ڶ����ֽڸ�λ�͵������ֽڵ�λ�ϲ�Ϊ16bit
08056DC0 3401     add     r4,1h			;���ݵ���
08056DC2 3301     add     r3,1h			;��������2�ֽڵĶ�ȡ,r3Ҳ����
08056DC4 2900     cmp     r1,0h			;��������Ϊ0
08056DC6 D028     beq     @@secondzero	;�����൱��ֱ�ӽ�����

@@bit16re:
08056DC8 2080     mov     r0,80h
08056DCA 0200     lsl     r0,r0,8h		;8000
08056DCC 4008     and     r0,r1			;�ڶ����ֽ��Ƿ���80
08056DCE 2800     cmp     r0,0h
08056DD0 D019     beq     @@SecondNo802
08056DD2 4807     ldr     r0,=7FFFh
08056DD4 4001     and     r1,r0			;ȥ���ڶ����ֽڵ�80
08056DD6 7820     ldrb    r0,[r4]		;��ȡ���ĸ��ֽ�
08056DD8 2800     cmp     r0,0h
08056DDA D00B     beq     @@fourthzero
08056DDC 2900     cmp     r1,0h			;�ϲ���ȥ��8000��Ϊ0
08056DDE D00B     beq     @@secondandthirdorr7Fzero

@@bit16loop:
08056DE0 7820     ldrb    r0,[r4]		;��ȡ���ĸ��ֽ�
08056DE2 7010     strb    r0,[r2]		;д���ڴ�
08056DE4 3202     add     r2,2h			;�ڴ�+2
08056DE6 3901     sub     r1,1h			;�ϲ������ݼ�
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
08056E1A 2B01     cmp     r3,1h		;�����;�л��˵��ֽں�˫�ֽ������
08056E1C DD9C     ble     @@restart
08056E1E 1C30     mov     r0,r6
08056E20 B001     add     sp,4h
08056E22 BC70     pop     r4-r6
08056E24 BC02     pop     r1
08056E26 4708     bx      r1
*/

using namespace std;
class RelBgData{       //�������
public:
    uint8_t roomWidth;
    uint8_t roomHeigh;
    uint8_t relCompressedData[0xF218];
    uint32_t length;
    RelBgData();
    ~RelBgData();
    void getRelCompressData(ifstream &inf);
};

class Lz77BgData{      //��λ�����4����
public:
    uint32_t bgSize;    //0 = 256x256, 1 = 512x256, 2 = 256x512 ֻ����һ�ֽ�
    uint32_t definelen;
    uint32_t decompressedLen;   //�������ֽ�
    uint8_t lz77CompressedTileTable[0x8000];
    uint32_t length;
    Lz77BgData();
    ~Lz77BgData();
    void getLz77CompressData(ifstream &inf,bool byte2flag);
};

#endif // DECOMPRESS_H_INCLUDED
