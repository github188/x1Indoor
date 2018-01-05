/*******************************************************************
File :  List.c
Desc :  动态List处理

*********************************************************************/
#include "gui_include.h"

#define GUI_ARRINE

/*************************************************
  Function:		MallocClass
  Description: 
  Input:		
  	1.size
  Output:		无
  Return:		无
  Others:
*************************************************/
void* MallocClass(unsigned int size)
{
    void* Memget = NULL;

    Memget = malloc(size);

    if (Memget)
    {
        memset(Memget, 0, size);
	}

    return Memget;
}

/*************************************************
  Function:		FreeClass
  Description: 
  Input:		
  	1.size
  Output:		无
  Return:		无
  Others:
*************************************************/
void FreeClass(void * list)
{
	if(list) 
	{
		free(list);
	}
}

/*************************************************
  Function:		StriCmp
  Description: 
  Input:		
  	1.size
  Output:		无
  Return:		无
  Others:
*************************************************/
int StriCmp(const char * s1, const char * s2)
{
    int        d ;
    while (*s1 && *s2)
    {
        d = (*s1) - (*s2);
        if (d != 0)
        {
            if (d != 'a' -'A' && d != 'A' -'a')
            {
                return d;
            }
            else if (*s1 < 'A' || *s1 > 'z' || (*s1 > 'Z' && *s1 < 'a'))
            {
                return d;
            }
        }
        s1++;
        s2++;
    }
    d = (*s1) - (*s2);
    return d;
}

///////////////////////////////////////////////////////////////////////
/*-------------------------------------------------------------------*/
/*
Name: ListCreate
Desc: Allocates space for the management of a list, and returns the pointer
to this UILIST.
*/
UILIST *ListCreate(unsigned short Attr, UILISTFTABLE * funcTable)
{
    UILIST *z = MallocClass(sizeof(UILIST));

    if (z)
    {
        z->attr = Attr;
        z->f = funcTable;
        z->first = z->last = NULL;
    }

	log_printf("ListCreate............\n");
    return z;
}

/*-------------------------------------------------------------------*/
/*
Name: ListDestroy
Desc: Destroys the contents of the list, and then the list itself.
*/
void  ListDestroy(UILIST * list)

{
    if (NULL != list)
    {
        ListClear(list);

        FreeClass(list);
    }
}

/*-------------------------------------------------------------------*/
/*
Name: ListAddItem
Desc: Adds item to the list box, returning its position in the list (0 is the
first position). Returns a negative value if an error occurs.
Check for duplicates only occurs in AddItem, not in InsertItem.
If a duplicate is found, and NODUPLICATES is selected, then the index
of the duplicate is returned.
*/
int ListAddItem(UILIST * list, void *item)

{
    unsigned short count = 0;
    UILISTENTRY *curr = NULL;

    if (list->attr & LS_NODUPLICATES)
    {
        int pos = ListFindItem(list, item);

        if (pos >= 0)
            return pos;
    }
    curr = list->first;
    while (curr)
    {
        if (list->attr & LS_SORTED)
        {
            if (list->f && list->f->CompareItem)
            {
                if ((*(list->f->CompareItem))(item, curr->data) < 0)
                    break;
            }
            else
            {     /* By default just use stricmp */
                if (StriCmp((char *) item, (char *) curr->data) < 0)
                    break;
            }
        }
        curr = curr->next;
        count++;
    }
    return ListInsertItem(list, item, count);
}

/*-------------------------------------------------------------------*/
/*
Name: ListFindItem
Desc: Find an item in the list, by using the compare function.
It returns the index of the item in the list or -1 if not found
*/
int ListFindItem(const UILIST * list, void *item)

{
    unsigned short count = 0;
    UILISTENTRY *curr;

    curr = list->first;
    while (curr)
    {
        if (list->f && list->f->CompareItem)
        {
            if ((*(list->f->CompareItem))(item, curr->data) == 0)
                break;
        }
        else
        {      /* By default just use stricmp */
            if (listStrcmp((char *) item, (char *) curr->data) == 0)
                break;
        }
        curr = curr->next;
        count++;
    }
    return curr == NULL ? -1 : count;
}

/*-------------------------------------------------------------------*/
/*
Name: ListCountOccurences
Desc: Counts the instances of an item in the list, by using the compare
  function.
  It returns the occurrences of the item in the list.
*/
int ListCountOccurrences(const UILIST * list, void *item)

