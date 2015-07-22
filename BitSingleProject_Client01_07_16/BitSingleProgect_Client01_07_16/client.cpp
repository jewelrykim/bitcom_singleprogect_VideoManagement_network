
# define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>

#pragma  comment(lib,"Ws2_32.lib")	//라이브러리 추가

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
	int iVNumber;		//비디오번호
	char * pVName;		//비디오이름
	int iGrade;			//비디오등급
	//char * pIntroduce;	//비디오소개
	bool bRent;			//ture 대여가능 false 대여중
	bool bReservation;	//ture 예약가능 false 예약있음
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

	if (*piCurrent < sizeof(PACKET))	//iCurrent 가 페킷사이즈를 나타넨 크기보다 작으면
	{
		return false;
	}

	PACKET* pPacket = (PACKET*)RecvBuffer;	//형변환해서 

	if (*piCurrent < pPacket->iTotalSize)		//지금까지 받은 값이 패킷크기보다 작으면
	{
		return false;				//데이터를 추가적으로 다시 받는다
	}
	//위 두가지 if 를 무사히 지나오면 데이터를 다 잘 받아온것

	memcpy(temp, RecvBuffer, pPacket->iTotalSize);		//패킷의 크기만큼 temp에 복사해주었다.

	*piCurrent -= pPacket->iTotalSize; //icurrent에 있던 완전한 크기의 패킷은 지워주고 다음 패킷의 데이터를 icurrent의 처음으로 옮겨준다.
	memcpy(RecvBuffer, RecvBuffer + pPacket->iTotalSize, *piCurrent);		//icurrent의 앞으로 복사해준것
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
	send(RemoteSocket, temp, packet.iTotalSize, NULL);		//packet.iTotalSize 사용자가 입력한 만큼만 보낸다.
}
void SendLoginData(SOCKET RemoteSocket)
{
	char Login[256];		//Login 패킷실어서 보낼 내용
	char Idtemp[128];		//사용자의 아이디
	char Password[128];		//사용자의 패스워드
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
	printf("\n이름 : ");
	gets_s(MemberName, 128);
	fflush(stdin);
	printf("\n 나이 : ");
	scanf_s("%d", &MemberAge);
	fflush(stdin);
	printf("\n전화번호 : ");
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

	char RecvBuffer[2048]; // 데이터를 받을 버퍼 [최대 두배로 데이터를 받을 수도 있으니까]두배넘기전에 앞버퍼가 처리된다.
	int iCurrent = 0;
	while (bCont){
		char temp[1024];
		char SendData[1024];
		int iRecvLen = recv(RemoteSocket, RecvBuffer + iCurrent, 1024, NULL);				//client가 받고 다시 보내는 정보를 받는다. //iRecvLen = recv받은 정보의 바이트수
		int count = 0;
		if (iRecvLen <= 0)
		{
			printf("서버 종료\n");
			break;
		}
		iCurrent += iRecvLen;
		while (GetPacket(RecvBuffer, &iCurrent, temp))
		{

			PACKET * pPacket = (PACKET*)temp;		//처음에는 패킷
			char * pMsg = (char*)(temp + sizeof(PACKET));

			switch (pPacket->iPacketGubun)
			{

			case PG_FAIL:
				printf("%s\n", pMsg);
				SetEvent(hContEvent);	//서버에서 기다리게 하는 함수
				break;
			case PG_LOGIN:				
				pMydata->MyID = (char*)malloc(128);					//login된후(login+signup) 사용자의 정보를 서버에서 받아서 클라이언트 데이터에 저장
				strcpy_s(pMydata->MyID, 128, (char*)(temp + sizeof(PACKET)));

				pMydata->MyPassWord = (char*)malloc(128);
				strcpy_s(pMydata->MyPassWord, 128, (char*)(temp + sizeof(PACKET)+128));

				pMydata->MyName = (char*)malloc(128);
				strcpy_s(pMydata->MyName, 128, (char*)(temp + sizeof(PACKET)+256));
					
				memcpy(&(pMydata->MyAge),temp + sizeof(PACKET)+384,4);

				pMydata->MyPhoneNumber = (char*)malloc(128);
				strcpy_s(pMydata->MyPhoneNumber, 128, (char*)(temp + sizeof(PACKET)+388));
				
				SetEvent(hContEvent);	//서버에서 기다리게 하는 함수
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
				printf("비디오 찾기 실패\n");
				SetEvent(hContEvent);
				break;
			case PG_VIDEOTABLE:
				printf("비디오 번호 : %d\n", (int)*(temp + sizeof(PACKET)));
				printf("비디오 이름 : %s\n", (char*)(temp + sizeof(PACKET)+4));
				printf("비디오 등급 : %d\n", (int)*(temp + sizeof(PACKET)+132));
				printf("비디오 대여 가능 여부 : ");
				if ((bool)*(temp + sizeof(PACKET)+136) == true)
				{
					printf("가능 \n");
				}
				else
				{
					printf("대여중 \n");
				}
				printf("비디오 예약 가능 여부 : ");
				if ((bool)*(temp + sizeof(PACKET)+140) == true)
				{
					printf("가능 \n");
				}
				else
				{
					printf("불가 \n");
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
					printf("%d 세 이하 관람불가 비디오 입니다.\n",pVideo->iGrade);
					SetEvent(hContEvent);
					break;
				}
				else if (pVideo->bRent == false)		//false 대여중
				{
					printf("이미 대여중인 비디오 입니다.\n");
					if (pVideo->bReservation == false)	//false 예약
					{
						printf("이미 예약중인 비디오 입니다.\n");
						SetEvent(hContEvent);
						break;
					}
					else{
						printf("예약 가능 비디오 입니다.\n");
						memcpy(SendData, pMydata->MyName, 128);
						memcpy(SendData +128, pVideo->pVName, 128);
						SendPacket(RemoteSocket,PG_VIDEORENTALRESERVATION,SendData,256);
						SetEvent(hContEvent);
						break;
					}
					break;
				}
				else{
					printf("대여가능한 비디오 입니다.\n");
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
		printf("REMOTESOCKET 생성실패");
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

	//send 도 해야되고 recv도 계속 해야한다.=> thread를 만든다
	THREADPARAM RecvParam;
	RecvParam.pMydata = &Mydata;
	RecvParam.pVideo = &Video;
	RecvParam.Socket = RemoteSocket;
	_beginthreadex(NULL, NULL, RecvFunc, (void*)&RecvParam, NULL, NULL);
	int iLoginMenu = 0;

	
	while (1)
	{
		iLoginMenu = 0;
		printf("1.로그인 2.회원가입 3.나가기\n");
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
			printf("1.비디오검색 2.비디오 목록 3.비디오 대여 4.마이페이지 \n");
			char SearchVideoName[128] = { NULL };
			scanf_s("%d", &ClientSelectMode);
			fflush(stdin);
			switch (ClientSelectMode)
			{	
			case 1:
				printf("찾을 비디오 이름 : ");	
				gets_s(SearchVideoName, 128);
				fflush(stdin);
				SendPacket(RemoteSocket, PG_VIDEOSEARCH, SearchVideoName, 128);
				WaitForSingleObject(hContEvent, INFINITE);

				break;
			case 2:
				printf("비디오 목록 \n");
				SendPacket(RemoteSocket, PG_VIDEOTABLE, "", 0);
				WaitForSingleObject(hContEvent, INFINITE);
				break;
			case 3:
				printf("대여할 비디오 이름 : ");
				gets_s(SearchVideoName, 128);
				fflush(stdin);
				SendPacket(RemoteSocket, PG_VIDEORENTAL, SearchVideoName, 128);
				WaitForSingleObject(hContEvent, INFINITE);

				break;
			case 4:
				printf("마이페이지\n");

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
		if (temp[0] == NULL)		//엔터치면 끝
		{
			break;
		}
		SendPacket(RemoteSocket, PG_CHATMSG, temp, strlen(temp) + 1);
	}*/

	closesocket(RemoteSocket);
	WSACleanup();

}