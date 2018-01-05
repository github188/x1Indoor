#ifndef __AU_INI__H_
#define __AU_INI_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>


/*************************************************
  Function:    		write_ini
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 write_ini(char *inifile,char *section,char *name,char *value);

/*************************************************
  Function:    		wriDeleteINIte_ini
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 DeleteINI(char *inifile,char *segment,char *name);

/*************************************************
  Function:    		read_ini
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int read_ini(char *inifile,char *segment,char *name,char *value,char *defaultvalue);

/*************************************************
  Function:    		ReadINIDomain
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 ReadINIDomain(char *inifile,char *segment,char *value);

/*************************************************
  Function:    		DeleteINIDomain
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 DeleteINIDomain(char *inifile,char *segment);

/*************************************************
  Function:    		CopyINI
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
void CopyINI(char *sfn,char *sdm,char *svn,char *tfn,char *tdm);

/*************************************************
  Function:    		get_inicount
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 get_inicount(char *path);

/*************************************************
  Function:    		BakConfigFile
  Description:		
  Input:	
  Output:
  Return:
  Others:
*************************************************/
int32 BakConfigFile(char* oldname,char* newname);


#endif


