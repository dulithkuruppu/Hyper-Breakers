struct dynList;
typedef void* dynElem;
typedef void (*freeMemfunct) (void*);
typedef void (*printElemFunct) (void*);

typedef struct dynList* dynList;
extern dynList createDynList(freeMemfunct fFunct, printElemFunct pFunct);
extern dynElem getElem(dynList dList, int index);
extern dynElem *getData(dynList dList);
extern dynElem removeAtIndexDynList(dynList dList, int index);
extern int getIndexOf(dynList dList, dynElem element);
extern dynElem removeDynList(dynList dList, dynElem element);
extern int getLengthDynList(dynList dList);
extern void increaseCap(dynList dList);
extern void insertDynList(dynList dList,dynElem new);
extern void freeDynList(dynList dList);
extern void printDynList(dynList dList);
extern int comp_uint32(void* a, void* b);