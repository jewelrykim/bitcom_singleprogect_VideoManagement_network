# define _WINSOCK_DEPRECATED_NO_WARNING
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#include <windows.h>
#include "VideoManagement.h"

#pragma  comment(lib,"Ws2_32.lib")	//라이브러리 추가

#define  PG_CHATMSG 1
#define  PG_KILLUSER 2

typedef struct _THREADPARM
{
	LINKEDLIST * pLinkedList;
	LINKEDLIST * pMemberLIst;
	LINKEDLIST * pVideoList;
	LINKEDLIST * pRenTalList;
	SOCKET Socket;
}THREADPARAM;

typedef struct _PACKET
{
	int iTotalSize;
	int iPacketGubun;
}PACKET;

void SendMessage(SOCKET ClientSocket, const char* pMsg, int iMsgLen)
{
	send(ClientSocket, pMsg, iMsgLen, NULL);//send(소켓,데이터,데이터길이,플레그)
}

void SendPacket(SOCKET ClientSocket, int iPacketGubun, const char* pMsg, int iMsgLen)
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
		SendPacket(ClientSocket, PG_KILLUSER,"", 0);
		return true;
	}
	return false;
}
unsigned CALLBACK RecvLoginFunc(void* pParam)
{
	THREADPARAM *p = (THREADPARAM*)pParam;

	SOCKET ClientSocket = p->Socket;
	LINKEDLIST* pList = p->pLinkedList;
	LINKEDLIST* pMemberList = p->pMemberLIst;
	char RecvBuffer[4096*2];
	int iCurrent = 0;
	while (1){
		char temp[4096];
		int iRecvLen = recv(ClientSocket, RecvBuffer + iCurrent, 4096, NULL);
		if (iRecvLen <= 0)
		{
			DeleteClient(pList, ClientSocket);
			break;
		}
		iCurrent += iRecvLen;
		while (GetPacket(RecvBuffer, &iCurrent, temp))
		{
			PACKET* pPacket = (PACKET*)temp;
			char (*pRecvID)[128] = (char(*)[128])(temp + sizeof(PACKET));	// client가보내준 아이디
			//temp는 char 배열의 시작주소이다.+ packet(index)의 크기=>128byte만 받기 위해서 배열로 포인터로 바꿔준다
			char(*pRecvPW)[128] = (char(*)[128])(temp + sizeof(PACKET)+128); //client 보낸 비밀번호
			//id(128Byte) 이후의 데이터pw
			NODE * pFindFlag = SSearchName(pMemberList, temp);	//memberlist에서 찾는다.
			if (pFindFlag == NULL){
				DeleteBottom(pList);
				SendPacket(ClientSocket, PG_KILLUSER, "", 0);
				break;
			}
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
		pRecvParam->Socket = ClientSocket;
		_beginthreadex(NULL, NULL, RecvLoginFunc, (void*)pRecvParam, NULL, NULL);
		const char* pMsg = "접속을 환영합니다.~";
		SendPacket(ClientSocket, PG_CHATMSG, pMsg, strlen(pMsg) + 1);
	}
	return 0;
}

void main()
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
	AcceptParam.Socket = ServerSocket;
	_beginthreadex(NULL, NULL, AcceptFunc, (void*)&AcceptParam, NULL, NULL);
	int iChoice = 1;
	while (iChoice != 0)
	{
		printf("1. 접속자 관리 \n");
		printf("2. 전체 메시지 전송 \n");
		scanf_s("%d", &iChoice);
		fflush(stdin);
		switch (iChoice)
		{
		case 1:
			//ManageConnectedUser(&list);
			break;
		case 2:
		//	SendMessageToAll(&list);
			break;
		default:
			break;
		}
	}
	closesocket(ServerSocket);
	WSACleanup();
}