{
    unsigned short count = 0;
    UILISTENTRY *curr;

    curr = list->first;
    while (curr)
    {
        if (list->f && list->f->CompareItem)
        {
            if ((*(list->f->CompareItem))(item, curr->data) == 0)
                count++;
        }
        else
        {      /* By default just use stricmp */
            if (listStrcmp(item, curr->data) == 0)
                count++;
        }
        curr = curr->next;
    }
    return count;
}

/*-------------------------------------------------------------------*/
/*
Name: ListFindItemBackwardsTo
Desc: Find an item in the list, by using the compare function, starting
  from the end up to limitItem.
  It returns the index of the item in the list.
*/
int ListFindItemBackwardsTo(const UILIST * list, void *searchItem, void *limitItem)

{
    unsigned short count = 0;
    UILISTENTRY *curr;
    int limitReached = FALSE;
    int found = FALSE;

    curr = list->last;
    while (curr && !limitReached && !found)
    {
        if (list->f && list->f->CompareItem)
        {
            if ((*(list->f->CompareItem))(limitItem, curr->data) == 0)
                limitReached = TRUE;
            else if ((*(list->f->CompareItem))(searchItem, curr->data) == 0)
                found = TRUE;
        }
        else
        {      /* By default just use stricmp */
            if (listStrcmp(limitItem, curr->data) == 0)
                limitReached = TRUE;
            else if (listStrcmp(searchItem, curr->data) == 0)
                found = TRUE;
        }
        curr = curr->prev;
        count++;
    }

    return found ? (ListGetCount(list) - count) : -1;
}

/*-------------------------------------------------------------------*/
/*
Name: ListFindItemBackwards
Desc: Find an item in the list, by using the compare function, starting
  from the end.
  It returns the index of the item in the list.
*/
int ListFindItemBackwards(const UILIST * list, void *item)

{
    unsigned short count = 0;
    UILISTENTRY *curr;

    curr = list->last;
    while (curr)
    {
        if (list->f && list->f->CompareItem)
        {
            if ((*(list->f->CompareItem))(item, curr->data) == 0)
                break;
        }
        else
        {      /* By default just use stricmp */
            if (listStrcmp(item, curr->data) == 0)
                break;
        }
        curr = curr->prev;
        count++;
    }
    /*  return curr == NULL ? -1 : count;*/
    return (curr == NULL) ? -1 : ((ListGetCount(list) - count) - 1);
}

/*-------------------------------------------------------------------*/
/*
Name: ListGetItem
Desc: Returns the item at the index position in the list.
(0 is the first position). Returns a negative value if an error occurs.
*/
void *ListGetItem(const UILIST * list, unsigned short index)

{
    UILISTENTRY *curr = ListGetListEntry(list, index);

    return curr == NULL ? NULL : curr->data;
}

/*-------------------------------------------------------------------*/
/*
Name: ListGetListEntry
Desc: Returns the actually UILISTENTRY at the index position in the list.
(0 is the first position). Returns NULL if an error occurs.
*/
UILISTENTRY *ListGetListEntry(const UILIST * list, unsigned short index)

{
    unsigned short count = 0;
    UILISTENTRY *curr;

    curr = list->first;
    while (count < index && curr)
    {
        curr = curr->next;
        count++;
    }
    return curr == NULL ? NULL : curr;
}

/*-------------------------------------------------------------------*/
/*
Name: ListInsertItem
Desc: Inserts item in the list box at the position supplied in index, and
returns the item's actual position (starting at 0) in the list. A negative
value is returned if an error occurs. The list is not resorted. If Index is
-1, the string is appended to the end of the list.
*/
int ListInsertItem(UILIST * list, void *item, unsigned short index)

{
    unsigned short count = 0;
    UILISTENTRY *curr;
    UILISTENTRY *thisentry = MallocClass(sizeof(UILISTENTRY));

    thisentry->data = item;
    thisentry->selected = FALSE;

    curr = list->first;
    while (count < index && curr)
    {
        curr = curr->next;
        count++;
    }
    if (curr == NULL)
    {
        thisentry->next = NULL;
        /* adding to the end of the list */
        if (list->last != NULL)
        {
            /* already entries in this list */
            thisentry->prev = list->last;
            list->last->next = thisentry;
        }
        else
        {
            /* adding to an empty list */
            thisentry->prev = NULL;
            list->first = thisentry;
        }
        list->last = thisentry;
    }
    else
    {
        /* inserting into the list */
        thisentry->prev = curr->prev;
        thisentry->next = curr;
        if (curr == list->first)
            /* adding at the beginning of the list */
            list->first = thisentry;
        else
            /* inserting between existing entries */
            curr->prev->next = thisentry;
        curr->prev = thisentry;
    }
    return count < index ? -1 : count;
}

