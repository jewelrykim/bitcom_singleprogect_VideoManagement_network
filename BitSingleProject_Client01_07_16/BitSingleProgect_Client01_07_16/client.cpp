
# define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>

#pragma  comment(lib,"Ws2_32.lib")	//���̺귯�� �߰�

#define  PG_LOGIN 1
#define  PG_SIGNUP 2

#define  PG_VIDEOSEARCH 3
#define  PG_VIDEORENTAL 4
#define  PG_VIDEOTABLE 5
#define  PG_VIDEOTABLEEND 8

#define  PG_VIDEORENTALPOSIBLE 6
#define  PG_VIDEORENTALRESERVATION 7

#define  PG_FAIL 11
#define  PG_VIDEOSEARCHFAIL 12

HANDLE hContEvent;

typedef struct _PACKET
{
	int iTotalSize;
	int iPacketGubun;
}PACKET;

typedef struct _MYDATA
{
	char* MyID;
	char* MyPassWord;
	char* MyName;
	int MyAge;
	char* MyPhoneNumber;
}MYDATA;
typedef struct _VIDEO
{
	int iVNumber;		//������ȣ
	char * pVName;		//�����̸�
	int iGrade;			//�������
	//char * pIntroduce;	//�����Ұ�
	bool bRent;			//ture �뿩���� false �뿩��
	bool bReservation;	//ture ���డ�� false ��������
}VIDEO;

typedef struct _THREADPARAM
{
	MYDATA * pMydata;
	VIDEO * pVideo;
	SOCKET Socket;

}THREADPARAM;

void InitMyData(MYDATA * pMydata)
{
	pMydata->MyID = NULL;
	pMydata->MyPassWord = NULL;
	pMydata->MyName = NULL;
	pMydata->MyAge = 0;
	pMydata->MyPhoneNumber = NULL;
}

void InitVideo(VIDEO * pVideo)
{
	pVideo->iVNumber = 0;
	pVideo->pVName = NULL;
	pVideo->iGrade = 0;
	//pVideo->pIntroduce = NULL;
	pVideo->bRent = false;
	pVideo->bReservation = false;
}
void gotoxy(int x, int y)
{
	COORD cur;
	cur.X = x;
	cur.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cur);
}

bool GetPacket(char * RecvBuffer, int * piCurrent, char* temp)
{

	if (*piCurrent < sizeof(PACKET))	//iCurrent �� ��Ŷ����� ��Ÿ�� ũ�⺸�� ������
	{
		return false;
	}

	PACKET* pPacket = (PACKET*)RecvBuffer;	//����ȯ�ؼ� 

	if (*piCurrent < pPacket->iTotalSize)		//���ݱ��� ���� ���� ��Ŷũ�⺸�� ������
	{
		return false;				//�����͸� �߰������� �ٽ� �޴´�
	}
	//�� �ΰ��� if �� ������ �������� �����͸� �� �� �޾ƿ°�

	memcpy(temp, RecvBuffer, pPacket->iTotalSize);		//��Ŷ�� ũ�⸸ŭ temp�� �������־���.

	*piCurrent -= pPacket->iTotalSize; //icurrent�� �ִ� ������ ũ���� ��Ŷ�� �����ְ� ���� ��Ŷ�� �����͸� icurrent�� ó������ �Ű��ش�.
	memcpy(RecvBuffer, RecvBuffer + pPacket->iTotalSize, *piCurrent);		//icurrent�� ������ �������ذ�
	return true;
}
void SendPacket(SOCKET RemoteSocket, int iPacketGubun, const char* pMsg, int iMsgLen)
{
	PACKET packet;
	memset(&packet, 0, sizeof(PACKET));
	packet.iPacketGubun = iPacketGubun;
	packet.iTotalSize = sizeof(PACKET)+iMsgLen;
	char temp[1024];
	memcpy(temp, &packet, sizeof(PACKET));
	memcpy(temp + sizeof(PACKET), pMsg, iMsgLen);
	send(RemoteSocket, temp, packet.iTotalSize, NULL);		//packet.iTotalSize ����ڰ� �Է��� ��ŭ�� ������.
}
void SendLoginData(SOCKET RemoteSocket)
{
	char Login[256];		//Login ��Ŷ�Ǿ ���� ����
	char Idtemp[128];		//������� ���̵�
	char Password[128];		//������� �н�����
	printf("ID : ");
	gets_s(Idtemp, 128);
	fflush(stdin);
	printf("\nPassWord : ");
	gets_s(Password, 128);
	fflush(stdin);
	memcpy(Login, Idtemp, 128);
	memcpy(Login + 128, Password, 128);
	SendPacket(RemoteSocket, PG_LOGIN, Login, 256);
}

