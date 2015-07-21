
# define _WINSOCK_DEPRECATED_NO_WARNING
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <process.h>

#pragma  comment(lib,"Ws2_32.lib")	//���̺귯�� �߰�

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
	SOCKET RemoteSocket = (SOCKET)pParam;
	bool bCont = true;

	char RecvBuffer[2048]; // �����͸� ���� ���� [�ִ� �ι�� �����͸� ���� ���� �����ϱ�]�ι�ѱ����� �չ��۰� ó���ȴ�.
	int iCurrent = 0;
	while (bCont){
		char temp[1024];
		int iRecvLen = recv(RemoteSocket, RecvBuffer + iCurrent, 1024, NULL);				//client�� �ް� �ٽ� ������ ������ �޴´�. //iRecvLen = recv���� ������ ����Ʈ��
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
		printf("REMOTESOCKET ��������");
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

	//send �� �ؾߵǰ� recv�� ��� �ؾ��Ѵ�.=> thread�� �����
	
	_beginthreadex(NULL, NULL, RecvFunc, (void*)RemoteSocket, NULL, NULL);

	while (1)
	{
		int iLoginMenu = 0;
		printf("1.�α��� 2.ȸ������ 3.������\n");
		scanf_s("%d", &iLoginMenu);
		fflush(stdin);
		switch (iLoginMenu)
		{
		case 1:
			SendLoginData(RemoteSocket);
			printf("�α��� ����\n");
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
		if (temp[0] == NULL)		//����ġ�� ��
		{
			break;
		}
		SendPacket(RemoteSocket, PG_CHATMSG, temp, strlen(temp) + 1);
	}

	closesocket(RemoteSocket);
	WSACleanup();

}