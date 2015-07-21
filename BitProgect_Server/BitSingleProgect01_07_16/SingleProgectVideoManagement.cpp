# define _WINSOCK_DEPRECATED_NO_WARNING
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>
#include <windows.h>
#include "VideoManagement.h"

#pragma  comment(lib,"Ws2_32.lib")	//���̺귯�� �߰�

#define  PG_CHATMSG 1
#define  PG_KILLUSER 2
#define  PG_LOGIN 3
#define  PG_SIGNUP 4

typedef struct _THREADPARM
{
	LINKEDLIST * pLinkedList;
	LINKEDLIST * pMemberLIst;
	LINKEDLIST * pVideoList;
	LINKEDLIST * pRenTalList;
	SOCKET Socket;
}THREADPARAM;

void SendMessage(SOCKET ClientSocket, const char* pMsg, int iMsgLen)
{
	send(ClientSocket, pMsg, iMsgLen, NULL);//send(����,������,�����ͱ���,�÷���)
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
		SendPacket(ClientSocket, PG_KILLUSER,"", 0);
		return true;
	}
	return false;
}
unsigned CALLBACK RecvLoginFunc(void* pParam)
{
	THREADPARAM *p = (THREADPARAM*)pParam;
	LINKEDLIST *pList = p->pLinkedList;
	LINKEDLIST *pMemberList = p->pMemberLIst;
	LINKEDLIST *pVideo = p->pVideoList;
	LINKEDLIST *pRent = p->pRenTalList;
	SOCKET ClientSocket = p->Socket;
	char RecvBuffer[4096*2];
	int iCurrent = 0;
	while (1){
		char temp[4096];
		int iRecvLen = recv(ClientSocket, RecvBuffer + iCurrent, 4096, NULL);
		if (iRecvLen <= 0)
		{
			DeleteClient(p->pLinkedList, ClientSocket);
			break;
		}
		iCurrent += iRecvLen;

		while (GetPacket(RecvBuffer, &iCurrent, temp))
		{
			PACKET* pPacket = (PACKET*)temp;
			if (pPacket->iPacketGubun == PG_LOGIN){
				char * pID = temp + sizeof(PACKET);
				char * pPW = temp + sizeof(PACKET) + 128;
				//id(128Byte) ������ ������pw
				NODE * pFindFlag = SSearchName(pMemberList, pID);	//memberlist���� ã�´�.
				if (pFindFlag == NULL){
					DeleteBottom(pList);
					SendPacket(ClientSocket, PG_KILLUSER, "", 0);
					break;
				}
				printf("%s", ((MEMBER*)pFindFlag->pData)->pName);
				//SendPacket(ClientSocket, PG_LOGIN, (char*)pFindFlag, strlen(temp) + 1);
			}
			else if (pPacket->iPacketGubun == PG_SIGNUP)
			{

				InputMember(pMemberList,(void*)temp);
				SendPacket(ClientSocket, PG_SIGNUP, temp, strlen(temp) + 1);
			}
			else if (pPacket->iPacketGubun == PG_CHATMSG)
			{
				printf("��� �������̴�\n" );
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
	LINKEDLIST *pMember = p->pMemberLIst;
	LINKEDLIST *pVideo = p->pVideoList;
	LINKEDLIST *pRent = p->pRenTalList;

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
		pRecvParam->pRenTalList = pVideo;
		pRecvParam->pVideoList = pRent;
		pRecvParam->Socket = ClientSocket;
		_beginthreadex(NULL, NULL, RecvLoginFunc, (void*)pRecvParam, NULL, NULL);
		/*const char* pMsg = "������ ȯ���մϴ�.~";
		SendPacket(ClientSocket, PG_CHATMSG, pMsg, strlen(pMsg) + 1);*/
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
		printf("ServerSocket ��������");
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

		printf("1. ������ ���� \n");
		printf("2. ��ü �޽��� ���� \n");
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