/*-------------------------------------------------------------------*/
/*
Name: ListDeleteItem
Desc: Deletes the item in the list at the position (starting at 0) supplied in
Index.
Returns the number of remaining list items, or a negative value if an error
occurs.
*/
int ListDeleteItem(UILIST * list, unsigned short index)

{
    unsigned short count = 0;
    UILISTENTRY *curr;

    curr = list->first;
    while (count < index && curr)
    {
        curr = curr->next;
        count++;
    }

    if (curr != NULL)
    {
        if (curr->next != NULL)
            curr->next->prev = curr->prev;
        else
            list->last = curr->prev;
        if (curr->prev != NULL)
            curr->prev->next = curr->next;
        else
            list->first = curr->next;
        if (curr->data && (list->attr & LS_OWNSITEMS))
        {
            if (list->f && list->f->FreeItem)
                (*list->f->FreeItem)(curr->data);
            else
                FreeClass(curr->data);
            curr->data = NULL;
        }
        if (curr->data)
            FreeClass(curr->data);    //LINGZJ MODIFY
        FreeClass(curr);
        return ListGetCount(list);
    }
    else
        return -1;
}

/*-------------------------------------------------------------------*/
/*
Name: ListRemoveItem
Desc: Removes the item in the list at the position (starting at 0) supplied in
Index. It specifically does not free the object, regardless of the attributes
of the List.
Returns the item entry, after removing it from the list.
*/
void *ListRemoveItem(UILIST * list,  unsigned short index)
{
     unsigned short count = 0;
    UILISTENTRY *curr;
    void *RetVal = NULL;

    curr = list->first;
    while (count < index && curr)
    {
        curr = curr->next;
        count++;
    }

    if (curr != NULL)
    {
        RetVal = curr->data;
        if (curr->next != NULL)
            curr->next->prev = curr->prev;
        else
            list->last = curr->prev;
        if (curr->prev != NULL)
            curr->prev->next = curr->next;
        else
            list->first = curr->next;
    }
    return RetVal;
}

/*-------------------------------------------------------------------*/
/*
Name: ListGetCount
Desc: Returns the number of items in the list.
*/
 unsigned short ListGetCount(const UILIST * list)

{
    UILISTENTRY *curr;
     unsigned short count = 0;

    if (NULL == list)
        curr = NULL;
    else
        curr = list->first;
    while (curr)
    {
        curr = curr->next;
        count++;
    }
    return count;
}

/*-------------------------------------------------------------------*/
/*
Name: ListClear
Desc: Deletes al of the items in the list.
*/
void  ListClear(UILIST * list)

{
    while (ListGetCount(list))
        ListDeleteItem(list, 0);
}

/*-------------------------------------------------------------------*/
/*
Name: ListGetSelCount
Desc: Returns the number of selected items in the list box. For single- or
multiple-selection list boxes.
*/
 unsigned short ListGetSelCount(const UILIST * list)

{
    UILISTENTRY *curr;
     unsigned short count = 0;

    curr = list->first;
    while (curr)
    {
        if (curr->selected)
            count++;
        curr = curr->next;
    }
    return count;
}

/*-------------------------------------------------------------------*/
/*
Name: ListGetSelIndex
Desc: Returns the positive index (starting at 0) of the currently selected item,
or a negative value if no item is selected. For single selection list boxes.
*/
int ListGetSelIndex(const UILIST * list)

{
    UILISTENTRY *curr;
     unsigned short count = 0;

    if (list->attr & LS_MULTIPLESEL)
        return -1;

    curr = list->first;
    while (curr)
    {
        if (curr->selected)
            return count;
        curr = curr->next;
        count++;
    }
    return -1;
}

/*-------------------------------------------------------------------*/
/*
Name: ListGetSelIndexes
Desc: Fills the indexes array with the indexes of up to maxCount selected strings.
Returns the number of items put in indexes (-1 for single-selection list
boxes).
*/
int ListGetSelIndexes(const UILIST * list,  unsigned short * indexes,  unsigned short maxCount)

