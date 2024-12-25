#include "font_update.h"
#include "ff.h"
#include "flash.h"
#include "tftlcd.h"
#include "string.h"
#include "malloc.h"
#include "SysTick.h"
#include "usart.h"


//????????????��(3???+unigbk?+??��?=3238700??,??791?W25QXX??)
#define FONTSECSIZE	 	791
//????????
#define FONTINFOADDR 	1024*1024*2 					//?12M??????????
//??12M?fatfs???.
//12M????3???+UNIGBK.BIN,??��3.09M,??????,???!
//15.10M??,????????.??????100K?????.
//??4M?fatfs???.


//?4??????��?��??��?��?
_font_info ftinfo;

//?????????��?
u8*const GBK_PATH[5]=
        {
//"/FONT/UNIGBK.BIN",	//UNIGBK.BIN???��?
//"/FONT/GBK12.FON",	//GBK12???��?
//"/FONT/GBK16.FON",	//GBK16???��?
//"/FONT/GBK24.FON",	//GBK24???��?
//"/FONT/GBK32.FON",	//GBK32???��?
                "/FONT/SanJiBang12.bin",	//GBK32???��?
                "/FONT/SanJiBang16.bin",
                "/FONT/SanJiBang20.bin",
        };
//?��????��?
u8*const UPDATE_REMIND_TBL[5]=
        {
//"Updating UNIGBK.BIN",	//?????��UNIGBK.bin
//"Updating GBK12.FON ",	//?????��GBK12
//"Updating GBK16.FON ",	//?????��GBK16
//"Updating GBK24.FON ",	//?????��GBK24
                "Updating SanJiBang12.bin ",	//?????��GBK32
                "Updating SanJiBang16.bin ",
                "Updating SanJiBang20.bin ",	//?????��GBK32
        };

