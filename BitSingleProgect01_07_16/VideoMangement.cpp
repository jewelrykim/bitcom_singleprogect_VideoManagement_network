#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "VideoManagement.h"

void LinkNode(LINKEDLIST * pLinkedList, void * pData, NODE * pSearch)
{
	NODE * pNode = (NODE *)malloc(sizeof(NODE));
	pNode->pPre = NULL;
	pNode->pNext = NULL;
	pNode->pData = pData;
	if (pLinkedList->pTop == NULL)
	{
		pLinkedList->pTop = pLinkedList->pBottom = pNode;
	}
	else if (pLinkedList->pTop == pSearch)
	{
		pNode->pNext = pLinkedList->pTop;
		pLinkedList->pTop->pPre = pNode;
		pLinkedList->pTop = pNode;
	}
	else if (NULL == pSearch)
	{
		pNode->pPre = pLinkedList->pBottom;
		pLinkedList->pBottom->pNext = pNode;
		pLinkedList->pBottom = pNode;
	}
	else
	{
		pNode->pPre = pSearch->pPre;
		pNode->pNext = pSearch;
		pSearch->pPre->pNext = pNode;
		pSearch->pPre = pNode;
	}
}
void AddTop(LINKEDLIST * pLinkedList, void * pData)
{
	LinkNode(pLinkedList, pData, pLinkedList->pTop);
}
void AddBottom(LINKEDLIST * pLinkedList, void * pData)
{
	LinkNode(pLinkedList, pData, NULL);
}
bool DeleteAt(LINKEDLIST * pLinkedList, NODE * pSearch)
{
	if (pSearch == NULL )
	{
		return false;
	}
	else if (pLinkedList->pTop==pLinkedList->pBottom)
	{
		pLinkedList->pTop = pLinkedList->pBottom = NULL;
	}
	else if (pLinkedList->pTop == pSearch)
	{
		pLinkedList->pTop = pLinkedList->pTop->pNext;
		pLinkedList->pTop->pPre = NULL;
	}
	else if (pLinkedList->pBottom == pSearch)
	{
		pLinkedList->pBottom = pLinkedList->pBottom->pPre;
		pLinkedList->pBottom->pNext = NULL;
	}
	else
	{
		pSearch->pPre->pNext = pSearch->pNext;
		pSearch->pNext->pPre = pSearch->pPre;
	}
	free(pSearch);
	return true;
}
bool DeleteTop(LINKEDLIST * pLinkedList)
{
	return DeleteAt(pLinkedList, pLinkedList->pTop);
}
bool DeleteBottom(LINKEDLIST * pLinkedList)
{
	return DeleteAt(pLinkedList, pLinkedList->pBottom);
}
bool DeleteAll(LINKEDLIST * pLinkedList)
{
	while (DeleteTop(pLinkedList));
	return NULL;
}
void InitLinkedList(LINKEDLIST * pLinkedList, ADDNODE AddTop, ADDNODE AddBottom, DELETEAT DeleteAt, DELETENODE DeleteTop, DELETENODE DeleteBottom, DELETENODE DeleteAll)
{
	pLinkedList->pBottom = NULL;
	pLinkedList->pTop = NULL;
	pLinkedList->AddTop = AddTop;
	pLinkedList->AddBottom = AddBottom;
	pLinkedList->DeleteAt = DeleteAt;
	pLinkedList->DeleteTop = DeleteTop;
	pLinkedList->DeleteBottom = DeleteBottom;
	pLinkedList->DeleteAll = DeleteAll;
}
NODE * FindFlag(LINKEDLIST* list, int iInputdata){
	NODE * pStart = list->pTop;
	while (pStart)
	{
		if ((int)pStart->pData > iInputdata)
		{
			return pStart;
		}
		pStart = pStart->pNext;
	}
	return NULL;
}
void InsertChar(const char * pTitle,char ** MemberData){
	char temp[1024] = {' '};
	printf("%s", pTitle);
	gets_s(temp,sizeof(temp));
	fflush(stdin);
	*MemberData = (char *)malloc(strlen(temp) + 1);
	strcpy_s(*MemberData, strlen(temp) + 1, temp);
}
void InsertInt(const char * pTitle,int * MemberData){
	printf("%s", pTitle);
	scanf_s("%d", MemberData);
	fflush(stdin);
}
void InputMemberData(NODE** pNode){
	MEMBER * Member = (MEMBER *)malloc(sizeof(MEMBER));
	(*pNode)->pData = Member;
	InsertChar("ȸ�� �̸� : ", &((MEMBER*)((*pNode)->pData))->pName);
	InsertInt("ȸ�� ���� : ",&((MEMBER*)((*pNode)->pData))->pAge);
	InsertChar("ȸ�� ��ȭ��ȣ : ", &((MEMBER*)((*pNode)->pData))->pPhone);
}
void InputVideoData(NODE** pNode){
	int switch_on = 0;
	VIDEO* Video = (VIDEO*)malloc(sizeof(VIDEO));
	(*pNode)->pData = Video;
	InsertInt("���� ��ȣ : ", &((VIDEO*)((*pNode)->pData))->iVNumber);
	InsertChar("���� �̸� : ", &((VIDEO*)((*pNode)->pData))->pVName);
	InsertInt("���� ��� : ", &((VIDEO*)((*pNode)->pData))->iGrade);
	printf("1.�뿩 ����, 2.�뿩 �Ұ� \n");
	scanf_s("%d", &switch_on);
	fflush(stdin);
	switch (switch_on)
	{
	case 1:
		printf("�뿩 ����\n");
		((VIDEO*)((*pNode)->pData))->bRent = true;
		break;
	default:
		printf("�뿩�� \n");
		((VIDEO*)((*pNode)->pData))->bRent = false;
		break;
	}
}
void InputMember(LINKEDLIST* list){
	NODE * pNode = (NODE *)malloc(sizeof(NODE));
	InputMemberData(&pNode);
	list->AddBottom(list, (void *)pNode->pData);
}
void InputVideo(LINKEDLIST* list){
	NODE * pNode = (NODE *)malloc(sizeof(NODE));
	InputVideoData(&pNode);
	list->AddBottom(list, (void *)pNode->pData);
}
void PrintMember(NODE * pStart){
		printf("MemberName : %s\n", ((MEMBER*)(pStart->pData))->pName);
		printf("MemberAge : %d\n", ((MEMBER*)(pStart->pData))->pAge);
		printf("MemberPhoneNumber : %s\n", ((MEMBER*)(pStart->pData))->pPhone);
}
void PrintVideo(NODE * pStart){
	printf("VideoNumber : %d\n", ((VIDEO*)(pStart->pData))->iVNumber);
	printf("VideoName : %s\n", ((VIDEO*)(pStart->pData))->pVName);
	printf("Videograde : %d\n", ((VIDEO*)(pStart->pData))->iGrade);
	if (((VIDEO*)(pStart->pData))->bRent == true){
		printf("�뿩 ����\n");
	}
	else{
		printf("�뿩�� �뿩�Ұ���\n");
	}
}
void vPrintMember(LINKEDLIST* list){
	NODE * pStart = list->pTop;
	while (pStart)
	{
		PrintMember(pStart);
		pStart = pStart->pNext;
	}
}
void vPrintVideo(LINKEDLIST* list){
	NODE * pStart = list->pTop;
	while (pStart)
	{
		PrintVideo(pStart);
		pStart = pStart->pNext;
	}
}
NODE* Search(LINKEDLIST * pList, COMPAREFUNC compFunc, void* pData)
{
	NODE *pStart = pList->pTop;
	while (pStart)
	{
		if (compFunc(pStart, pData) == true)
		{
			return pStart;
		}
		pStart = pStart->pNext;
	}
	return NULL;
}
NODE * SSearchName(LINKEDLIST* list, char* iSearch){
	NODE * pStart = list->pTop;
	while (pStart){

		if (strcmp(((MEMBER*)(pStart->pData))->pName,iSearch) == 0)
		{
			return pStart;
		}
		pStart = pStart->pNext;
	}
	return NULL;
}
NODE * SSearchNumber(LINKEDLIST* list, int iSearch){
	NODE * pStart = list->pTop;
	while (pStart){

		if (((VIDEO*)(pStart->pData))->iVNumber== iSearch)
		{
			return pStart;
		}
		pStart = pStart->pNext;
	}
	return NULL;
}
void SearchMember(LINKEDLIST* list){
	char temp[1024] = { ' ' };
	printf("ã�� �� : ");
	gets_s(temp, sizeof(temp));
	fflush(stdin);
	NODE * pFindFlag = SSearchName(list, temp);
	if (pFindFlag == NULL){
		printf("�׷��� ����.");
		return;
	}
	PrintMember(pFindFlag);
}
void SearchVideo(LINKEDLIST* list){
	int temp=0;
	printf("ã�� �� : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(list, temp);
	if (pFindFlag == NULL){
		printf("�׷��� ����.");
		return;
	}
	PrintVideo(pFindFlag);
}
void ModifyMember(LINKEDLIST* list){
	char temp[1024] = { ' ' };
	printf("ã�� �� : ");
	gets_s(temp, sizeof(temp));
	fflush(stdin);
	NODE * pFindFlag = SSearchName(list, temp);
	if (pFindFlag == NULL){
		printf("�׷��� ����.");
		return;
	}
	InputMemberData(&pFindFlag);
}
void ModifyVideo(LINKEDLIST* list){
	int temp = 0;
	printf("ã�� �� : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(list, temp);
	if (pFindFlag == NULL){
		printf("�׷��� ����.");
		return;
	}
	InputMemberData(&pFindFlag);
}
void DeleteMember(LINKEDLIST* list){
	char temp[1024] = { ' ' };
	printf("ã�� �� : ");
	gets_s(temp, sizeof(temp));
	fflush(stdin);
	NODE * pFindFlag = SSearchName(list, temp);
	if (pFindFlag)
	{
		list->DeleteAt(list, pFindFlag);
	}
	else
	{
		printf("�׷��� ����.");
	}
}
void DeleteVideo(LINKEDLIST* list){
	int temp = 0;
	printf("ã�� �� : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(list, temp);
	if (pFindFlag)
	{
		list->DeleteAt(list, pFindFlag);
	}
	else
	{
		printf("�׷��� ����.");
	}
}
void InputRentalData(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, NODE** Rebtallist){
	RENTAL* pRental = (RENTAL*)malloc(sizeof(RENTAL));
	(*Rebtallist)->pData = pRental;
	char Name[1024] = { ' ' };
	int Number = 0;
	printf("ȸ�� �̸� : ");
	gets_s(Name, sizeof(Name));
	fflush(stdin);
	NODE * pFindMember = SSearchName(Memberlist, Name);
	if (pFindMember == NULL){
		printf("�׷��� ����.");
		return;
	}
	((RENTAL*)((*Rebtallist)->pData))->pRMemberName = ((MEMBER*)pFindMember->pData)->pName;

	printf("���� ��ȣ : ");
	scanf_s("%d", &Number);
	fflush(stdin);
	NODE * pFindVideo = SSearchNumber(Videolist, Number);
	if (pFindVideo == NULL){
		printf("�׷����� ����.\n");
		return;
	}
	if (((VIDEO*)(pFindVideo->pData))->bRent == false)
	{
		printf("�̹� �뿩��.\n");
		return;
	}
	if (((VIDEO*)(pFindVideo->pData))->iGrade > ((MEMBER*)pFindMember->pData)->pAge)
	{
		printf("�� ����\n");
		return;
	}
	((VIDEO*)(pFindVideo->pData))->bRent = false;
	((RENTAL*)((*Rebtallist)->pData))->iRVideoNum = ((VIDEO*)pFindVideo->pData)->iVNumber;
	((RENTAL*)((*Rebtallist)->pData))->pRVideoName = ((VIDEO*)pFindVideo->pData)->pVName;

}
void InputRental(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* Rentallist){
	NODE * pNode = (NODE *)malloc(sizeof(NODE));
	InputRentalData(Memberlist, Videolist, &pNode);
	Rentallist->AddBottom(Rentallist, (void *)pNode->pData);
}
void ReturnVideo(LINKEDLIST* Rentallist){
	int temp = 0;
	printf("ã�� �� : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(Rentallist, temp);
	if (pFindFlag == NULL){
		printf("�׷��� ����.");
		return;
	}
	((VIDEO *)pFindFlag->pData)->bRent = true;
	DeleteAt(Rentallist,pFindFlag);
}
void PrintRentallist(LINKEDLIST* Rentallist){
	NODE * pStart = Rentallist->pTop;
	while (pStart)
	{
		printf("MemberName : %s\n", ((RENTAL*)(pStart->pData))->pRMemberName);
		printf("VideoNumber : %d\n", (((RENTAL*)(pStart)->pData))->iRVideoNum);
		printf("VideoName : %s\n", ((RENTAL*)(pStart->pData))->pRVideoName);
		if (((VIDEO*)(pStart->pData))->bRent == true){
			printf("�뿩 ����\n");
		}
		else{
			printf("�뿩�� �뿩�Ұ���\n");
		}
		pStart = pStart->pNext;
	}
}

/*
void VideoManagemanetMain()
{
	char mode = '1';
	char cManageMember = '1';
	char cManageVideo = '1';
	char cManageRental = '1';
	LINKEDLIST Memberlist;
	Memberlist.InitList = InitLinkedList;
	Memberlist.InitList(&Memberlist, AddTop, AddBottom, DeleteAt, DeleteTop, DeleteBottom, DeleteAll);

	LINKEDLIST Videolist;
	Videolist.InitList = InitLinkedList;
	Videolist.InitList(&Videolist, AddTop, AddBottom, DeleteAt, DeleteTop, DeleteBottom, DeleteAll);

	LINKEDLIST Rentallist;
	Rentallist.InitList = InitLinkedList;
	Rentallist.InitList(&Rentallist, AddTop, AddBottom, DeleteAt, DeleteTop, DeleteBottom, DeleteAll);

	while (mode != '0'){
		printf("\nmode ���� ( 1 = ����� ���� , 2 = ���� ���� , 3 = �뿩 ����, 0 = ����) \n");
		mode = getchar();
		fflush(stdin);

		switch (mode)
		{
		case '1':
			printf("\nmode ���� ( 1 = ȸ�� ��� , 2 = ȸ�� ��� , 3 = ȸ�� �˻�, 4 = ȸ�� �������� ,5 = ȸ�� ���� 0 = ���ư���) \n");
			cManageMember = getchar();
			fflush(stdin);
			switch (cManageMember){
			case '1':
				InputMember(&Memberlist);
				break;
			case '2':
				vPrintMember(&Memberlist);
				break;
			case '3':
				SearchMember(&Memberlist);
				break;
			case '4':
				ModifyMember(&Memberlist);
				break;
			case '5':
				DeleteMember(&Memberlist);
				break;
			default:
				cManageMember = '0';
				break;
			}
			break;
		case '2':
			printf("\nmode ���� ( 1 = ���� ��� , 2 = ���� ��� , 3 = ���� �˻�, 4 = ���� �������� ,5 = ���� ���� 0 = ���ư���) \n");
			cManageVideo = getchar();
			fflush(stdin);
			switch (cManageVideo){
			case '1':
				InputVideo(&Videolist);
				break;
			case '2':
				vPrintVideo(&Videolist);
				break;
			case '3':
				SearchVideo(&Videolist);
				break;
			case '4':
				ModifyVideo(&Videolist);
				break;
			case '5':
				DeleteVideo(&Videolist);
				break;
			default:
				cManageVideo = '0';
				break;
			}
			break;
		case '3':
			printf("\nmode ���� ( 1 = ���� �뿩 , 2 = ���� �ݳ� , 3 = ���� �뿩��� ,0 = ���ư���) \n");
			cManageRental = getchar();
			fflush(stdin);
			switch (cManageRental){
			case '1':
				InputRental(&Memberlist, &Videolist, &Rentallist);
				break;
			case '2':
				ReturnVideo(&Rentallist);
				break;
			case '3':
				PrintRentallist(&Rentallist);
				break;
			}
		}
	}
}
*/
