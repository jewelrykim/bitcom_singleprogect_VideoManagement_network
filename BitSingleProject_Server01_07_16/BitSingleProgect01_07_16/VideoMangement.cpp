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

	char* pRecvMemberID = (char*)temp +sizeof(PACKET);	// client가보내준 아이디
	//temp는 char 배열의 시작주소이다.+ packet(index)의 크기=>128byte만 받기 위해서 배열로 포인터로 바꿔준다
	char *pRecvMemberPW = (char*)temp + sizeof(PACKET)+128; //client 보낸 비밀번호
	//id(128Byte) 이후의 데이터pw
	char*pRecvMemberName = (char*)temp + sizeof(PACKET)+256;	// client가보내준 회원이름
	int pRecvMemberAge = *((char*)temp + sizeof(PACKET)+384);		// client가보내준 회원나이
	char*pRecvMemberPhoneNumber = (char*)temp + sizeof(PACKET)+388; //client 보낸 회원전화번호

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
	InsertChar("회원 ID : ", &(pMember->pMemberID));
	InsertChar("회원 PassWord : ", &(pMember->pPassWord));
	InsertChar("회원 이름 : ", &(pMember->pName));
	InsertInt("회원 나이 : ", &(pMember->pAge));
	InsertChar("회원 전화번호 : ", &(pMember->pPhone));

}
void InputVideoData(VIDEO * pVideo){
	int switch_on = 0;
	InsertInt("비디오 번호 : ", &(pVideo->iVNumber));
	InsertChar("비디오 이름 : ", &(pVideo->pVName));
	InsertInt("비디오 등급 : ", &(pVideo->iGrade));
	printf("1.대여 가능, 2.대여 불가 \n");
	scanf_s("%d", &switch_on);
	fflush(stdin);
	switch (switch_on)
	{
	case 1:
		printf("대여 가능\n");
		pVideo->bRent = true;
		break;
	default:
		printf("대여중 \n");
		pVideo->bRent = false;
		break;
	}
}
void InputMember_Client(LINKEDLIST* list,void* temp)
{		//temp는 client가 입력한 데이터를 그대로 가져온 것이다. void를 풀고 파라메타로 받으면 파라메타수가 너무 많아지기 때문에 그대로 받았다.
	MEMBER * pMember = (MEMBER *)malloc(sizeof(MEMBER));
	InputMemberData_Client(pMember, temp);
	list->AddBottom(list, (void *)pMember);
}
void InputMember_Server(LINKEDLIST* list)
{	
	MEMBER * pMember = (MEMBER *)malloc(sizeof(MEMBER));
	InputMemberData_Server(pMember);
	list->AddBottom(list, (void *)pMember);
}
void InputVideo(LINKEDLIST* list){
	VIDEO * pVideo = (VIDEO *)malloc(sizeof(VIDEO));
	InputVideoData(pVideo);
	list->AddBottom(list, (void *)pVideo);
}
void PrintMember(NODE * pStart){
		printf("회원 아이디 : %s\n", ((MEMBER*)(pStart->pData))->pMemberID);
		printf("회원 비밀번호 : %s\n", ((MEMBER*)(pStart->pData))->pPassWord);
		printf("회원 이름 : %s\n", ((MEMBER*)(pStart->pData))->pName);
		printf("회원 나이 : %d\n", ((MEMBER*)(pStart->pData))->pAge);
		printf("회원 전화번호 : %s\n", ((MEMBER*)(pStart->pData))->pPhone);
}
void PrintVideo(NODE * pStart){
	printf("비디오 번호 : %d\n", ((VIDEO*)(pStart->pData))->iVNumber);
	printf("비디오 이름 : %s\n", ((VIDEO*)(pStart->pData))->pVName);
	printf("비디오 등급 : %d\n", ((VIDEO*)(pStart->pData))->iGrade);
	if (((VIDEO*)(pStart->pData))->bRent == true){
		printf("대여 가능\n");
	}
	else{
		printf("대여중...대여불가능\n");
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
NODE * SSearchVideoName(LINKEDLIST* list, char* iSearch)
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
	printf("찾을 회원의 이름 : ");
	gets_s(temp, sizeof(temp));
	fflush(stdin);
	NODE * pFindFlag = SSearchName(list, temp);
	if (pFindFlag == NULL){
		printf("일치하는 회원이 없습니다.");
		return;
	}
	PrintMember(pFindFlag);
}

void SearchVideo(LINKEDLIST* list){
	int temp=0;
	printf("찾을 비디오 번호 : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(list, temp);
	if (pFindFlag == NULL){
		printf("일치하는 비디오가 없습니다.");
		return;
	}
	PrintVideo(pFindFlag);
}
void ModifyMember(LINKEDLIST* list)	//temp는 client가 입력한 데이터를 그대로 가져온 것이다. void를 풀고 파라메타로 받으면 파라메타수가 너무 많아지기 때문에 그대로 받았다.
{
	char temp[1024] = { ' ' };
	printf("수정할 회원 이름 : ");
	gets_s(temp, sizeof(temp));
	fflush(stdin);
	NODE * pFindFlag = SSearchName(list, temp);
	if (pFindFlag == NULL){
		printf("일치하는 회원이 없습니다.");
		return;
	}
	InputMemberData_Server((MEMBER*)pFindFlag->pData);
}
void ModifyVideo(LINKEDLIST* list)	//temp는 client가 입력한 데이터를 그대로 가져온 것이다. void를 풀고 파라메타로 받으면 파라메타수가 너무 많아지기 때문에 그대로 받았다.
{
	int temp = 0;
	printf("찾을 비디오 번호 : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(list, temp);
	if (pFindFlag == NULL){
		printf("일치하는 비디오가 없습니다.");
		return;
	}
	InputVideoData((VIDEO*)pFindFlag->pData);
}
void DeleteMember(LINKEDLIST* list){
	char temp[1024] = { ' ' };
	printf("삭제할 회원 이름 : ");
	gets_s(temp, sizeof(temp));
	fflush(stdin);
	NODE * pFindFlag = SSearchName(list, temp);
	if (pFindFlag)
	{
		list->DeleteAt(list, pFindFlag);
	}
	else
	{
		printf("일치하는 회원이 없습니다.");
	}
}
void DeleteVideo(LINKEDLIST* list){
	int temp = 0;
	printf("삭제할 비디오 번호 : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(list, temp);
	if (pFindFlag)
	{
		list->DeleteAt(list, pFindFlag);
	}
	else
	{
		printf("일치하는 비디오가 없습니다.");
	}
}
/*
void InputRentalData(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, NODE** Rebtallist){
	RENTAL* pRental = (RENTAL*)malloc(sizeof(RENTAL));
	(*Rebtallist)->pData = pRental;
	char Name[1024] = { ' ' };
	int Number = 0;
	printf("회원 이름 : ");
	gets_s(Name, sizeof(Name));
	fflush(stdin);
	NODE * pFindMember = SSearchName(Memberlist, Name);
	if (pFindMember == NULL){
		printf("그런고객 없다.");
		return;
	}
	((RENTAL*)((*Rebtallist)->pData))->pRMemberName = ((MEMBER*)pFindMember->pData)->pName;

	printf("비디오 번호 : ");
	scanf_s("%d", &Number);
	fflush(stdin);
	NODE * pFindVideo = SSearchNumber(Videolist, Number);
	if (pFindVideo == NULL){
		printf("그런비디오 없다.\n");
		return;
	}
	if (((VIDEO*)(pFindVideo->pData))->bRent == false)
	{
		printf("이미 대여중.\n");
		return;
	}
	if (((VIDEO*)(pFindVideo->pData))->iGrade > ((MEMBER*)pFindMember->pData)->pAge)
	{
		printf("넌 못봐\n");
		return;
	}
	((VIDEO*)(pFindVideo->pData))->bRent = false;
	((RENTAL*)((*Rebtallist)->pData))->iRVideoNum = ((VIDEO*)pFindVideo->pData)->iVNumber;
	((RENTAL*)((*Rebtallist)->pData))->pRVideoName = ((VIDEO*)pFindVideo->pData)->pVName;

}*/
void InputRentalData(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, RENTAL* pRental){

	char Name[1024] = { ' ' };
	int Number = 0;
	printf("회원 이름 : ");
	gets_s(Name, sizeof(Name));
	fflush(stdin);
	NODE * pFindMember = SSearchName(Memberlist, Name);
	if (pFindMember == NULL){
		printf("일치하는 회원이 없습니다.");
		return;
	}
	pRental->pRMemberName = ((MEMBER*)pFindMember->pData)->pName;

	printf("비디오 번호 : ");
	scanf_s("%d", &Number);
	fflush(stdin);
	NODE * pFindVideo = SSearchNumber(Videolist, Number);
	if (pFindVideo == NULL){
		printf("일치하는 비디오가 없습니다.\n");
		return;
	}
	if (((VIDEO*)(pFindVideo->pData))->bRent == false)
	{
		printf("이미 대여중입니다.\n");
		return;
	}
	if (((VIDEO*)(pFindVideo->pData))->iGrade > ((MEMBER*)pFindMember->pData)->pAge)
	{
		printf("%d 이상 관람가 비디오입니다.\n", ((VIDEO*)(pFindVideo->pData))->iGrade);
		return;
	}
	((VIDEO*)(pFindVideo->pData))->bRent = false;
	pRental->iRVideoNum = ((VIDEO*)pFindVideo->pData)->iVNumber;
	pRental->pRVideoName = ((VIDEO*)pFindVideo->pData)->pVName;

}
void InPutRentalData_Client(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, RENTAL* pRental, void* Member, void* Video)
{
	
	NODE * pFindMember = SSearchName(Memberlist, (char*) Member);		//일치하는 고객이 없을 수 없다.
	pRental->pRMemberName = ((MEMBER*)pFindMember->pData)->pName;

	NODE * pFindVideo = SSearchVideoName(Videolist, (char*)Video);			//일치하는 비디오가 없을 수 없다.
	pRental->pRVideoName = ((VIDEO*)pFindVideo->pData)->pVName;			//이미 검사를 끝냈기 때문에 못빌릴 수 없다.

	((VIDEO*)(pFindVideo->pData))->bRent = false;
	pRental->iRVideoNum = ((VIDEO*)pFindVideo->pData)->iVNumber;
		
}
void InputRental(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* Rentallist)
{
	RENTAL* pRental = (RENTAL*)malloc(sizeof(RENTAL));
	InputRentalData(Memberlist, Videolist, pRental);
	Rentallist->AddBottom(Rentallist, (void *)pRental);
}
void InputRental_Client(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* Rentallist,void* Member,void* Video)
{
	RENTAL* pRental = (RENTAL*)malloc(sizeof(RENTAL));
	InPutRentalData_Client(Memberlist, Videolist, pRental, Member, Video);
	Rentallist->AddBottom(Rentallist, (void *)pRental);

}

void ReturnVideo(LINKEDLIST* Rentallist){
	int temp = 0;
	printf("반납할 비디오 번호 : ");
	scanf_s("%d", &temp);
	fflush(stdin);
	NODE * pFindFlag = SSearchNumber(Rentallist, temp);
	if (pFindFlag == NULL){
		printf("일치하는 비디오 번호가 없습니다.");
		return;
	}
	((VIDEO *)pFindFlag->pData)->bRent = true;
	DeleteAt(Rentallist,pFindFlag);
}

void PrintRentallist(LINKEDLIST* Rentallist){
	NODE * pStart = Rentallist->pTop;
	while (pStart)
	{
		printf("회원 이름 : %s\n", ((RENTAL*)(pStart->pData))->pRMemberName);
		printf("비디오 번호 : %d\n", (((RENTAL*)(pStart)->pData))->iRVideoNum);
		printf("비디오 이름 : %s\n", ((RENTAL*)(pStart->pData))->pRVideoName);
		if (((VIDEO*)(pStart->pData))->bRent == true){
			printf("대여 가능\n");
		}
		else{
			printf("대여중...대여불가능\n");
		}
		pStart = pStart->pNext;
	}
}
void File_Write_Member(LINKEDLIST* MemberList)
{
	FILE *fp;
	printf("회원의 명단을 저장합니다.\n");
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

void File_Read_Member(LINKEDLIST* MemberList )
{
	FILE *fp;
	fopen_s(&fp, "MemberList.txt", "r");
	if (fp == NULL)
		printf("주소록이 없습니다.\n");
	else{
		printf("주소록을 읽어옵니다.\n");
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
	printf("비디오 목록을 저장합니다.\n");
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
		printf("비디오 목록이 없습니다.\n");
	else{
		printf("비디오 목록을 읽어옵니다.\n");
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
	printf("비디오 대여 목록을 저장합니다.\n");
	fopen_s(&fp, "RentalList.txt", "w");
	NODE * pStart = RentalList->pTop;
	fprintf(fp, "%d\n", RentalList->iCount);
	for (int index = 0; index < RentalList->iCount; index++)
	{
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
		printf("비디오 대여 목록이 없습니다.\n");
	else{
		printf("비디오 대여 목록을 읽어옵니다.\n");
		int count = 0;
		File_Read_int(fp, &count);

		while (count != 0)
		{
			RENTAL * pRental = (RENTAL *)malloc(sizeof(RENTAL));

			File_Read_char(fp, &(pRental->pRMemberName));
			File_Read_int(fp, &(pRental->iRVideoNum));
			File_Read_char(fp, &(pRental->pRVideoName));

			RentalList->AddBottom(RentalList, (void *)pRental);
			--count;
		}
		fclose(fp);
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
		printf("\nmode 선택 ( 1 = 사용자 관리 , 2 = 비디오 관리 , 3 = 대여 관리, 0 = 종료) \n");
		mode = getchar();
		fflush(stdin);

		switch (mode)
		{
		case '1':
			printf("\nmode 선택 ( 1 = 회원 등록 , 2 = 회원 목록 , 3 = 회원 검색, 4 = 회원 정보수정 ,5 = 회원 삭제 0 = 돌아가기) \n");
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
			printf("\nmode 선택 ( 1 = 비디오 등록 , 2 = 비디오 목록 , 3 = 비디오 검색, 4 = 비디오 정보수정 ,5 = 비디오 삭제 0 = 돌아가기) \n");
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
			printf("\nmode 선택 ( 1 = 비디오 대여 , 2 = 비디오 반납 , 3 = 비디오 대여목록 ,0 = 돌아가기) \n");
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
