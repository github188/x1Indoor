

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include "au_types.h"
#include "storage_ini.h"


#define MAX_LINE_LEN		255
#define MAX_FILESIZE 		32768  //最大配置文件大小
#define INT_SIZE   			sizeof(int32)
#define MAX_FIELD       128

static char WriteBuf[MAX_FILESIZE];
static int32  BufChanged=0;
static char PrevFile[MAX_LINE_LEN]="";
 
#define validchar(x) (((x)>= 32 && (x)<=126) || ((x)& 0x80))

/*************************************************
  Function:    valid_string
  Description: 判断指定的字符串是否有效
  Input:       
	1.指定的字符串指针
  返回值: 
        
*************************************************/
static int32 validstring(char *s)
{
   while(*s)
   {
	  if(!validchar(*s)) return 0;
	  s++;
   }
   return 1;   
}

/*************************************************
  Function:    read_line
  Description: 读取指定的文件中一行到一字符串中
  Input:       
	1.文件指针
	2.欲存储的字符串指针
  返回值: 
  1.读取的最后一个字符          
*************************************************/
static int32 read_line(FILE *f,char *s) 
{
  int32 c;
  do{
	 c=fgetc(f);
	 *(s++)=c;
  }while(c!=EOF&&c!='\xa'&&c!='\xd');
  *(s-1)='\0';
  return c;
}

/*************************************************
  Function:    del_space
  Description: 去除指定字符串头或者结尾的空字符
  Input:       
	1.指定字符串指针	
  返回值: 
  1.处理后的字符串指针          
*************************************************/
static char *delete_space(char *sp)
{
	char *tp;
	tp=sp+strlen(sp)-1;
	while(*tp==' ')	{*tp='\0';tp--;}
	while(*sp==' ') sp++;
	return sp;
}

/*************************************************
  Function:    get_line
  Description: 从原字符串拷贝指定大小的字符到目标字符串
  						 碰到换行或者回车自动结束.原字符串指针指
  						 到下一行
  Input:       
	1.原字符串指针
	2.目标字符串指针
	3.欲拷贝大小
  返回值: 
  1.处理后的原字符串指针          
*************************************************/
static char *get_line(char *sp,char *dest,int32 maxlen)
{
	while(maxlen--){
		if(*sp&&*sp!=13&&*sp!=10)	
			*(dest++)=*(sp++);
		else	
			break;
	}
	*dest='\0';
	while((*sp==10||*sp==13)&&*sp)	sp++;
	return sp;
}

/*************************************************
  Function:    delete_line
  Description: 删除指定的缓冲区中的一行,并后内容往前移
  Input:       
	1.指定的文件缓冲区指针
  返回值: 
  无
          
*************************************************/
static void delete_line(char *sp)
{
	char *cp;
	cp=sp;
	while(*sp&&*sp!=10&&*sp!=13)	sp++;
	while((*sp==10||*sp==13)&&*sp)	sp++;
	while(*sp)	*(cp++)=*(sp++);
	*cp=*sp;
}

/*************************************************
  Function:    		insert_line
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
static void insert_line(char *sp,char *line)
{
	int32 i,l,end;
	l=strlen(line);
	end=strlen(sp);
	for(i=end;i>=0;i--)		sp[i+l]=sp[i];
	for(i=0;i<l;i++)		sp[i]=line[i];
}

/*************************************************
  Function:    		write_inifile
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
void write_inifile(char *inifile)
{
	FILE *Inif;
	int32 size;
	
	Inif=fopen(PrevFile,"wb");
	if(Inif)
	{	
			size=strlen(WriteBuf);
			fwrite(WriteBuf,size,1,Inif);
			fclose(Inif);
	}
}

/*************************************************
  Function:    flush_buffer
  Description: 刷新指定的文件内容缓冲区
  Input:       
	1.指定的文件路径指针
  返回值: 
          
*************************************************/
int32 flush_buffer(char *inifile)
{
	FILE *Inif;
	int32 size;

	if(strcmp(PrevFile,inifile)!=0){//if target file changed	
		if(BufChanged)
		{
			Inif=fopen(PrevFile,"wb");
			if(Inif){	//write file
				size=strlen(WriteBuf);
				fwrite(WriteBuf,size,1,Inif);
				fclose(Inif);
			}			
		}
		BufChanged=0;
		Inif=NULL;
		if(inifile[0])	
			Inif=fopen(inifile,"a+");
		if(Inif)
		{	
			size=fread(WriteBuf,1,sizeof(WriteBuf),Inif);			
			fclose(Inif);
			Inif=NULL;
			strcpy(PrevFile,inifile);
			WriteBuf[size]='\0';
		}
		else 
		{
			return 1;
		}
	}
	return 0;
}

