
# define _WINSOCK_DEPRECATED_NO_WARNING
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
	int iClientClose;			//Ŭ���̾�Ʈ or ������ ���Ḧ Ȯ���ϱ� ���� iClientClose ==0 ��������
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
void Sendint(SOCKET RemoteSocket, int iPacketGubun, int data)
{
	PACKET packet;
	memset(&packet, 0, sizeof(PACKET));
	packet.iPacketGubun = iPacketGubun;
	packet.iTotalSize = sizeof(PACKET)+4;
	char temp[1024];
	memcpy(temp, &packet, sizeof(PACKET));
	memcpy(temp + sizeof(PACKET),&data, 4);
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
	printf("PassWord : ");
	gets_s(Password, 128);
	fflush(stdin);
	memcpy(Login, Idtemp, 128);
	memcpy(Login + 128, Password, 128);
	SendPacket(RemoteSocket, PG_LOGIN, Login, 256);
}

int SendSignUpData(SOCKET RemoteSocket)
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
	if (MemberAge<0 || MemberAge>150)
	{
		printf("���� ���� �ùٸ��� �ʽ��ϴ�.\n");
		return 1;
	}
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
	return 0;
}

unsigned CALLBACK RecvFunc(void* pParam)
{
	THREADPARAM *p = (THREADPARAM *)pParam;
	SOCKET RemoteSocket = p->Socket;
	MYDATA * pMydata = p->pMydata;
	VIDEO* pVideo = p->pVideo;
	bool bCont = true;
	bool bRent = true;
	bool bReser = true;

	char RecvBuffer[2048]; // �����͸� ���� ���� [�ִ� �ι�� �����͸� ���� ���� �����ϱ�]�ι�ѱ����� �չ��۰� ó���ȴ�.
	int iCurrent = 0;
	while (bCont){
		char temp[1024];
		char SendData[1024];
		int iRecvLen = recv(RemoteSocket, RecvBuffer + iCurrent, 1024, NULL);				//client�� �ް� �ٽ� ������ ������ �޴´�. //iRecvLen = recv���� ������ ����Ʈ��
		int count = 0;
		if (iRecvLen <= 0)
		{
			if (p->iClientClose == 0)
			{
				printf("���� ����\n");
			}
			else
			{
				printf("Ŭ���̾�Ʈ ����\n");
			}
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
				printf("\n");
				SetEvent(hContEvent);	
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
				printf("\n");
				SetEvent(hContEvent);	//�����尡 ��ٸ��°��� ������ �Լ�
				break;
			case PG_VIDEOSEARCH:
				
				printf("���� ��ȣ : %d\n", (int)*(temp + sizeof(PACKET)));
				printf("���� �̸� : %s\n", (char*)(temp + sizeof(PACKET)+4));
				printf("���� ��� : %d\n", (int)*(temp + sizeof(PACKET)+132));
				printf("���� �뿩 ���� ���� : ");
				memcpy(&bRent, temp + sizeof(PACKET)+136, 4);
				memcpy(&bReser, temp + sizeof(PACKET)+140, 4);
				if (bRent == true)
				{
					printf("���� \n");
				}
				else
				{
					printf("�뿩�� \n");
				}
				printf("���� ���� ���� ���� : ");
				if (bReser == true)
				{
					printf("���� \n");
				}
				else
				{
					printf("�Ұ� \n");
				}
				printf("\n");
				SetEvent(hContEvent);
				break;
			case PG_VIDEOSEARCHFAIL:
				printf("���� ã�� ����\n");
				printf("\n");
				SetEvent(hContEvent);
				break;
			case PG_VIDEOTABLE:
				printf("���� ��ȣ : %d\n", (int)*(temp + sizeof(PACKET)));
				printf("���� �̸� : %s\n", (char*)(temp + sizeof(PACKET)+4));
				printf("���� ��� : %d\n", (int)*(temp + sizeof(PACKET)+132));
				printf("���� �뿩 ���� ���� : ");
				memcpy(&bRent, temp + sizeof(PACKET)+136, 4);
				memcpy(&bReser, temp + sizeof(PACKET)+140, 4);
				if (bRent == true)
				{
					printf("���� \n");
				}
				else
				{
					printf("�뿩�� \n");
				}
				printf("���� ���� ���� ���� : ");
				if (bReser == true)
				{
					printf("���� \n");
				}
				else
				{
					printf("�Ұ� \n");
				}
				printf("\n");
				break;
			case PG_VIDEOTABLEEND:
				printf("%s\n", (char*)(temp + sizeof(PACKET)));
				printf("\n");
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
					printf("\n");
					SetEvent(hContEvent);
					break;
				}
				else if (pVideo->bRent == false)		//false �뿩��
				{
					printf("�̹� �뿩���� ���� �Դϴ�.\n");
					if (pVideo->bReservation == false)	//false ����
					{
						printf("�̹� �������� ���� �Դϴ�.\n");
						printf("\n");
						SetEvent(hContEvent);
						break;
					}
					else{
						printf("���� ���� ���� �Դϴ�.\n");
						memcpy(SendData, pMydata->MyName, 128);
						memcpy(SendData +128, pVideo->pVName, 128);
						memcpy(SendData + 256, &pVideo->iVNumber, 4);
						SendPacket(RemoteSocket,PG_VIDEORENTALRESERVATION,SendData,256);
						printf("\n");
						SetEvent(hContEvent);
						break;
					}
					break;
				}
				else{
					printf("�뿩������ ���� �Դϴ�.\n");
					memcpy(SendData, pMydata->MyID, 128);
					memcpy(SendData + 128, pVideo->pVName, 128);
					memcpy(SendData + 256, &pVideo->iVNumber, 4);
					SendPacket(RemoteSocket, PG_VIDEORENTALPOSIBLE, SendData, 260);
					printf("\n");
					break;
				}
			case PG_VIDEORENTALPOSIBLE:
				printf("%s\n", pMsg);
				printf("\n");
				SetEvent(hContEvent);
				break;

			case PG_RENTALLIST:
				printf("���� ��ȣ : %d\n", (int)*(temp + sizeof(PACKET)));
				printf("���� �̸� : %s\n", (char*)(temp + sizeof(PACKET)+4));
				printf("\n");
				break;

			case PG_RENTALLISTEND:
				printf("%s\n", (char*)(temp + sizeof(PACKET)));
				printf("\n");
				SetEvent(hContEvent);
				break;
			case PG_WITHDREW:
				pMydata->MyID = NULL;
				SetEvent(hContEvent);
				break;
			case PG_WITHDREWFAIL:	
				printf("%s\n",(char*)(temp + sizeof(PACKET)));
				SetEvent(hContEvent);
				break;
			case PG_RETURNVIDEO:
				printf("%s\n", (char*)(temp + sizeof(PACKET)));
				SetEvent(hContEvent);
				break;
			case PG_NOVIDEO:
				printf("%s\n", (char*)(temp + sizeof(PACKET)));
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
		addr.sin_addr.s_addr = inet_addr("165.132.221.149");
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
		RecvParam.iClientClose = 0;
		_beginthreadex(NULL, NULL, RecvFunc, (void*)&RecvParam, NULL, NULL);
		int iLoginMenu = 0;
		while (iLoginMenu != 3){
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
					if (SendSignUpData(RemoteSocket) == 0)
					{
						WaitForSingleObject(hContEvent, INFINITE);
					}
					break;
				case 3:
					break;
				default:
					break;
				}
				if (Mydata.MyID != NULL)
					break;
				else if (iLoginMenu == 3)
				{
					break;
				}
			}
			while (Mydata.MyID != NULL)
			{		//���θ޴� ����
				int ClientSelectMode = 0;
				int MyPageMode = 0;
				int ModtfyMydataMode = 0;
				int ReturnVideo = 0;
				printf("1.�����˻� 2.���� ��� 3.���� �뿩 4.���� �ݳ� 5.���������� 6.�α׾ƿ� \n");
				char SearchVideoName[128] = { NULL };
				char ModifyMyData[516] = { NULL };
				char Modifytemp[128] = { NULL };
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
					printf("�ݳ��� ���� ��ȣ : ");
					scanf_s("%d", &ReturnVideo);
					fflush(stdin);
					Sendint(RemoteSocket, PG_RETURNVIDEO, ReturnVideo);
					WaitForSingleObject(hContEvent, INFINITE);
					break;
				case 5:
					printf("����������\n");
					printf("1. �� ���� ���� 2.���� ���� ���� ���� 3. ���� ���� 4. Ż��\n");
					scanf_s("%d", &MyPageMode);
					fflush(stdin);
					switch (MyPageMode)		//���������� ����
					{
					case 1:
						printf("ID	: %s\n", Mydata.MyID);
						printf("�̸�	: %s\n", Mydata.MyName);
						printf("����	: %d\n", Mydata.MyAge);
						printf("��ȭ��ȣ: %s\n", Mydata.MyPhoneNumber);
						break;
					case 2:
						SendPacket(RemoteSocket, PG_RENTALLIST, Mydata.MyID, 128);
						WaitForSingleObject(hContEvent, INFINITE);
						break;
					case 3:			
						printf("�� ���� ����\n");
						printf("1. �̸� ���� 2. ���� ���� 3. ��ȭ��ȣ ���� 4. ��й�ȣ ����\n");
						scanf_s("%d", &ModtfyMydataMode);
						fflush(stdin);
						switch (ModtfyMydataMode)
						{
						case 1:
							printf("������ �̸� :\n");
							gets_s(Modifytemp, 128);
							fflush(stdin);
							memcpy(Mydata.MyName, Modifytemp, 128);		
							break;
						case 2:
							printf("������ ���� :\n");
							scanf_s("%d", &Mydata.MyAge);
							fflush(stdin);
							break;
						case 3:
							printf("������ ��ȭ��ȣ :\n");
							gets_s(Modifytemp, 128);
							fflush(stdin);
							memcpy(Mydata.MyPhoneNumber, Modifytemp, 128);
							break;
						default:
							break;
						}
						if (ModtfyMydataMode == 1 || ModtfyMydataMode == 2 || ModtfyMydataMode == 3)
						{
							memcpy(ModifyMyData, Mydata.MyID, 128);
							memcpy(ModifyMyData + 128, Mydata.MyPassWord, 128);
							memcpy(ModifyMyData + 256, Mydata.MyName, 128);
							memcpy(ModifyMyData + 384, &Mydata.MyAge, 4);
							memcpy(ModifyMyData + 388, Mydata.MyPhoneNumber, 128);
						}
						break;
					case 4:
						SendPacket(RemoteSocket, PG_WITHDREW, Mydata.MyID, 128);
						WaitForSingleObject(hContEvent, INFINITE);
						break;
					default:
						break;
					}			//���������� ���� ��
					break;
				case 6:
					SendPacket(RemoteSocket, PG_LOGOUT, Mydata.MyID, 128);	
					Mydata.MyID = NULL;
				default:
					break;
				}
			}//���θ޴� ������
			
		}	
		RecvParam.iClientClose = 1;
		closesocket(RemoteSocket);
		WSACleanup();
}