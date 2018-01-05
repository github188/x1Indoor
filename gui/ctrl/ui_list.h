/*******************************************************************
File :  List.c
Desc :  动态链表接口函数
*********************************************************************/
#ifndef UI_UIF_TOOLS_UIFLIST_H
#define UI_UIF_TOOLS_UIFLIST_H

typedef struct list_entry
{
    struct list_entry *next;
    struct list_entry *prev;
    void *data;
    int selected;
}UILISTENTRY;

/* List styles */
#define LS_NONE    0x0000
#define LS_OWNSITEMS  0x0001
#define LS_SORTED   0x0002
#define LS_MULTIPLESEL  0x0004
#define LS_NODUPLICATES  0x0008

typedef struct
{
    unsigned int (*CompareItem)(const void *, const void *);
    void (*FreeItem)(void *);
}  UILISTFTABLE;

typedef struct
{
    UILISTENTRY *first;
    UILISTENTRY *last;
    unsigned short attr;
    UILISTFTABLE *f;
}UILIST;

UILIST * ListCreate(unsigned short Attr, UILISTFTABLE * funcTable);
void ListDestroy(UILIST * list);
void ListClear(UILIST * list);
int ListAddItem(UILIST * list, void *item);
int ListFindItem(const UILIST * list, void *item);
int ListFindItemBackwards(const UILIST * list, void *item);
int ListFindItemBackwardsTo(const UILIST * list, void *searchItem, void *limitItem);
int ListCountOccurrences(const UILIST * list, void *item);
int ListInsertItem(UILIST * list, void *item, unsigned short index);
int ListDeleteItem(UILIST * list, unsigned short index);
void * ListRemoveItem(UILIST * list, unsigned short index);
unsigned short ListGetCount(const UILIST * list);
void * ListGetItem(const UILIST * list, unsigned short index);
UILISTENTRY * ListGetListEntry(const UILIST * list, unsigned short index);
unsigned short ListGetSelCount(const UILIST * list);
int ListGetSelIndex(const UILIST * list);
int ListGetSelIndexes(const UILIST * list, unsigned short * indexes, unsigned short maxCount);
int ListSetSelIndex(const UILIST * list, int index);
int ListSetSelIndexes(const UILIST * list, const unsigned short * indexes, int numSel, char shouldSet);
unsigned short ListGetAttributes(const UILIST * inList);
void ListSetAttributes(UILIST * inList, unsigned short inAttributes);
void *ListFindItemPtr(const UILIST * list, void *item);
void FreeClass(void * list);
int listStrcmp(const char * cs, const char * ct);
/*************************************************
  Function:		MallocClass
  Description: 
  Input:		
  	1.size
  Output:		无
  Return:		无
  Others:
*************************************************/
void* MallocClass(unsigned int size);
#endif