//��??????��??
//x,y:??
//size:???��
//fsize:??��??��
//pos:??��???��?
u32 fupd_prog(u16 x,u16 y,u8 size,u32 fsize,u32 pos)
{
    float prog;
    u8 t=0XFF;
    prog=(float)pos/fsize;
    prog*=100;
    if(t!=prog)
    {
        LCD_ShowString(x+3*size/2,y,240,320,size,"%");
        t=prog;
        if(t>100)t=100;
        LCD_ShowNum(x,y,t,3,size);//��???
    }
    return 0;
}
//?��???
//x,y:??
//size:???��
//fxpath:��?
//fx:?��??? 0,ungbk;1,gbk12;2,gbk16;3,gbk24;4,gbk32;
//???:0,??;??,??.
u8 updata_fontx(u16 x,u16 y,u8 size,u8 *fxpath,u8 fx)
{
    u32 flashaddr=0;
    FIL * fftemp;
    u8 *tempbuf;
    u8 res;
    u16 bread;
    u32 offx=0;
    u8 rval=0;
    fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//????
    if(fftemp==NULL)rval=1;
    tempbuf=mymalloc(SRAMIN,4096);				//??4096?????
    if(tempbuf==NULL)rval=1;
    res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ);
    if(res)rval=2;//??��???
    if(rval==0)
    {
        switch(fx)
        {
//			case 0:												//?��UNIGBK.BIN
//				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//��????��??UNIGBK????
//				ftinfo.ugbksize=fftemp->obj.objsize;					//UNIGBK?��
//				flashaddr=ftinfo.ugbkaddr;
//				break;
            case 1:
                ftinfo.f12addr=FONTINFOADDR+sizeof(ftinfo);	//UNIGBK??��??GBK12??
                ftinfo.gbk12size=fftemp->obj.objsize;					//GBK12???��
                flashaddr=ftinfo.f12addr;						//GBK12?????
                printf("ftinfo.f12addr is %lu\r\n",flashaddr);
                break;
            case 2:
                ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;	//GBK12??��??GBK16??
                ftinfo.gbk16size=fftemp->obj.objsize;					//GBK16???��
                flashaddr=ftinfo.f16addr;
                printf("ftinfo.f16addr is %lu\r\n",flashaddr);
                //GBK16?????
                break;
            case 3:
                ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16??��??GBK24??
                ftinfo.gbk24size=fftemp->obj.objsize;					//GBK24???��
                flashaddr=ftinfo.f24addr;						//GBK24?????
                printf("ftinfo.f24addr is %lu\r\n",flashaddr);

                break;
            case 4:
                ftinfo.f32addr=ftinfo.f24addr+ftinfo.gbk24size;	//GBK24??��??GBK32??
                ftinfo.gbk32size=fftemp->obj.objsize;					//GBK32???��
                flashaddr=ftinfo.f32addr;						//GBK32?????
                break;
        }

        while(res==FR_OK)//?????
        {
            res=f_read(fftemp,tempbuf,4096,(UINT *)&bread);		//????
            if(res!=FR_OK)break;								//????
            EN25QXX_Write(tempbuf,offx+flashaddr,4096);		//?0??��?4096???
            offx+=bread;
            fupd_prog(x,y,size,fftemp->obj.objsize,offx);	 			//??��?
            if(bread!=4096)break;								//???.
        }
        f_close(fftemp);
    }
    myfree(SRAMIN,fftemp);	//????
    myfree(SRAMIN,tempbuf);	//????
    return res;
}
//?��??��?,UNIGBK,GBK12,GBK16,GBK24,GBK32???��
//x,y:??��??��???
//size:???��
//src:??4???."0:",SD?;"1:",FLASH?,"2:"NAND?,"3:",U?.
//??��????��
//???:0,?��??;
//		 ??,????.
u8 update_font(u16 x,u16 y,u8 size,u8* src)
{
    u8 *pname;
    u32 *buf;
    u8 res=0;
    u16 i,j;
    FIL *fftemp;
    u8 rval=0;
    res=0XFF;
    ftinfo.fontok=0XFF;
    pname=mymalloc(SRAMIN,100);	//??100????
    buf=mymalloc(SRAMIN,4096);	//??4K????
    fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//????
    if(buf==NULL||pname==NULL||fftemp==NULL)
    {
        myfree(SRAMIN,fftemp);
        myfree(SRAMIN,pname);
        myfree(SRAMIN,buf);
        return 5;		//??????
    }
    for(i=0;i<=2;i++)	//???��?UNIGBK,GBK12,GBK16,GBK24,GBK32��???
    {
        strcpy((char*)pname,(char*)src);				//copy src???pname
        strcat((char*)pname,(char*)GBK_PATH[i]);		//????��?��?
        res=f_open(fftemp,(const TCHAR*)pname,FA_READ);	//????
        if(res)
        {
            rval|=1<<7;	//????��???
            break;		//???,????
        }
    }
    myfree(SRAMIN,fftemp);	//????
    if(rval==0)				//??��????.
    {
//		LCD_ShowString(x,y,240,320,size,"Erasing sectors... ");//????????
//		for(i=0;i<FONTSECSIZE;i++)			//???????,??��???
//		{
//			fupd_prog(x+20*size/2,y,size,FONTSECSIZE,i);//??��?
//			EN25QXX_Read((u8*)buf,((FONTINFOADDR/4096)+i)*4096,4096);//?????????
//			for(j=0;j<1024;j++)//��???
//			{
//				if(buf[j]!=0XFFFFFFFF)break;//????
//			}
//			if(j!=1024)EN25QXX_Erase_Sector((FONTINFOADDR/4096)+i);	//???????
//		}
        for(i=0;i<=2;i++)	//???��UNIGBK,GBK12,GBK16,GBK24,GBK32
        {
            LCD_ShowString(x,y,240,320,size,UPDATE_REMIND_TBL[i]);
            strcpy((char*)pname,(char*)src);				//copy src???pname
            strcat((char*)pname,(char*)GBK_PATH[i]); 		//????��?��?
            res=updata_fontx(x+20*size/2,y,size,pname,i+1);	//?��??
            if(res)
            {
                myfree(SRAMIN,buf);
                myfree(SRAMIN,pname);
                return 1+i;
            }
        }
        //???��??
        ftinfo.fontok=0X55;
        EN25QXX_Write((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//????��?
    }
    myfree(SRAMIN,pname);//????
    myfree(SRAMIN,buf);
    return rval;//???.
}
//?????
//???:0,????.
//		 ??,????
u8 font_init(void)
{
    u8 t=0;
    EN25QXX_Init();
    while(t<10)//l???10?,?��??,????��???,??��???
    {
        t++;
        EN25QXX_Read((u8*)&ftinfo,FONTINFOADDR,sizeof(ftinfo));//??ftinfo?????
        if(ftinfo.fontok==0X55)break;
        delay_ms(20);
    }
    if(ftinfo.fontok!=0X55)return 1;
    return 0;
}
