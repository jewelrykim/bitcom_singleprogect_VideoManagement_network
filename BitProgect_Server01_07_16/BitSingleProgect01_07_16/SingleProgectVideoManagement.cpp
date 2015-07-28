# define _WINSOCK_DEPRECATED_NO_WARNING
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#include <windows.h>
#include "VideoManagement.h"

#pragma  comment(lib,"Ws2_32.lib")	//라이브러리 추가



#define  PG_LOGIN 1
#define  PG_SIGNUP 2

#define  PG_VIDEOSEARCH 3
#define  PG_VIDEORENTAL 4
#define  PG_VIDEOTABLE 5
#define  PG_VIDEOTABLEEND 6

#define  PG_VIDEORENTALPOSIBLE 7
#define  PG_VIDEORENTALRESERVATION 8

#define  PG_MODITYDATA 9

#define  PG_RENTALLIST 12
#define  PG_RENTALLISTEND 13

#define  PG_RETURNVIDEO 15

#define  PG_LOGOUT 19
#define  PG_WITHDREW 20

#define  PG_FAIL 21
#define  PG_VIDEOSEARCHFAIL 22
#define  PG_WITHDREWFAIL 23
#define  PG_NOVIDEO 24


typedef struct _THREADPARM
{
	LINKEDLIST * pLinkedList;
	LINKEDLIST * pMemberLIst;
	LINKEDLIST * pVideoList;
	LINKEDLIST * pRenTalList;
	LINKEDLIST * pLogInList;
	LINKEDLIST * pSameNameRenTalList;
	SOCKET Socket;
}THREADPARAM;

void SendMessage(SOCKET ClientSocket, const char* pMsg, int iMsgLen)
{
	send(ClientSocket, pMsg, iMsgLen, NULL);//send(소켓,데이터,데이터길이,플레그)
}

