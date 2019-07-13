#ifndef __HW1_H__

#define __HW1_H__

extern void InsertData(char* key, int keySize, char* pBuf, int bufSize);
extern int getDataByKey(char* key, int keySize, char* pBuf, int bufSize);
extern void RemoveDataByKey(char* key, int keySize);

#endif