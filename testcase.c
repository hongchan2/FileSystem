#include "hw1.h"
#include "validate.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


void testcase1(void)
{
	int check_getData = 0;
	char* pBuf = (char*)malloc(4);
	InsertData("111",3,"aaaa",4);	
	check_getData += getDataByKey("111",3,pBuf,4);
	InsertData("222",3,"bbbb",4);	
	check_getData += getDataByKey("222",3,pBuf,4);
	InsertData("333",3,"cccc",4);	
	InsertData("444",3,"dddd",4);	
	check_getData += getDataByKey("333",3,pBuf,4);
	InsertData("555",3,"eeee",4);	
	check_getData += getDataByKey("444",3,pBuf,4);
	InsertData("666",3,"ffff",4);	
	InsertData("777",3,"hhhh",4);	
	check_getData += getDataByKey("555",3,pBuf,4);
	InsertData("888",3,"iiii",4);	
	check_getData += getDataByKey("999",3,pBuf,4);
	InsertData("999",3,"jjjj",4);	
	InsertData("000",3,"kkkk",4);	

	check_TestCase1(check_getData);
}	

void testcase2(void) 
{
	int check_getData = 0;
	char* pBuf=(char*)malloc(4);
	RemoveDataByKey("222",3);
	check_getData += getDataByKey("666",3,pBuf,4);
	
	RemoveDataByKey("555",3);
	RemoveDataByKey("888",3);
	check_getData += getDataByKey("555",3,pBuf,4);
	
	RemoveDataByKey("444",3);
	RemoveDataByKey("999",3);	
	check_getData += getDataByKey("333",3,pBuf,4);
	check_getData += getDataByKey("222",3,pBuf,4);
	RemoveDataByKey("333",3);

	check_TestCase2(check_getData);

}

void testcase3(void)
{
	char* pBuf = (char*)malloc(8);
	
	InsertData("222",3,"AAAAA",5);
	InsertData("33",2,"BBBB",4);
	InsertData("4444",4,"CCCCCCC",7);
	InsertData("NEW",3,"IIIIJJJJ",8);
	getDataByKey("NEW",3,pBuf,8);

	check_TestCase3(pBuf);

}

int main(int argc, char* argv[])
{
	int tcNum;
	
	if(argc!= 2)
	{
		printf("Input TestCase Number!\n");
		return 0;
	}
	
	tcNum = atoi(argv[1]);
	
	switch(tcNum)
	{
		case 1:
			testcase1();
			break;
		case 2:
			testcase2();
			break;
		case 3:
			testcase3();
			break;
	}
	
	return 0;
}

