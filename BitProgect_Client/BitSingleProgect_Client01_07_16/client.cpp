
# define _WINSOCK_DEPRECATED_NO_WARNING
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>

#pragma  comment(lib,"Ws2_32.lib")	//라이브러리 추가

#define  PG_CHATMSG 1
#define  PG_KILLUSER 2
#define  PG_LOGIN 3
#define  PG_SIGNUP 4

typedef struct _PACKET
{
	int iTotalSize;
	int iPacketGubun;
}PACKET;

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
	printf("\n");
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
	SOCKET RemoteSocket = (SOCKET)pParam;
	bool bCont = true;

	char RecvBuffer[2048]; // 데이터를 받을 버퍼 [최대 두배로 데이터를 받을 수도 있으니까]두배넘기전에 앞버퍼가 처리된다.
	int iCurrent = 0;
	while (bCont){
		char temp[1024];
		int iRecvLen = recv(RemoteSocket, RecvBuffer + iCurrent, 1024, NULL);				//client가 받고 다시 보내는 정보를 받는다. //iRecvLen = recv받은 정보의 바이트수
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
			case PG_CHATMSG:
				printf("%s\n", pMsg);
				break;
			case PG_KILLUSER:
				bCont = false;
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
	WSADATA data;
	memset(&data, 0, sizeof(data));
	WSAStartup(MAKEWORD(2, 2), &data);

	SOCKET RemoteSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (RemoteSocket == SOCKET_ERROR)
	{
		printf("REMOTESOCKET 생성실패");
		return;
	}

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = inet_addr("165.132.221.163");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(50000);
	int iError = connect(RemoteSocket, (sockaddr*)&addr, sizeof(addr));
	if (iError == SOCKET_ERROR)
	{
		printf("connect Error\n");
		return;
	}

	//send 도 해야되고 recv도 계속 해야한다.=> thread를 만든다
	
	_beginthreadex(NULL, NULL, RecvFunc, (void*)RemoteSocket, NULL, NULL);

	while (1)
	{
		int iLoginMenu = 0;
		printf("1.로그인 2.회원가입 3.나가기\n");
		scanf_s("%d", &iLoginMenu);
		fflush(stdin);
		switch (iLoginMenu)
		{
		case 1:
			SendLoginData(RemoteSocket);
			printf("로그인 실패\n");
			break;
		case 2:
			SendSignUpData(RemoteSocket);
			break;
		default:
			break;
		}	
		if (iLoginMenu == 2)
			break;
	}
	while (1)
	{
		char temp[1024];
		gets_s(temp, 1024);
		if (temp[0] == NULL)		//엔터치면 끝
		{
			break;
		}
		SendPacket(RemoteSocket, PG_CHATMSG, temp, strlen(temp) + 1);
	}

	closesocket(RemoteSocket);
	WSACleanup();

}