void SendPacket(SOCKET ClientSocket, int iPacketGubun, void* pMsg, int iMsgLen)
{
	char temp[4096];
	PACKET packet;
	packet.iPacketGubun = iPacketGubun;
	packet.iTotalSize = sizeof(packet)+iMsgLen;
	memcpy(temp, &packet, sizeof(packet));
	memcpy(temp + sizeof(packet), pMsg, iMsgLen);
	SendMessage(ClientSocket, temp, packet.iTotalSize);
}
bool GetPacket(char RecvBuffer[], int *piCurrent, char temp[])
{
	if (*piCurrent < sizeof(PACKET))
	{
		return false;
	}
	PACKET* pPacket = (PACKET*)RecvBuffer;
	if (*piCurrent < pPacket->iTotalSize)
	{
		return false;
	}
	memcpy(temp, RecvBuffer, pPacket->iTotalSize);
	*piCurrent -= pPacket->iTotalSize;
	memcpy(RecvBuffer, RecvBuffer + pPacket->iTotalSize, *piCurrent);
	return true;
}
bool EqualFunc(NODE* pNode, void* pData)
{
	return pNode->pData == pData;
}
bool DeleteClient(LINKEDLIST * pList, SOCKET DeleteSocket)
{
	if (pList->iCount)
	{
		return false;
	}
	NODE*pSearch = Search(pList, EqualFunc, (void*)DeleteSocket);
	if (pSearch)
	{
		SOCKET ClientSocket = (SOCKET)pSearch->pData;
		DeleteAt(pList, pSearch);
		SendPacket(ClientSocket, PG_FAIL,"로그아웃\n", 128);
		return true;
	}
	return false;
}
void SendlogInData(SOCKET RemoteSocket,void* pMember)		//login 성공시 client에게 memberlist에서 찾은 클라이언트의 정보를 보내준다.
{
	char LongIn[516];

	memcpy(LongIn, ((MEMBER*)pMember)->pMemberID, 128);
	memcpy(LongIn + 128, ((MEMBER*)pMember)->pPassWord, 128);
	memcpy(LongIn + 256, ((MEMBER*)pMember)->pName, 128);
	memcpy(LongIn + 384, &((MEMBER*)pMember)->pAge, 4);
	memcpy(LongIn + 388, ((MEMBER*)pMember)->pPhone, 128);
	//printf("\n");
	SendPacket(RemoteSocket, PG_LOGIN, LongIn, 516);

}
void SendVideoData(SOCKET RemoteSocket, void*pVideo, int mode)
{
	char VideoData[144] = {" "};

	memcpy(VideoData, &((VIDEO*)pVideo)->iVNumber, 4);
	memcpy(VideoData +4, ((VIDEO*)pVideo)->pVName, 128);
	memcpy(VideoData +132, &((VIDEO*)pVideo)->iGrade, 4);
	memcpy(VideoData+136, &((VIDEO*)pVideo)->bRent, 4);
	memcpy(VideoData+140, &((VIDEO*)pVideo)->bReservation, 4);
	SendPacket(RemoteSocket, mode, VideoData, 144);
}
void SendRentalData(SOCKET RemoteSocket, void*pRental, int mode)
{
	char RentalData[132] = { " " };

	memcpy(RentalData , &((RENTAL*)pRental)->iRVideoNum, 4);
	memcpy(RentalData + 4, ((RENTAL*)pRental)->pRVideoName, 128);
	SendPacket(RemoteSocket, mode, RentalData, 132);
}
unsigned CALLBACK RecvFunc(void* pParam)
{
	THREADPARAM *p = (THREADPARAM*)pParam;
	LINKEDLIST *pList = p->pLinkedList;
	LINKEDLIST *pMemberList = p->pMemberLIst;
	LINKEDLIST *pVideoList = p->pVideoList;
	LINKEDLIST *pRentList = p->pRenTalList;
	LINKEDLIST *pLogInList = p->pLogInList;
	LINKEDLIST *pSameNameRentalList = p->pSameNameRenTalList;

	SOCKET ClientSocket = p->Socket;
	char RecvBuffer[4096*2];
	int iCurrent = 0;
	char * pID = {NULL};
	char * pPW = {NULL};	//id(128Byte) 이후의 데이터pw
	char * pVideoName = {NULL};
	int iVideoNum = 0;

	NODE * pFindFlag;
	NODE * pLogInFlag;
	NODE * pFindVideoFlag;
	NODE * pStart;
	while (1){
		char temp[4096];
		int iRecvLen = recv(ClientSocket, RecvBuffer + iCurrent, 4096, NULL);
		if (iRecvLen <= 0)
		{
			DeleteClient(p->pLinkedList, ClientSocket);
			if (*pID != NULL)
			{
				DeleteMember_Client(pLogInList, pID);
			}
			break;
		}
		iCurrent += iRecvLen;

		while (GetPacket(RecvBuffer, &iCurrent, temp))
		{
			PACKET* pPacket = (PACKET*)temp;
			switch (pPacket->iPacketGubun)
			{
			case PG_LOGIN:
				pID = temp + sizeof(PACKET);
				pPW = temp + sizeof(PACKET)+128;	//id(128Byte) 이후의 데이터pw
					pFindFlag = SSearchID(pMemberList, pID);	//memberlist에서 찾는다.
					pLogInFlag = SSearchID(pLogInList, pID);	//현재login되있는 유저와 같은 아이디로 로그인 할 수 없다
					if (pFindFlag != NULL && strcmp(((MEMBER*)(pFindFlag->pData))->pPassWord, pPW) == 0)
					{
						if (pLogInFlag != NULL)
						{
							SendPacket(ClientSocket, PG_FAIL, "접속중인 ID입니다.\n", 128);
							break;
						}
						SendlogInData(ClientSocket, (MEMBER *)pFindFlag->pData);
						pLogInList->AddBottom(pLogInList, (void*)(pFindFlag->pData));
						break;
					}
					else{
						 SendPacket(ClientSocket, PG_FAIL, "LOG_IN 실패\n", 128);
						 break;
					}			
					
			case PG_SIGNUP:
				pID = temp + sizeof(PACKET);
				pFindFlag = SSearchID(pMemberList, pID);	//memberlist에서 찾는다
				if (pFindFlag != NULL)
				{
					SendPacket(ClientSocket, PG_FAIL, "같은 ID 가 존재합니다\n", 128);
					break;
				}
				InputMember_Client(pMemberList, pLogInList, (void*)temp);
					SendPacket(ClientSocket, PG_LOGIN, (char*)temp + sizeof(PACKET), pPacket->iTotalSize);
					break;
			case PG_LOGOUT:
				pID = temp + sizeof(PACKET);
				DeleteMember_Client(pLogInList, pID);			//login(현재 접속자)list에서 삭제
				break;
			case PG_VIDEOSEARCH:
					pVideoName = temp + sizeof(PACKET);
					pFindVideoFlag = SSearchVideoName(pVideoList, pVideoName);	//videolist에서 찾는다.
					if (pFindVideoFlag != NULL){
						SendVideoData(ClientSocket, pFindVideoFlag->pData, PG_VIDEOSEARCH);
						break;
					}
					else{
						SendPacket(ClientSocket, PG_VIDEOSEARCHFAIL, "", 0);
						break;
					}
			case PG_VIDEOTABLE:			//전체 테이블 출력
				pStart = pVideoList->pTop;
				while (pStart)
				{
					SendVideoData(ClientSocket, pStart->pData, PG_VIDEOTABLE);
					pStart = pStart->pNext;
				}
				if (pVideoList->iCount==0)
				{
					SendPacket(ClientSocket, PG_VIDEOTABLEEND, "비디오 목록이 없습니다.", 32);
					break;
				}
				else{
					SendPacket(ClientSocket, PG_VIDEOTABLEEND, "", 0);
					break;
				}
			case PG_VIDEORENTAL:
				pVideoName = temp + sizeof(PACKET);
				pSameNameRentalList->DeleteAll(pSameNameRentalList);
				pFindVideoFlag = MSearchVideoName(pVideoList,pSameNameRentalList, pVideoName);	//videolist에서 찾는다.
				pStart = pSameNameRentalList->pTop;
				if (pFindVideoFlag == NULL){		//비디오가 없을때
					SendPacket(ClientSocket, PG_VIDEOSEARCHFAIL, "", 0);
					break;
				}
				else{
					while (pStart){
						if (((VIDEO*)(pStart->pData))->bRent == true){
							//SendVideoData(ClientSocket, pStart->pData, PG_VIDEORENTAL);
							break;
						}
						pStart = pStart->pNext;
					}
					if (pStart != NULL)
					{
						SendVideoData(ClientSocket, pStart->pData, PG_VIDEORENTAL);
						break;
					}
					else 
					{
						SendPacket(ClientSocket, PG_NOVIDEO, "모든 비디오가 대여중입니다", 32);
					}
					break;
				}
			case PG_VIDEORENTALPOSIBLE:
				pID = temp + sizeof(PACKET);
				pVideoName = temp + sizeof(PACKET)+128;
				memcpy(&iVideoNum, temp + sizeof(PACKET)+256,4);
				InputRental_Client(pMemberList, pVideoList, pRentList, pSameNameRentalList, pID, pVideoName,iVideoNum);
				SendPacket(ClientSocket, PG_VIDEORENTALPOSIBLE, "대여 성공", 128);
				break;

			case PG_MODITYDATA:
				ModifyMember_Client(pMemberList, temp);
				break;

			case PG_WITHDREW:
				pID = temp + sizeof(PACKET);
				pStart = pRentList->pTop;
				while (pStart)
				{
					if (strcmp(pID, ((RENTAL*)(pStart->pData))->pMemberID)==0)
					{
						SendPacket(ClientSocket, PG_WITHDREWFAIL, "대여중인 비디오가있어서 탈퇴할 수 없습니다.비디오를 반납하세요", 128);
						break;
					}
					pStart = pStart->pNext;
				}
				if (pStart == false)
				{
					SendPacket(ClientSocket, PG_WITHDREW, "", 0);
					DeleteMember_Client(pMemberList, pID);
				}
				break;

			case PG_RENTALLIST:
				pID = temp + sizeof(PACKET);
				pStart = pRentList->pTop;
				while (pStart)
				{
					
					if (strcmp(((RENTAL*)(pStart->pData))->pMemberID, pID) == 0)
					{
						SendRentalData(ClientSocket, pStart->pData, PG_RENTALLIST);
					}
					pStart = pStart->pNext;
				}
				if (pRentList->iCount == 0)
				{
					SendPacket(ClientSocket, PG_RENTALLISTEND, "비디오 목록이 없습니다.", 32);
					break;
				}
				else{
					SendPacket(ClientSocket, PG_RENTALLISTEND, "", 0);
					break;
				}
				break;
			case PG_RETURNVIDEO:
				iVideoNum = (int)*(temp + sizeof(PACKET));
				printf("%d", iVideoNum);
				if (ReturnVideo_Client(pRentList, pVideoList, iVideoNum) == 0)
				{
					SendPacket(ClientSocket, PG_RETURNVIDEO, "비디오를 반납했습니다.", 32);
				}
				else
					SendPacket(ClientSocket, PG_NOVIDEO, "대여한 비디오가 아닙니다.", 32);
				break;
			}

			/*if (pPacket->iPacketGubun == PG_LOGIN)
			{
				char * pID = temp + sizeof(PACKET);
				char * pPW = temp + sizeof(PACKET) + 128;
				//id(128Byte) 이후의 데이터pw
				NODE * pFindFlag = SSearchName(pMemberList, pID);	//memberlist에서 찾는다.
				if (pFindFlag == NULL){
					DeleteBottom(pList);
					SendPacket(ClientSocket, PG_KILLUSER, "", 0);
					break;
				}
				SendlogInData(ClientSocket, (void*)pFindFlag);
			}
			else if (pPacket->iPacketGubun == PG_SIGNUP)
			{

				InputMember(pMemberList,(void*)temp);
				SendPacket(ClientSocket, PG_LOGIN, (char*)temp + sizeof(PACKET), pPacket->iTotalSize);
				break;
			}
			else if (pPacket->iPacketGubun == PG_CHATMSG)
			{
				printf("우와 자유민이다\n" );
			}*/
		}
	}

	free(p);
	closesocket(ClientSocket);
	return 0;
}