void SendSignUpData(SOCKET RemoteSocket)
{
	char SignUp[516];
	char MemberID[128];
	char MemberPassWord[128];
	char MemberName[128];
	int MemberAge;
	char MemberPhonNumber[128];
	printf("ID : ");
	gets_s(MemberID, 128);
	fflush(stdin);
	printf("\nPassWord : ");
	gets_s(MemberPassWord, 128);
	fflush(stdin);
	printf("\n�̸� : ");
	gets_s(MemberName, 128);
	fflush(stdin);
	printf("\n ���� : ");
	scanf_s("%d", &MemberAge);
	fflush(stdin);
	printf("\n��ȭ��ȣ : ");
	gets_s(MemberPhonNumber, 128);
	fflush(stdin);

	memcpy(SignUp, MemberID, 128);
	memcpy(SignUp + 128, MemberPassWord, 128);
	memcpy(SignUp + 256, MemberName, 128);
	memcpy(SignUp + 384, &MemberAge, 4);
	memcpy(SignUp + 388, MemberPhonNumber, 128);
	printf("\n");
	SendPacket(RemoteSocket, PG_SIGNUP, SignUp, 516);

}

unsigned CALLBACK RecvFunc(void* pParam)
{
	THREADPARAM *p = (THREADPARAM *)pParam;
	SOCKET RemoteSocket = p->Socket;
	MYDATA * pMydata = p->pMydata;
	VIDEO* pVideo = p->pVideo;
	bool bCont = true;

	char RecvBuffer[2048]; // �����͸� ���� ���� [�ִ� �ι�� �����͸� ���� ���� �����ϱ�]�ι�ѱ����� �չ��۰� ó���ȴ�.
	int iCurrent = 0;
	while (bCont){
		char temp[1024];
		char SendData[1024];
		int iRecvLen = recv(RemoteSocket, RecvBuffer + iCurrent, 1024, NULL);				//client�� �ް� �ٽ� ������ ������ �޴´�. //iRecvLen = recv���� ������ ����Ʈ��
		int count = 0;
		if (iRecvLen <= 0)
		{
			printf("���� ����\n");
			break;
		}
		iCurrent += iRecvLen;
		while (GetPacket(RecvBuffer, &iCurrent, temp))
		{

			PACKET * pPacket = (PACKET*)temp;		//ó������ ��Ŷ
			char * pMsg = (char*)(temp + sizeof(PACKET));

			switch (pPacket->iPacketGubun)
			{

			case PG_FAIL:
				printf("%s\n", pMsg);
				SetEvent(hContEvent);	//�������� ��ٸ��� �ϴ� �Լ�
				break;
			case PG_LOGIN:				
				pMydata->MyID = (char*)malloc(128);					//login����(login+signup) ������� ������ �������� �޾Ƽ� Ŭ���̾�Ʈ �����Ϳ� ����
				strcpy_s(pMydata->MyID, 128, (char*)(temp + sizeof(PACKET)));

				pMydata->MyPassWord = (char*)malloc(128);
				strcpy_s(pMydata->MyPassWord, 128, (char*)(temp + sizeof(PACKET)+128));

				pMydata->MyName = (char*)malloc(128);
				strcpy_s(pMydata->MyName, 128, (char*)(temp + sizeof(PACKET)+256));
					
				memcpy(&(pMydata->MyAge),temp + sizeof(PACKET)+384,4);

				pMydata->MyPhoneNumber = (char*)malloc(128);
				strcpy_s(pMydata->MyPhoneNumber, 128, (char*)(temp + sizeof(PACKET)+388));
				
				SetEvent(hContEvent);	//�������� ��ٸ��� �ϴ� �Լ�
				break;
			case PG_VIDEOSEARCH:
				
				memcpy(&(pVideo->iVNumber), temp + sizeof(PACKET), 4);
				pVideo->pVName = (char*)malloc(128);
				strcpy_s(pVideo->pVName, 128, (char *)(temp + sizeof(PACKET)+4));

				memcpy(&(pVideo->iGrade), temp + sizeof(PACKET)+132, 4);
				memcpy(&(pVideo->bRent), temp + sizeof(PACKET)+136, 4);
				memcpy(&(pVideo->bReservation), temp + sizeof(PACKET)+140, 4);

				printf("%d\n", pVideo->iVNumber);
				SetEvent(hContEvent);
				break;
			case PG_VIDEOSEARCHFAIL:
				printf("���� ã�� ����\n");
				SetEvent(hContEvent);
				break;
			case PG_VIDEOTABLE:
				printf("���� ��ȣ : %d\n", (int)*(temp + sizeof(PACKET)));
				printf("���� �̸� : %s\n", (char*)(temp + sizeof(PACKET)+4));
				printf("���� ��� : %d\n", (int)*(temp + sizeof(PACKET)+132));
				printf("���� �뿩 ���� ���� : ");
				if ((bool)*(temp + sizeof(PACKET)+136) == true)
				{
					printf("���� \n");
				}
				else
				{
					printf("�뿩�� \n");
				}
				printf("���� ���� ���� ���� : ");
				if ((bool)*(temp + sizeof(PACKET)+140) == true)
				{
					printf("���� \n");
				}
				else
				{
					printf("�Ұ� \n");
				}
				break;
			case PG_VIDEOTABLEEND:
				SetEvent(hContEvent);
				break;
			case PG_VIDEORENTAL:
				memcpy(&(pVideo->iVNumber), temp + sizeof(PACKET), 4);
				pVideo->pVName = (char*)malloc(128);
				strcpy_s(pVideo->pVName, 128, (char *)(temp + sizeof(PACKET)+4));

				memcpy(&(pVideo->iGrade), temp + sizeof(PACKET)+132, 4);
				memcpy(&(pVideo->bRent), temp + sizeof(PACKET)+136, 4);
				memcpy(&(pVideo->bReservation), temp + sizeof(PACKET)+140, 4);
				if (pMydata->MyAge < pVideo->iGrade)
				{
					printf("%d �� ���� �����Ұ� ���� �Դϴ�.\n",pVideo->iGrade);
					SetEvent(hContEvent);
					break;
				}
				else if (pVideo->bRent == false)		//false �뿩��
				{
					printf("�̹� �뿩���� ���� �Դϴ�.\n");
					if (pVideo->bReservation == false)	//false ����
					{
						printf("�̹� �������� ���� �Դϴ�.\n");
						SetEvent(hContEvent);
						break;
					}
					else{
						printf("���� ���� ���� �Դϴ�.\n");
						memcpy(SendData, pMydata->MyName, 128);
						memcpy(SendData +128, pVideo->pVName, 128);
						SendPacket(RemoteSocket,PG_VIDEORENTALRESERVATION,SendData,256);
						SetEvent(hContEvent);
						break;
					}
					break;
				}
				else{
					printf("�뿩������ ���� �Դϴ�.\n");
					memcpy(SendData, pMydata->MyName, 128);
					memcpy(SendData +128, pVideo->pVName, 128);
					SendPacket(RemoteSocket, PG_VIDEORENTALPOSIBLE, SendData, 256);
					break;

				}
				//memcpy(temp + sizeof())
			case PG_VIDEORENTALPOSIBLE:
				printf("%s\n", pMsg);
				SetEvent(hContEvent);
				break;
			default:
				break;
			}
		}
	}
	shutdown(RemoteSocket, SD_BOTH);
	closesocket(RemoteSocket);
	return 0;
}



