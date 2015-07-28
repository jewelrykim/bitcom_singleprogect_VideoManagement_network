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
	if (pLinkedList->iCount	 == NULL)
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
	++(pLinkedList->iCount);
}
void LinkFindNode(LINKEDLIST * pLinkedList, void * pData, NODE * pSearch)
{
	NODE * pNode = (NODE *)malloc(sizeof(NODE));
	pNode->pPre = NULL;
	pNode->pNext = NULL;
	pNode->pData = pData;
	if (pLinkedList->iCount == NULL)
	{
		pLinkedList->pTop = pLinkedList->pBottom = pNode;
	}
	else if (((VIDEO*)(pLinkedList->pTop)->pData)->iVNumber > ((VIDEO*)pData)->iVNumber)
	{
		pNode->pNext = pLinkedList->pTop;
		pLinkedList->pTop->pPre = pNode;
		pLinkedList->pTop = pNode;
	}
	else if (((VIDEO*)(pLinkedList->pBottom)->pData)->iVNumber < ((VIDEO*)pData)->iVNumber)
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
	++(pLinkedList->iCount);
}
void AddTop(LINKEDLIST * pLinkedList, void * pData)
{
	LinkNode(pLinkedList, pData, pLinkedList->pTop);
}
void AddBottom(LINKEDLIST * pLinkedList, void * pData)
{
	LinkNode(pLinkedList, pData, NULL);
}
void AddPoint(LINKEDLIST * pLinkedList, void * pData)
{
	NODE* pStart = pLinkedList->pTop;

	while (pStart)
	{
		if (((VIDEO*)pData)->iVNumber < ((VIDEO*)pStart->pData)->iVNumber)
		{
			LinkFindNode(pLinkedList, pData, pStart);
			return;
		}
		pStart = pStart->pNext;
	}
	if (pLinkedList->iCount == 0)
	{
		LinkFindNode(pLinkedList, pData, NULL);
		return;
	}
	else
		LinkFindNode(pLinkedList, pData, pLinkedList->pBottom);
}
bool DeleteAt(LINKEDLIST * pLinkedList, NODE * pSearch)
{
	if (pLinkedList->iCount ==0 ||pSearch == NULL )
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
	--(pLinkedList->iCount);
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
void InitLinkedList(LINKEDLIST * pLinkedList, ADDNODE AddTop, ADDNODE AddBottom, ADDNODE AddPoint, DELETEAT DeleteAt, DELETENODE DeleteTop, DELETENODE DeleteBottom, DELETENODE DeleteAll)
{
	pLinkedList->pBottom = NULL;
	pLinkedList->pTop = NULL;
	pLinkedList->AddTop = AddTop;
	pLinkedList->AddBottom = AddBottom;
	pLinkedList->AddPoint = AddPoint;
	pLinkedList->DeleteAt = DeleteAt;
	pLinkedList->DeleteTop = DeleteTop;
	pLinkedList->DeleteBottom = DeleteBottom;
	pLinkedList->DeleteAll = DeleteAll;
	pLinkedList->iCount = 0;
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
void InputMemberData_Client(MEMBER* pMember, void* temp){

	char* pRecvMemberID = (char*)temp +sizeof(PACKET);	// client�������� ���̵�
	//temp�� char �迭�� �����ּ��̴�.+ packet(index)�� ũ��=>128byte�� �ޱ� ���ؼ� �迭�� �����ͷ� �ٲ��ش�
	char *pRecvMemberPW = (char*)temp + sizeof(PACKET)+128; //client ���� ��й�ȣ
	//id(128Byte) ������ ������pw
	char*pRecvMemberName = (char*)temp + sizeof(PACKET)+256;	// client�������� ȸ���̸�
	int pRecvMemberAge = *((char*)temp + sizeof(PACKET)+384);		// client�������� ȸ������
	char*pRecvMemberPhoneNumber = (char*)temp + sizeof(PACKET)+388; //client ���� ȸ����ȭ��ȣ

	pMember->pMemberID = (char*)malloc(strlen(pRecvMemberID) + 1);
	strcpy_s(pMember->pMemberID, strlen(pRecvMemberID) + 1, pRecvMemberID);

	pMember->pPassWord = (char*)malloc(strlen(pRecvMemberPW) + 1);
	strcpy_s(pMember->pPassWord, strlen(pRecvMemberPW) + 1, pRecvMemberPW);

	pMember->pName = (char*)malloc(strlen(pRecvMemberName) + 1);
	strcpy_s(pMember->pName, strlen(pRecvMemberName) + 1, pRecvMemberName);

	pMember->pAge = pRecvMemberAge;

	pMember->pPhone = (char*)malloc(strlen(pRecvMemberPhoneNumber) + 1);
	strcpy_s(pMember->pPhone, strlen(pRecvMemberPhoneNumber) + 1, pRecvMemberPhoneNumber);
}
void InputMemberData_Server(MEMBER* pMember)
{
	InsertChar("ȸ�� ID : ", &(pMember->pMemberID));
	InsertChar("ȸ�� PassWord : ", &(pMember->pPassWord));
	InsertChar("ȸ�� �̸� : ", &(pMember->pName));
	InsertInt("ȸ�� ���� : ", &(pMember->pAge));
	InsertChar("ȸ�� ��ȭ��ȣ : ", &(pMember->pPhone));

}
void InputVideoData(VIDEO * pVideo){

	InsertInt("���� ��ȣ : ", &(pVideo->iVNumber));
	InsertChar("���� �̸� : ", &(pVideo->pVName));
	InsertInt("���� ��� : ", &(pVideo->iGrade));
	pVideo->bRent = true;
	pVideo->bReservation = false;
}
void InputMember_Client(LINKEDLIST* Memberlist, LINKEDLIST* LoginList,void* temp)
{		//temp�� client�� �Է��� �����͸� �״�� ������ ���̴�. void�� Ǯ�� �Ķ��Ÿ�� ������ �Ķ��Ÿ���� �ʹ� �������� ������ �״�� �޾Ҵ�.
	MEMBER * pMember = (MEMBER *)malloc(sizeof(MEMBER));
	InputMemberData_Client(pMember, temp);
	Memberlist->AddBottom(Memberlist, (void *)pMember);
	LoginList->AddBottom(LoginList, (void*)pMember);
}
void InputMember_Server(LINKEDLIST* Memberlist, LINKEDLIST* LoginList)
{	
	NODE * pStar = Memberlist->pTop;
	MEMBER * pMember = (MEMBER *)malloc(sizeof(MEMBER));
	InputMemberData_Server(pMember);
	while (pStar)
	{
		if(strcmp(pMember->pMemberID, ((MEMBER *)pStar->pData)->pMemberID) == 0)
		{
			printf("���� ID�� �ֽ��ϴ�.");
			return ;
		}
		pStar = pStar->pNext;
	}
	if (pMember->pAge<0 || pMember->pAge>150)
	{
		printf("���� ���� �ùٸ��� �ʽ��ϴ�.\n");
		return;
	}
	Memberlist->AddBottom(Memberlist, (void *)pMember);
	LoginList->AddBottom(LoginList, (void*)pMember);
}
void InputVideo(LINKEDLIST* list){
	NODE * pStar = list->pTop;
	VIDEO * pVideo = (VIDEO *)malloc(sizeof(VIDEO));
	InputVideoData(pVideo);
	while (pStar)
	{
		if (pVideo->iVNumber == ((VIDEO*)(pStar->pData))->iVNumber)
		{
			printf("���� ��ȣ�� ���� ������ �ֽ��ϴ�.\n");
			return;
		}
		pStar = pStar->pNext;
	}
/*
	if (list->iCount == 0)
	{
		list->AddBottom(list, (void *)pVideo);
	}*/
	
		list->AddPoint(list, (void *)pVideo);
	
}
void PrintMember(NODE * pStart){
		printf("ȸ�� ���̵� : %s\n", ((MEMBER*)(pStart->pData))->pMemberID);
		printf("ȸ�� ��й�ȣ : %s\n", ((MEMBER*)(pStart->pData))->pPassWord);
		printf("ȸ�� �̸� : %s\n", ((MEMBER*)(pStart->pData))->pName);
		printf("ȸ�� ���� : %d\n", ((MEMBER*)(pStart->pData))->pAge);
		printf("ȸ�� ��ȭ��ȣ : %s\n", ((MEMBER*)(pStart->pData))->pPhone);
}
void PrintVideo(NODE * pStart){
	printf("���� ��ȣ : %d\n", ((VIDEO*)(pStart->pData))->iVNumber);
	printf("���� �̸� : %s\n", ((VIDEO*)(pStart->pData))->pVName);
	printf("���� ��� : %d\n", ((VIDEO*)(pStart->pData))->iGrade);
	if (((VIDEO*)(pStart->pData))->bRent == true){
		printf("�뿩 ����\n");
	}
	else{
		printf("�뿩��...�뿩�Ұ���\n");
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
NODE * SSearchID(LINKEDLIST* list, char* iSearch){
	NODE * pStart = list->pTop;
	while (pStart){

		if (strcmp(((MEMBER*)(pStart->pData))->pMemberID, iSearch) == 0)
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
NODE * SSearchRentalName(LINKEDLIST* list, char* iSearch)
{
	NODE * pStart = list->pTop;
	while (pStart){

		if (strcmp(((RENTAL*)(pStart->pData))->pRMemberName, iSearch) == 0)
		{
			return pStart;
		}
		pStart = pStart->pNext;
	}
	return NULL;
}

NODE * SSearchVideoName(LINKEDLIST* list,char* iSearch)
{
	NODE * pStart = list->pTop;
	while (pStart){

		if (strcmp(((VIDEO*)(pStart->pData))->pVName, iSearch) == 0)
		{
			return pStart;
		}
		pStart = pStart->pNext;
	}
	return NULL;
}
NODE * MSearchVideoName(LINKEDLIST* list, LINKEDLIST* SameNameRentalList, char* iSearch)
{
	NODE * pStart = list->pTop;
	while (pStart){

		if (strcmp(((VIDEO*)(pStart->pData))->pVName, iSearch) == 0)
		{
			VIDEO * p = (VIDEO *)malloc(sizeof(VIDEO));
			p->iVNumber = ((VIDEO*)(pStart->pData))->iVNumber;
			p->pVName = (char*)malloc(strlen(((VIDEO*)(pStart->pData))->pVName) + 1);
			strcpy_s(p->pVName, strlen(((VIDEO*)(pStart->pData))->pVName) + 1, ((VIDEO*)(pStart->pData))->pVName);

			p->iGrade = ((VIDEO*)(pStart->pData))->iGrade;
			p->bRent = ((VIDEO*)(pStart->pData))->bRent;
			p->bReservation = ((VIDEO*)(pStart->pData))->bReservation;

			SameNameRentalList->AddBottom(SameNameRentalList, p);
		}
		pStart = pStart->pNext;
	}
	if (SameNameRentalList->pTop == NULL)
		return NULL;
	else
		return SameNameRentalList->pTop;
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
NODE * SSearchRentalNumber(LINKEDLIST* list, int iSearch){
	NODE * pStart = list->pTop;
	while (pStart){

		if (((RENTAL*)(pStart->pData))->iRVideoNum == iSearch)
		{
			return pStart;
		}
		pStart = pStart->pNext;
	}
	return NULL;
}

void SearchMember(LINKEDLIST* list){
	char temp[1024] = { ' ' };
	printf("ã�� ȸ���� �̸� : ");
	gets_s(temp, sizeof(temp));
	fflush(stdin);
	NODE * pFindFlag = SSearchName(list, temp);
	if (pFindFlag == NULL){
		printf("��ġ�ϴ� ȸ���� �����ϴ�.\n");
		return;
	}
	PrintMember(pFindFlag);
}

void SearchVideo(LINKEDLIST* list){
	int temp=0;
	printf("ã�� ���� ��ȣ : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(list, temp);
	if (pFindFlag == NULL){
		printf("��ġ�ϴ� ������ �����ϴ�.\n");
		return;
	}
	PrintVideo(pFindFlag);
}
void ModifyMember_Client(LINKEDLIST* Memberlist,char * temp)	//temp�� client�� �Է��� �����͸� �״�� ������ ���̴�. void�� Ǯ�� �Ķ��Ÿ�� ������ �Ķ��Ÿ���� �ʹ� �������� ������ �״�� �޾Ҵ�.
{
	char * ClientID = temp + sizeof(PACKET);
	NODE * pFindFlag = SSearchID(Memberlist, ClientID);		//memberlist���� ã�´�.�α����� �����̱� ������ ���� ���� ����
	
	InputMemberData_Client((MEMBER*)(pFindFlag->pData), temp);
}
void ModifyMember_Server(LINKEDLIST* list)	//temp�� client�� �Է��� �����͸� �״�� ������ ���̴�. void�� Ǯ�� �Ķ��Ÿ�� ������ �Ķ��Ÿ���� �ʹ� �������� ������ �״�� �޾Ҵ�.
{
	char temp[1024] = { ' ' };
	printf("������ ȸ�� �̸� : ");
	gets_s(temp, sizeof(temp));
	fflush(stdin);
	NODE * pFindFlag = SSearchName(list, temp);
	if (pFindFlag == NULL){
		printf("��ġ�ϴ� ȸ���� �����ϴ�.\n");
		return;
	}
	InputMemberData_Server((MEMBER*)pFindFlag->pData);
}
void ModifyVideo(LINKEDLIST* list)	//temp�� client�� �Է��� �����͸� �״�� ������ ���̴�. void�� Ǯ�� �Ķ��Ÿ�� ������ �Ķ��Ÿ���� �ʹ� �������� ������ �״�� �޾Ҵ�.
{
	int temp = 0;
	printf("ã�� ���� ��ȣ : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(list, temp);
	if (pFindFlag == NULL){
		printf("��ġ�ϴ� ������ �����ϴ�.\n");
		return;
	}
	InputVideoData((VIDEO*)pFindFlag->pData);
}
void DeleteMember_Client(LINKEDLIST* Memberlist, char * temp)
{
	NODE * pFindFlag = SSearchID(Memberlist, temp);
	Memberlist->DeleteAt(Memberlist, pFindFlag);

}
void DeleteMember_Server(LINKEDLIST* list){
	char temp[1024] = { ' ' };
	printf("������ ȸ�� �̸� : ");
	gets_s(temp, sizeof(temp));
	fflush(stdin);
	NODE * pFindFlag = SSearchName(list, temp);
	if (pFindFlag)
	{
		list->DeleteAt(list, pFindFlag);
	}
	else
	{
		printf("��ġ�ϴ� ȸ���� �����ϴ�.\n");
	}
}
void DeleteVideo(LINKEDLIST* list){
	int temp = 0;
	printf("������ ���� ��ȣ : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(list, temp);
	if (pFindFlag)
	{
		list->DeleteAt(list, pFindFlag);
	}
	else
	{
		printf("��ġ�ϴ� ������ �����ϴ�.\n");
	}
}
int InputRentalData_Server(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, RENTAL* pRental){

	char MemberID[1024] = { ' ' };
	int Number = 0;
	printf("ȸ�� ���̵� : ");
	gets_s(MemberID, sizeof(MemberID));
	fflush(stdin);
	NODE * pFindMember = SSearchID(Memberlist, MemberID);
	if (pFindMember == NULL){
		printf("��ġ�ϴ� ȸ���� �����ϴ�.\n");
		return 1;
	}

	printf("���� ��ȣ : ");
	scanf_s("%d", &Number);
	fflush(stdin);
	NODE * pFindVideo = SSearchNumber(Videolist, Number);
	if (pFindVideo == NULL){
		printf("��ġ�ϴ� ������ �����ϴ�.\n");
		return 1;
	}
	if (((VIDEO*)(pFindVideo->pData))->bRent == false)
	{
		printf("�̹� �뿩���Դϴ�.\n");
		return 1;
	}
	if (((VIDEO*)(pFindVideo->pData))->iGrade > ((MEMBER*)pFindMember->pData)->pAge)
	{
		printf("%d �̻� ������ �����Դϴ�.\n", ((VIDEO*)(pFindVideo->pData))->iGrade);
		return 1;
	}
	((VIDEO*)(pFindVideo->pData))->bRent = false;
	pRental->pMemberID = ((MEMBER*)pFindMember->pData)->pMemberID;
	pRental->pRMemberName = ((MEMBER*)pFindMember->pData)->pName;
	pRental->iRVideoNum = ((VIDEO*)pFindVideo->pData)->iVNumber;
	pRental->pRVideoName = ((VIDEO*)pFindVideo->pData)->pVName;
	return 0;
}
void InPutRentalData_Client(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* SameNameRentalList, RENTAL* pRental, void* Member, void* Video,int VideoNum)
{
	
	NODE * pFindMember = SSearchID(Memberlist, (char*) Member);		//��ġ�ϴ� ���� ���� �� ����.
	NODE * pStart = SameNameRentalList->pTop;
	NODE * pFindVideo = SSearchNumber(Videolist, VideoNum);

	((VIDEO*)(pFindVideo->pData))->bRent = false;
	pRental->pMemberID = ((MEMBER*)pFindMember->pData)->pMemberID;
	pRental->pRMemberName = ((MEMBER*)pFindMember->pData)->pName;
	pRental->iRVideoNum = ((VIDEO*)pFindVideo->pData)->iVNumber;
	pRental->pRVideoName = ((VIDEO*)pFindVideo->pData)->pVName;


}
void InputRental_Server(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* Rentallist)
{
	RENTAL* pRental = (RENTAL*)malloc(sizeof(RENTAL));
	
	if (InputRentalData_Server(Memberlist, Videolist, pRental) == 0){
		Rentallist->AddBottom(Rentallist, (void *)pRental);
		return ;
	}
}
void InputRental_Client(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* Rentallist, LINKEDLIST* SameNameRentalList, void* Member, void* Video,int VideoNum)
{
	RENTAL* pRental = (RENTAL*)malloc(sizeof(RENTAL));
	InPutRentalData_Client(Memberlist, Videolist, SameNameRentalList, pRental, Member, Video,VideoNum);
	Rentallist->AddBottom(Rentallist, (void *)pRental);

}

int ReturnVideo_Client(LINKEDLIST* Rentallist, LINKEDLIST* Videolist, int VideoNum)
{
	NODE * pFindFlag = SSearchRentalNumber(Rentallist, VideoNum);
	NODE * pFindVideo = SSearchNumber(Videolist, VideoNum);
	if (pFindFlag == NULL){
		printf("��ġ�ϴ� ���� ��ȣ�� �����ϴ�.\n");
		return 1;
	}
	((VIDEO *)pFindFlag->pData)->bRent = true;
	((VIDEO *)pFindVideo->pData)->bRent = true;
	DeleteAt(Rentallist, pFindFlag);
	printf(" �� ������ �ݳ�����ϴ�.\n");
	return 0;
}
void ReturnVideo(LINKEDLIST* Rentallist, LINKEDLIST* Videolist){
	int temp = 0;
	printf("�ݳ��� ���� ��ȣ : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchRentalNumber(Rentallist, temp);
	NODE * pFindVideo = SSearchNumber(Videolist, temp);
	if (pFindFlag == NULL){
		printf("��ġ�ϴ� ���� ��ȣ�� �����ϴ�.\n");
		return ;
	}
	((VIDEO *)pFindFlag->pData)->bRent = true;
	((VIDEO *)pFindVideo->pData)->bRent = true;
	DeleteAt(Rentallist,pFindFlag);
	printf("������ �ݳ��߽��ϴ�.\n");

}

void PrintRentallist(LINKEDLIST* Rentallist){
	NODE * pStart = Rentallist->pTop;
	while (pStart)
	{
		printf("ȸ�� �̸� : %s\n", ((RENTAL*)(pStart->pData))->pRMemberName);
		printf("���� ��ȣ : %d\n", (((RENTAL*)pStart->pData))->iRVideoNum);
		printf("���� �̸� : %s\n", ((RENTAL*)(pStart->pData))->pRVideoName);
		if (((VIDEO*)(pStart->pData))->bRent == true){
			printf("�뿩 ����\n");
		}
		else{
			printf("�뿩��...�뿩�Ұ���\n");
		}
		pStart = pStart->pNext;
		printf("\n");
	}
	if (Rentallist->iCount == 0)
		printf("�뿩���� ������ �����ϴ�.");
	printf("\n");
}
void PrintMemberRentalList(LINKEDLIST* Rentallist,char temp[])
{
	NODE * pStart = Rentallist->pTop;
	while (pStart)
	{
		if (strcmp(((RENTAL*)(pStart->pData))->pRMemberName, temp) == 0)
		{
			printf("ȸ�� �̸� : %s\n", ((RENTAL*)(pStart->pData))->pRMemberName);
			printf("���� ��ȣ : %d\n", (((RENTAL*)pStart->pData))->iRVideoNum);
			printf("���� �̸� : %s\n", ((RENTAL*)(pStart->pData))->pRVideoName);
		}		
		pStart = pStart->pNext;
	}
}
void File_Write_Member(LINKEDLIST* MemberList)
{
	FILE *fp;
	printf("ȸ���� ����� �����մϴ�.\n");
	fopen_s(&fp,"MemberList.txt", "w");
	NODE * pStart = MemberList->pTop;
	fprintf(fp, "%d\n", MemberList->iCount);
	for (int index = 0; index < MemberList->iCount; index++)
	{
		fprintf(fp, "%s\n", ((MEMBER*)(pStart->pData))->pMemberID);
		fprintf(fp, "%s\n", ((MEMBER*)(pStart->pData))->pPassWord);
		fprintf(fp, "%s\n", ((MEMBER*)(pStart->pData))->pName);
		fprintf(fp, "%d\n", ((MEMBER*)(pStart->pData))->pAge);
		fprintf(fp, "%s\n", ((MEMBER*)(pStart->pData))->pPhone);
		pStart = pStart->pNext;
	}
	fclose(fp);
}
void File_Read_char(FILE * fp, char **pData)
{
	char temp[1024] = { " " };
	fgets(temp, 1024, fp);
	fflush(stdin);
	*pData = (char *)malloc(strlen(temp));
	strcpy_s(*pData, strlen(temp) + 1, temp);
	(*pData)[strlen(temp) - 1] = NULL;
}
void File_Read_int(FILE * fp, int *pData)
{
	fscanf_s(fp, "%d\n", pData);
	fflush(stdin);
	
}
void File_Read_bool(FILE * fp, bool *pData)
{
	fscanf_s(fp, "%d\n", pData);
	fflush(stdin);

}

void File_Read_Member(LINKEDLIST* MemberList)
{
	FILE *fp;
	fopen_s(&fp, "MemberList.txt", "r");
	if (fp == NULL)
		printf("�ּҷ��� �����ϴ�.\n");
	else{
		printf("�ּҷ��� �о�ɴϴ�.\n");
		int count=0;
		File_Read_int(fp, &count);
		
		while (count!=0)
		{
			MEMBER * pMember = (MEMBER *)malloc(sizeof(MEMBER));

			File_Read_char(fp, &(pMember->pMemberID));
			File_Read_char(fp, &(pMember->pPassWord));
			File_Read_char(fp, &(pMember->pName));
			File_Read_int(fp, &(pMember->pAge));
			File_Read_char(fp, &(pMember->pPhone));

			MemberList->AddBottom(MemberList, (void *)pMember);
			--count;
		}
		fclose(fp);
	}
}
void File_Write_Video(LINKEDLIST* VideoList)
{
	FILE *fp;
	printf("���� ����� �����մϴ�.\n");
	fopen_s(&fp, "VideoList.txt", "w");
	NODE * pStart = VideoList->pTop;
	fprintf(fp, "%d\n", VideoList->iCount);
	for (int index = 0; index < VideoList->iCount; index++)
	{
		fprintf(fp, "%d\n", ((VIDEO*)(pStart->pData))->iVNumber);
		fprintf(fp, "%s\n", ((VIDEO*)(pStart->pData))->pVName);
		fprintf(fp, "%d\n", ((VIDEO*)(pStart->pData))->iGrade);
		fprintf(fp, "%d\n", ((VIDEO*)(pStart->pData))->bRent);
		fprintf(fp, "%d\n", ((VIDEO*)(pStart->pData))->bReservation);
		pStart = pStart->pNext;
	}
	fclose(fp);
}
void File_Read_Video(LINKEDLIST* VideoList)
{
	FILE *fp;
	fopen_s(&fp, "VideoList.txt", "r");
	if (fp == NULL)
		printf("���� ����� �����ϴ�.\n");
	else{
		printf("���� ����� �о�ɴϴ�.\n");
		int count = 0;
		File_Read_int(fp, &count);

		while (count != 0)
		{
			VIDEO * pVideo = (VIDEO *)malloc(sizeof(VIDEO));

			File_Read_int(fp, &(pVideo->iVNumber));
			File_Read_char(fp, &(pVideo->pVName));
			File_Read_int(fp, &(pVideo->iGrade));
			File_Read_bool(fp, &(pVideo->bRent));
			File_Read_bool(fp, &(pVideo->bReservation));

			VideoList->AddBottom(VideoList, (void *)pVideo);
			--count;
		}
		fclose(fp);
	}
}
void File_Write_Rental(LINKEDLIST* RentalList)
{
	FILE *fp;
	printf("���� �뿩 ����� �����մϴ�.\n");
	fopen_s(&fp, "RentalList.txt", "w");
	NODE * pStart = RentalList->pTop;
	fprintf(fp, "%d\n", RentalList->iCount);
	for (int index = 0; index < RentalList->iCount; index++)
	{
		RENTAL * p = (RENTAL*)(pStart->pData);
		fprintf(fp, "%s\n", ((RENTAL*)(pStart->pData))->pMemberID);
		fprintf(fp, "%s\n", ((RENTAL*)(pStart->pData))->pRMemberName);
		fprintf(fp, "%d\n", ((RENTAL*)(pStart->pData))->iRVideoNum);
		fprintf(fp, "%s\n", ((RENTAL*)(pStart->pData))->pRVideoName);
		pStart = pStart->pNext;
	}
	fclose(fp);
}
void File_Read_Rental(LINKEDLIST* RentalList)
{
	FILE *fp;
	fopen_s(&fp, "RentalList.txt", "r");
	if (fp == NULL)
		printf("���� �뿩 ����� �����ϴ�.\n");
	else{
		printf("���� �뿩 ����� �о�ɴϴ�.\n");
		int count = 0;
		File_Read_int(fp, &count);

		while (count != 0)
		{
			RENTAL * pRental = (RENTAL *)malloc(sizeof(RENTAL));
			File_Read_char(fp, &(pRental->pMemberID));
			File_Read_char(fp, &(pRental->pRMemberName));
			File_Read_int(fp, &(pRental->iRVideoNum));
			File_Read_char(fp, &(pRental->pRVideoName));

			RentalList->AddBottom(RentalList, (void *)pRental);
			--count;
		}
		fclose(fp);
	}
}
