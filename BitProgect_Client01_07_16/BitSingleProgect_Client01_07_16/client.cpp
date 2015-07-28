
# define _WINSOCK_DEPRECATED_NO_WARNING
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
	int iClientClose;			//클라이언트 or 서버의 종료를 확인하기 위해 iClientClose ==0 서버종료
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
void Sendint(SOCKET RemoteSocket, int iPacketGubun, int data)
{
	PACKET packet;
	memset(&packet, 0, sizeof(PACKET));
	packet.iPacketGubun = iPacketGubun;
	packet.iTotalSize = sizeof(PACKET)+4;
	char temp[1024];
	memcpy(temp, &packet, sizeof(PACKET));
	memcpy(temp + sizeof(PACKET),&data, 4);
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
	printf("\n이름 : ");
	gets_s(MemberName, 128);
	fflush(stdin);
	printf("\n 나이 : ");
	scanf_s("%d", &MemberAge);
	fflush(stdin);
	if (MemberAge<0 || MemberAge>150)
	{
		printf("나이 값이 올바르지 않습니다.\n");
		return 1;
	}
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

	char RecvBuffer[2048]; // 데이터를 받을 버퍼 [최대 두배로 데이터를 받을 수도 있으니까]두배넘기전에 앞버퍼가 처리된다.
	int iCurrent = 0;
	while (bCont){
		char temp[1024];
		char SendData[1024];
		int iRecvLen = recv(RemoteSocket, RecvBuffer + iCurrent, 1024, NULL);				//client가 받고 다시 보내는 정보를 받는다. //iRecvLen = recv받은 정보의 바이트수
		int count = 0;
		if (iRecvLen <= 0)
		{
			if (p->iClientClose == 0)
			{
				printf("서버 종료\n");
			}
			else
			{
				printf("클라이언트 종료\n");
			}
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
				printf("\n");
				SetEvent(hContEvent);	
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
				printf("\n");
				SetEvent(hContEvent);	//스레드가 기다리는것을 끝내는 함수
				break;
			case PG_VIDEOSEARCH:
				
				printf("비디오 번호 : %d\n", (int)*(temp + sizeof(PACKET)));
				printf("비디오 이름 : %s\n", (char*)(temp + sizeof(PACKET)+4));
				printf("비디오 등급 : %d\n", (int)*(temp + sizeof(PACKET)+132));
				printf("비디오 대여 가능 여부 : ");
				memcpy(&bRent, temp + sizeof(PACKET)+136, 4);
				memcpy(&bReser, temp + sizeof(PACKET)+140, 4);
				if (bRent == true)
				{
					printf("가능 \n");
				}
				else
				{
					printf("대여중 \n");
				}
				printf("비디오 예약 가능 여부 : ");
				if (bReser == true)
				{
					printf("가능 \n");
				}
				else
				{
					printf("불가 \n");
				}
				printf("\n");
				SetEvent(hContEvent);
				break;
			case PG_VIDEOSEARCHFAIL:
				printf("비디오 찾기 실패\n");
				printf("\n");
				SetEvent(hContEvent);
				break;
			case PG_VIDEOTABLE:
				printf("비디오 번호 : %d\n", (int)*(temp + sizeof(PACKET)));
				printf("비디오 이름 : %s\n", (char*)(temp + sizeof(PACKET)+4));
				printf("비디오 등급 : %d\n", (int)*(temp + sizeof(PACKET)+132));
				printf("비디오 대여 가능 여부 : ");
				memcpy(&bRent, temp + sizeof(PACKET)+136, 4);
				memcpy(&bReser, temp + sizeof(PACKET)+140, 4);
				if (bRent == true)
				{
					printf("가능 \n");
				}
				else
				{
					printf("대여중 \n");
				}
				printf("비디오 예약 가능 여부 : ");
				if (bReser == true)
				{
					printf("가능 \n");
				}
				else
				{
					printf("불가 \n");
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
					printf("%d 세 이하 관람불가 비디오 입니다.\n",pVideo->iGrade);
					printf("\n");
					SetEvent(hContEvent);
					break;
				}
				else if (pVideo->bRent == false)		//false 대여중
				{
					printf("이미 대여중인 비디오 입니다.\n");
					if (pVideo->bReservation == false)	//false 예약
					{
						printf("이미 예약중인 비디오 입니다.\n");
						printf("\n");
						SetEvent(hContEvent);
						break;
					}
					else{
						printf("예약 가능 비디오 입니다.\n");
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
					printf("대여가능한 비디오 입니다.\n");
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
				printf("비디오 번호 : %d\n", (int)*(temp + sizeof(PACKET)));
				printf("비디오 이름 : %s\n", (char*)(temp + sizeof(PACKET)+4));
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
			printf("REMOTESOCKET 생성실패");
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

		//send 도 해야되고 recv도 계속 해야한다.=> thread를 만든다
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
			{		//메인메뉴 루프
				int ClientSelectMode = 0;
				int MyPageMode = 0;
				int ModtfyMydataMode = 0;
				int ReturnVideo = 0;
				printf("1.비디오검색 2.비디오 목록 3.비디오 대여 4.비디오 반납 5.마이페이지 6.로그아웃 \n");
				char SearchVideoName[128] = { NULL };
				char ModifyMyData[516] = { NULL };
				char Modifytemp[128] = { NULL };
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
					printf("반납할 비디오 번호 : ");
					scanf_s("%d", &ReturnVideo);
					fflush(stdin);
					Sendint(RemoteSocket, PG_RETURNVIDEO, ReturnVideo);
					WaitForSingleObject(hContEvent, INFINITE);
					break;
				case 5:
					printf("마이페이지\n");
					printf("1. 내 정보 보기 2.내가 빌린 비디오 정보 3. 정보 수정 4. 탈퇴\n");
					scanf_s("%d", &MyPageMode);
					fflush(stdin);
					switch (MyPageMode)		//마이페이지 루프
					{
					case 1:
						printf("ID	: %s\n", Mydata.MyID);
						printf("이름	: %s\n", Mydata.MyName);
						printf("나이	: %d\n", Mydata.MyAge);
						printf("전화번호: %s\n", Mydata.MyPhoneNumber);
						break;
					case 2:
						SendPacket(RemoteSocket, PG_RENTALLIST, Mydata.MyID, 128);
						WaitForSingleObject(hContEvent, INFINITE);
						break;
					case 3:			
						printf("내 정보 수정\n");
						printf("1. 이름 변경 2. 나이 변경 3. 전화번호 변경 4. 비밀번호 변경\n");
						scanf_s("%d", &ModtfyMydataMode);
						fflush(stdin);
						switch (ModtfyMydataMode)
						{
						case 1:
							printf("수정할 이름 :\n");
							gets_s(Modifytemp, 128);
							fflush(stdin);
							memcpy(Mydata.MyName, Modifytemp, 128);		
							break;
						case 2:
							printf("수정할 나이 :\n");
							scanf_s("%d", &Mydata.MyAge);
							fflush(stdin);
							break;
						case 3:
							printf("수정할 전화번호 :\n");
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
					}			//마이페이지 루프 끝
					break;
				case 6:
					SendPacket(RemoteSocket, PG_LOGOUT, Mydata.MyID, 128);	
					Mydata.MyID = NULL;
				default:
					break;
				}
			}//메인메뉴 루프끝
			
		}	
		RecvParam.iClientClose = 1;
		closesocket(RemoteSocket);
		WSACleanup();
}