{
    UILISTENTRY *curr;
     unsigned short count = 0;
     unsigned short selcount = 0;

    if (!(list->attr & LS_MULTIPLESEL))
        return -1;

    curr = list->first;
    while (curr)
    {
        if (curr->selected)
        {
            indexes[selcount++] = count;
            if (selcount >= maxCount)
                return selcount; /* Just have to work on those selections we can report */
        }
        curr = curr->next;
        count++;
    }
    return selcount;
}

/*-------------------------------------------------------------------*/
/*
Name: ListSetSelIndex
Desc: Forces the selection of the item at the position (starting at 0) supplied in
Index. If Index is -1, the list box is cleared of any selection.

Returns a negative number if an error occurs. For single-selection list
boxes.
*/
int ListSetSelIndex(const UILIST * list, int index)

{
     unsigned short count = 0;
    UILISTENTRY *curr;
    int currentsel;

    if (list->attr & LS_MULTIPLESEL)
        return -1;

    /* Clear any existing selection */
    currentsel = ListGetSelIndex(list);
    if (currentsel >= 0)
    {
        UILISTENTRY *listEntry = ListGetListEntry(list, ( unsigned short) currentsel);

        if (listEntry)
            listEntry->selected = FALSE;
    }
    curr = list->first;
    if (index == -1)
    {
        while (curr)
        {
            curr->selected = FALSE;
            curr = curr->next;
        }
        return 0;
    }
    else
    {
        while (count < index && curr)
        {
            curr = curr->next;
            count++;
        }
        if (curr == NULL)
            return -1;
        curr->selected = TRUE;
        return count;
    }
}

/*-------------------------------------------------------------------*/
/*
Name: ListSetSelIndexes
Desc: For multiple-selection list boxes. Selects or deselects the strings in the
associated list box at the indexes specified in the Indexes array. If
ShouldSet is TRUE, the indexed strings are selcted and highlighted, if
ShouldSet is FALSE the highlight is removed and they are no longer selected.

Returns the number of strings successfully selected or deselected (-1 for
single selection list boxes).

If numSel is less than zero, all strings are selected or deselected,
and a negative value is returned on failure.
*/
int ListSetSelIndexes(const UILIST * list, const unsigned short * indexes, int32 numSel, char shouldSet)

{
    UILISTENTRY *curr;
    short int selcount = 0;
     unsigned short i;

    if (!(list->attr & LS_MULTIPLESEL))
        return -1;
    curr = list->first;
    if (numSel == -1)
    {
        while (curr)
        {
            curr->selected = shouldSet;
            curr = curr->next;
        }
        return 0;
    }
    else
    {
        for (i = 0; i < numSel; i++)
        {
            curr = ListGetListEntry(list, indexes[i]);
            if (curr)
            {
                curr->selected = shouldSet;
                selcount++;
            }
        }
        return selcount;
    }
}

/*-------------------------------------------------------------------*/
 unsigned short
ListGetAttributes(const UILIST * inList)
{
    return (inList != NULL) ? inList->attr : 0;
}

void
ListSetAttributes(UILIST * inList,  unsigned short inAttributes)
{
    if (inList != NULL)
        inList->attr = inAttributes;
}

/*-------------------------------------------------------------------*/
/*
Name: ListFindItemPtr
Desc: Find an item in the list, by using the compare function.
It returns the pointer to the item in the list.
It can be used for an search into a list using an incomplete reference
*/
void *ListFindItemPtr(const UILIST * list, void *item)

{
    UILISTENTRY *curr;

    curr = list->first;
    while (curr)
    {
        if (list->f && list->f->CompareItem)
        {
            if ((*(list->f->CompareItem))(item, curr->data) == 0)
                return curr->data;
        }
        else
        {      /* By default just use stricmp */
            if (listStrcmp((char *) item, (char *) curr->data) == 0)
                return curr->data;
        }
        curr = curr->next;
    }
    return NULL;
}


int listStrcmp(const char * cs, const char * ct)
{

    if (cs == ct)
        return 0;

    return -1;;

    /*
     register signed char __res; while (1)
       {

     if ((__res = *cs - *ct++) != 0 || !*cs++)

     break;
       }

       return __res;
       */
}