unsigned CALLBACK AcceptFunc(void* pParam){
	THREADPARAM * p = (THREADPARAM*)pParam;
	SOCKET ServerSocket = p->Socket;
	LINKEDLIST *pList = p->pLinkedList;
	LINKEDLIST *pMember = p->pMemberLIst;
	LINKEDLIST *pVideo = p->pVideoList;
	LINKEDLIST *pRent = p->pRenTalList;
	LINKEDLIST *pLogInlist = p->pLogInList;
	LINKEDLIST *pSameNameRentalList = p->pSameNameRenTalList;

	while (1)
	{
		SOCKADDR_IN addr;
		memset(&addr, 0, sizeof(addr));
		int iAddrLen = sizeof(addr);
		SOCKET ClientSocket = accept(ServerSocket, (sockaddr*)&addr, &iAddrLen);
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("Accpet ERROR");
			break;
		}
		AddBottom(pList, (void*)ClientSocket);

		THREADPARAM* pRecvParam = (THREADPARAM*)malloc(sizeof(THREADPARAM));
		pRecvParam->pLinkedList = pList;
		pRecvParam->pMemberLIst = pMember;
		pRecvParam->pVideoList = pVideo; 
		pRecvParam->pRenTalList = pRent;
		pRecvParam->pLogInList = pLogInlist;
		pRecvParam->pSameNameRenTalList = pSameNameRentalList;
		pRecvParam->Socket = ClientSocket;
		_beginthreadex(NULL, NULL, RecvFunc, (void*)pRecvParam, NULL, NULL);
		/*const char* pMsg = "접속을 환영합니다.~";
		SendPacket(ClientSocket, PG_CHATMSG, pMsg, strlen(pMsg) + 1);*/
	}
	return 0;
}