/*************************************************
  Function:    		write_ini
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 write_ini(char *inifile,char *section,char *name,char *value)
{
	int32		found = 0;
	char	*sp,*np,*vp,buf[MAX_LINE_LEN],seg[MAX_LINE_LEN];

	if(flush_buffer(inifile))	return 1;

	if(!validstring(value)||!validstring(name)||!validstring(section)) 
	{
		return false;
	}
	sprintf(seg,"[%s]",section);
	sp=WriteBuf;
	found=0;
	while(*sp)
	{
		sp=get_line(sp,buf,MAX_LINE_LEN);
		if(!strcmp(seg,buf)){
			while(1)
			{
				np=get_line(sp,buf,MAX_LINE_LEN);
				if(*sp&&buf[0]!='['){
					vp=strchr(buf,'=');
					if(vp){
						*vp='\0';
						if(!strcmp(delete_space(buf),name)){
							vp++;
							if(strcmp(delete_space(vp),value)){
								delete_line(sp);
								sprintf(buf,"%s=%s\r\n",name,value);
								insert_line(sp,buf);
								BufChanged=1;
							}
							found=1;

							break;
						}
					}
					else{
						delete_line(sp);
						BufChanged=1;
					}
				}
				else break;
				sp=np;
			}//end while(1);
			if(!found){
				sprintf(buf,"%s=%s\r\n",name,value);
				insert_line(sp,buf);
				BufChanged=1;
				found=1;
			}
			break;
		}
	}

	if(!found)
	{
		sprintf(sp,"\r\n[%s]\r\n%s=%s\r\n",section,name,value);			
		BufChanged=1;
	}
	write_inifile(inifile);
	return true;
}

/*************************************************
  Function:    		wriDeleteINIte_ini
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 DeleteINI(char *inifile,char *segment,char *name)
{
	
	char	*sp,*np,*vp,buf[MAX_LINE_LEN],seg[MAX_LINE_LEN];

	if(flush_buffer(inifile))	return 1;
	if(!validstring(name)||!validstring(segment)) return 2;
	sprintf(seg,"[%s]",segment);
	sp=WriteBuf;
	while(*sp)
	{
		sp=get_line(sp,buf,MAX_LINE_LEN);
		if(!strcmp(seg,buf))
		{
			while(1)
			{
				np=get_line(sp,buf,MAX_LINE_LEN);
				if(*sp&&buf[0]!='['){
					vp=strchr(buf,'=');
					if(vp){
						*vp='\0';
						if(!strcmp(delete_space(buf),name)){
							delete_line(sp);
							BufChanged=1;
							break;
						}
					}
					else{
						delete_line(sp);
						BufChanged=1;
					}
				}
				else break;
				sp=np;
			}
			break;
		}
	}
	write_inifile(inifile);
	return 0;
}

/*************************************************
  Function:    		read_ini
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int read_ini(char *inifile,char *segment,char *name,char *value,char *defaultvalue)
{
	FILE *inif;
	int32  found,i;
	char buf[MAX_LINE_LEN],bufname[MAX_LINE_LEN],seg[MAX_LINE_LEN],*s,*sp,*np,*vp;

	found=0;
	sprintf(seg,"[%s]",segment);

	if(!strcmp(inifile,PrevFile))
	{
		sp=WriteBuf;
		while(*sp)
		{
			sp=get_line(sp,buf,MAX_LINE_LEN);
			if(!strcmp(seg,buf))
			{
				while(1)
				{
					np=get_line(sp,buf,MAX_LINE_LEN);
					if(*sp&&buf[0]!='[')
					{
						vp=strchr(buf,'=');
						if(vp)
						{
							*vp='\0';
							if(!strcmp(delete_space(buf),name))
							{
								vp++;
								vp=delete_space(vp);
								strcpy(value,vp);
								found=1;
								break;
							}
						}
					}
					else break;
					sp=np;
				}
				break;
			}
		}
		if (!found||!validstring(defaultvalue)) 
		{
			 strcpy(value,defaultvalue);
		}
		return -1;
	}

	if ((inif=fopen(inifile,"r"))==NULL) 
	{
		return -1;
	}

	while(read_line(inif,buf)!=EOF)
	{
		if(!strcmp(seg,buf))
		{
			while(read_line(inif,buf)!=EOF&&buf[0]!='[')
			{
				for(i=0;buf[i]!='='&&buf[i];i++) bufname[i]=buf[i];
				bufname[i]='\0';
				for(i--;bufname[i]==' ';i--) bufname[i]='\0';
				if(!strcmp(bufname,name))
				{
					s=buf;
					while(*s!='='&&*s) s++;
					s++;
					while(*s==' ')     s++;
					strcpy(value,s);
					found=1;
					break;
				}
			}
			break;
		}
	}

	fclose(inif);
	if (!found||!validstring(defaultvalue)) 
	{ 
		strcpy(value,defaultvalue);
	}
	
	return 0;
}

/*************************************************
  Function:    		ReadINIDomain
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 ReadINIDomain(char *inifile,char *segment,char *value)
{
	FILE *inif;
	int32 i,count=0;
	char buf[MAX_LINE_LEN],seg[MAX_LINE_LEN],*sp,*np,*vp;

	*value='\0';
	sprintf(seg,"[%s]",segment);

	if(!strcmp(inifile,PrevFile))// if file in buffer
	{
		sp=WriteBuf;
		while(*sp)
		{
			sp=get_line(sp,buf,MAX_LINE_LEN);
			if(!strcmp(seg,buf))
			{
				while(1)
				{
					np=get_line(sp,buf,MAX_LINE_LEN);
					if(*sp&&buf[0]!='[')
					{
						vp=strchr(buf,'=');
						if(vp)
						{
							*vp='\0';
							vp=delete_space(buf);
							strcpy(value,vp);
							value+=strlen(vp)+1;
							count++;
						}
					}
					else break;
					sp=np;
				}
				break;
			}
		}
		return	count;
	}

	if ((inif=fopen(inifile,"r"))==NULL)  return 0;

	while(read_line(inif,buf)!=EOF)
	{
		if(!strcmp(seg,buf)){
			while(read_line(inif,buf)!=EOF&&buf[0]!='[')
			{				
				for(i=0;buf[i]!='='&&buf[i];i++);
					buf[i]='\0';
				for(i--;buf[i]==' ';i--) buf[i]='\0';
					if(buf[0]!='\0'){
						strcpy(value,buf);
						value+=strlen(buf)+1;
						count++;
					}					
			}
			break;
		}
	}
	fclose(inif);
	return count;
}

/*************************************************
  Function:    		DeleteINIDomain
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 DeleteINIDomain(char *inifile,char *segment)
{
	char *sp,*np,buf[MAX_LINE_LEN],seg[MAX_LINE_LEN];
	
	if(flush_buffer(inifile))	return 1;
	if(!validstring(segment))	return 2;
		
	sprintf(seg,"[%s]",segment);
	sp=WriteBuf;
	while(*sp)
	{
		np=get_line(sp,buf,MAX_LINE_LEN);
		if(!strcmp(seg,buf)){
			do{
				delete_line(sp);
			}while(*sp&&*sp!='[');
			BufChanged=1;
			break;
		}
		sp=np;
	}
	write_inifile(inifile);
	return 0;
}

/*************************************************
  Function:    		CopyINI
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
void CopyINI(char *sfn,char *sdm,char *svn,char *tfn,char *tdm)
{
	char buf[MAX_LINE_LEN];
	char value[MAX_LINE_LEN];
	char onoff[MAX_LINE_LEN];
	int32  i,j,l;

	read_ini(sfn,sdm,svn,buf,"");
	l=strlen(buf);
	i=0;
	while(i<l)
	{
		j=0;
		if(buf[i]=='<')
		{
			int32 k;
			char temp[MAX_LINE_LEN];
			k=i;
			while(buf[i]!='>') value[j++]=buf[i++];
			value[j]='\0';
			read_ini(tfn,tdm,value+1,onoff,"");
			if (onoff[0]){
				value[j++]='_';
				value[j]='\0';
				strcat(value,onoff);
				strcat(value,">");
				read_ini(sfn,sdm,value,onoff,value);
				buf[k]='\0';
				strcpy(temp,buf);
				strcat(temp,onoff);
				strcat(temp,buf+i+1);
				strcpy(buf,temp);
				i=0;
			}
		}
		i++;
	}
	write_ini(tfn,tdm,svn,buf);
}

/*************************************************
  Function:    		get_inicount
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 get_inicount(char *path)
{
	FILE *fp = NULL;
	int32 num = 0;
	char buf[MAX_LINE_LEN];
	
	fp = fopen(path,"r");		 
	if(fp == NULL)
		return -1;
	while(read_line(fp,buf)!=EOF)
	{
		if(buf[0] == '[')
			num++;
	}	
	
	fclose(fp);
	return num;
}

/*************************************************
  Function:    		BakConfigFile
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 BakConfigFile(char* oldname,char* newname)
{	
	//check name
	if(!oldname||!newname)
		return 1;		
	else if(strlen(oldname)==0||strlen(newname)==0)
		return 1;		
	if(!validstring(oldname)||!validstring(newname))
		return 1;
	if(BufChanged){		
		return rename(oldname,newname);			
	}
	else return 0;	
}  


