#ifndef _font_update_H
#define _font_update_H

#include "system.h"
// ��pack�Ͳ���pack�Ĳ�ͬ��
//���Ƚṹ����,����λ�����,���������������Щλ
//��pack�����ֽ���������������
//����pack����������벻�������������������Ǳ�����ʵ�ʲ������Щ���������,
//����������Ҫ����Щ������ĵط�д "((u8*)dest)[i]""�����Щ�ط�����ŵ���ȷ��λ��,�ṹ����ִ�λ,������������"
//�˹�д((u8*)dest)[i]���Է��ʵ���Щ���������"��������Ч���ݾ�������"
//Aligned: fontok=1, ugbkaddr=134678021, ugbksize=202050057
//Packed: fontok=1, ugbkaddr=84148994, ugbksize=151521030
//fontok Aligned�������Զ����3���������� 0x2, 0x3, 0x4����������Ѩ�������˶�����
//ugbkaddr���� 0x5 0x6 0x7 0x8Ϊ 0x08070605 Ϊ134678021 ���134678021����
//size���� 0x09,0x0A,0x0B,0X0C Ϊ0x0C0B0A09 Ϊ202050057 print���202050057����
//fontok Packed����������Ѩ�������˶�����
//ugbkaddr���� 0x2, 0x3, 0x4, 0x5 0x05040302 Ϊ84,148,994 ��84148994����
//size���� 	   0x6, 0x7, 0x8,0x09 Ϊ0x09080706 Ϊ151521030 print���151521030����


extern u32 FONTINFOADDR;

typedef struct __attribute__((packed)) {
    u8 fontok;                //??????��0X55��???????��?????
    u32 ugbkaddr;            //unigbk???
    u32 ugbksize;            //unigbk??��
    u32 f12addr;            //gbk12??
    u32 gbk12size;            //gbk12??��
    u32 f16addr;            //gbk16??
    u32 gbk16size;            //gbk16??��
    u32 f24addr;            //gbk24??
    u32 gbk24size;            //gbk24??��
    u32 f32addr;            //gbk32??
    u32 gbk32size;            //gbk32??��
} _font_info;



extern _font_info ftinfo;    //??��????

u32 fupd_prog(u16 x, u16 y, u8 size, u32 fsize, u32 pos);    //��??��??
u8 updata_fontx(u16 x, u16 y, u8 size, u8 *fxpath, u8 fx);    //?��????
u8 update_font(u16 x, u16 y, u8 size, u8 *src);            //?��????
u8 font_init(void);                                        //?????


#endif