void main()
{

	hContEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	WSADATA data;

	MYDATA Mydata;
	VIDEO Video;
	InitMyData(&Mydata);
	InitVideo(&Video);
	memset(&data, 0, sizeof(data));
	WSAStartup(MAKEWORD(2, 2), &data);

	SOCKET RemoteSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (RemoteSocket == INVALID_SOCKET)
	{
		printf("REMOTESOCKET ��������");
		return;
	}

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = inet_addr("165.132.254.201");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(50000);
	int iError = connect(RemoteSocket, (sockaddr*)&addr, sizeof(addr));
	if (iError == SOCKET_ERROR)
	{
		printf("connect Error\n");
		return;
	}

	//send �� �ؾߵǰ� recv�� ��� �ؾ��Ѵ�.=> thread�� �����
	THREADPARAM RecvParam;
	RecvParam.pMydata = &Mydata;
	RecvParam.pVideo = &Video;
	RecvParam.Socket = RemoteSocket;
	_beginthreadex(NULL, NULL, RecvFunc, (void*)&RecvParam, NULL, NULL);
	int iLoginMenu = 0;

	
	while (1)
	{
		iLoginMenu = 0;
		printf("1.�α��� 2.ȸ������ 3.������\n");
		scanf_s("%d", &iLoginMenu);
		fflush(stdin);
		switch (iLoginMenu)
		{
		case 1:
			SendLoginData(RemoteSocket);
			WaitForSingleObject(hContEvent, INFINITE);
			break;
		case 2:
			SendSignUpData(RemoteSocket);
			WaitForSingleObject(hContEvent, INFINITE);
			break;
		default:
			break;
		}	
		if (Mydata.MyID != NULL)
			break;
	}
	while (Mydata.MyID != NULL){
		int ClientSelectMode = 0;
		int MyPageMode = 0;
		while (ClientSelectMode != -1){
			printf("1.�����˻� 2.���� ��� 3.���� �뿩 4.���������� \n");
			char SearchVideoName[128] = { NULL };
			scanf_s("%d", &ClientSelectMode);
			fflush(stdin);
			switch (ClientSelectMode)
			{	
			case 1:
				printf("ã�� ���� �̸� : ");	
				gets_s(SearchVideoName, 128);
				fflush(stdin);
				SendPacket(RemoteSocket, PG_VIDEOSEARCH, SearchVideoName, 128);
				WaitForSingleObject(hContEvent, INFINITE);

				break;
			case 2:
				printf("���� ��� \n");
				SendPacket(RemoteSocket, PG_VIDEOTABLE, "", 0);
				WaitForSingleObject(hContEvent, INFINITE);
				break;
			case 3:
				printf("�뿩�� ���� �̸� : ");
				gets_s(SearchVideoName, 128);
				fflush(stdin);
				SendPacket(RemoteSocket, PG_VIDEORENTAL, SearchVideoName, 128);
				WaitForSingleObject(hContEvent, INFINITE);

				break;
			case 4:
				printf("����������\n");

				switch (MyPageMode)
				{
				case 1:

					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			break;
		}
	}
	/*while (1)
	{
		char temp[1024];
		gets_s(temp, 1024);
		if (temp[0] == NULL)		//����ġ�� ��
		{
			break;
		}
		SendPacket(RemoteSocket, PG_CHATMSG, temp, strlen(temp) + 1);
	}*/

	closesocket(RemoteSocket);
	WSACleanup();

}