void main()
{
	LINKEDLIST LogInlist;
	LogInlist.InitList = InitLinkedList;
	LogInlist.InitList(&LogInlist, AddTop, AddBottom, AddPoint, DeleteAt, DeleteTop, DeleteBottom, DeleteAll);

	LINKEDLIST Memberlist;
	Memberlist.InitList = InitLinkedList;
	Memberlist.InitList(&Memberlist, AddTop, AddBottom, AddPoint, DeleteAt, DeleteTop, DeleteBottom, DeleteAll);

	LINKEDLIST Videolist;
	Videolist.InitList = InitLinkedList;
	Videolist.InitList(&Videolist, AddTop, AddBottom, AddPoint, DeleteAt, DeleteTop, DeleteBottom, DeleteAll);

	LINKEDLIST Rentallist;
	Rentallist.InitList = InitLinkedList;
	Rentallist.InitList(&Rentallist, AddTop, AddBottom, AddPoint, DeleteAt, DeleteTop, DeleteBottom, DeleteAll);

	LINKEDLIST pSameNameRenTalList;
	pSameNameRenTalList.InitList = InitLinkedList;
	pSameNameRenTalList.InitList(&pSameNameRenTalList, AddTop, AddBottom, AddPoint, DeleteAt, DeleteTop, DeleteBottom, DeleteAll);

	File_Read_Member(&Memberlist);
	File_Read_Video(&Videolist);
	File_Read_Rental(&Rentallist);

	LINKEDLIST list;
	memset(&list, 0, sizeof(list));

	WSADATA data;
	memset(&data, 0, sizeof(data));
	WSAStartup(MAKEWORD(2, 2), &data);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET)
	{
		printf("ServerSocket 생성실패");
		return;
	}
	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(50000);

	int iError = bind(ServerSocket, (sockaddr*)&addr, sizeof(addr));
	if (iError == SOCKET_ERROR)
	{
		printf("BIND ERROR\n");
		return;
	}
	iError = listen(ServerSocket, SOMAXCONN);
	if (iError == SOCKET_ERROR)
	{
		printf("LISTEN ERROR\n");
		return;
	}
	THREADPARAM AcceptParam;
	memset(&AcceptParam, 0, sizeof(AcceptParam));
	AcceptParam.pLinkedList = &list;
	AcceptParam.pMemberLIst = &Memberlist;
	AcceptParam.pVideoList = &Videolist;
	AcceptParam.pRenTalList = &Rentallist;
	AcceptParam.pLogInList = &LogInlist;
	AcceptParam.pSameNameRenTalList = &pSameNameRenTalList;
	AcceptParam.Socket = ServerSocket;
	_beginthreadex(NULL, NULL, AcceptFunc, (void*)&AcceptParam, NULL, NULL);


	int iChoice = 1;
	int iManageMamber = 1;
	int iManageVideo = 1;
	int iManageRental = 1;
	char temp[128];
	while (iChoice != 0)		//메인 루프
	{

		printf("1. 회원 관리 2. 비디오 관리 3. 비디오 대여 목록 관리 9.서버종료 \n");
		scanf_s("%d", &iChoice);
		fflush(stdin);
		switch (iChoice)
		{
		case 1:
			while (iManageMamber != 0)
			{
				printf("1. 접속자 관리 \n");
				printf("1.회원 추가 2. 회원 목록 3. 회원 검색 4. 회원 수정 5.회원 삭제 0.메인메뉴 \n");
				scanf_s("%d", &iManageMamber);
				fflush(stdin);
				switch (iManageMamber)
				{
				case 1:
					InputMember_Server(&Memberlist, &LogInlist);
					break;
				case 2:
					vPrintMember(&Memberlist);
					break;
				case 3:
					SearchMember(&Memberlist);
					break;
				case 4:
					ModifyMember_Server(&Memberlist);
					break;
				case 5:
					DeleteMember_Server(&Memberlist);
					break;
				case 6:
					iManageMamber = 0;
					break;
				}
				printf("\n");
			}
			iManageMamber = 1;
			break;
		case 2:
			while (iManageVideo != 0)
			{
				printf("2. 비디오 관리 \n");
				printf("1.비디오추가 2. 비디오 목록 3.비디오 검색 4. 비디오 수정 5. 비디오 삭제 0.메인메뉴\n");
				scanf_s("%d", &iManageVideo);
				fflush(stdin);
				switch (iManageVideo)
				{
				case 1:
					InputVideo(&Videolist);
					break;
				case 2:
					vPrintVideo(&Videolist);
					break;
				case 3:
					SearchVideo(&Videolist);
					break;
				case 4:
					ModifyVideo(&Videolist);
					break;
				case 5:
					DeleteVideo(&Videolist);
					break;
				case 0:
					iManageVideo = 0;
					break;
				}
				printf("\n");
			}
			iManageVideo = 1;
			break;
		case 3:
			while (iManageRental !=0)
			{
				printf("3. 비디오 대여관리\n1 = 비디오 대여 , 2 = 비디오 반납 , 3 = 비디오 대여목록 4 = 회원 대여목록 검색,0 =메인메뉴\n");
				scanf_s("%d", &iManageRental);
				fflush(stdin);
				switch (iManageRental){
				case 1:
					InputRental_Server(&Memberlist, &Videolist, &Rentallist);
					break;
				case 2:
					ReturnVideo(&Rentallist,&Videolist);
					break;
				case 3:
					PrintRentallist(&Rentallist);
					break;
				case 4:
					printf("찾을 회원의 이름 \n");
					gets_s(temp, sizeof(temp));
					fflush(stdin);
					PrintMemberRentalList(&Rentallist,temp);
					break;
				case 0:
					iManageRental = 0;
					break;
				}
				printf("\n");
			}
			iManageRental = 1;
			break;
		case 9:
			iChoice = 0;
			break;
		}
		File_Write_Member(&Memberlist);
		File_Write_Video(&Videolist);
		File_Write_Rental(&Rentallist);
	}		//메인루프 끝

	closesocket(ServerSocket);
	WSACleanup();
}