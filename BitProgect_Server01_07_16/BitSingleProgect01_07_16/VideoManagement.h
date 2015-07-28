#ifndef DEFINE_LINKEDLIST	//define�� �ȵǾ������� true
typedef struct _MEMBER
{
	char* pMemberID;	//���ID
	char* pPassWord;	//��� PassWord
	char* pName;		//����̸�
	int pAge;			//�������
	char* pPhone;		//�����ȭ��ȣ
}MEMBER;
typedef struct _VIDEO
{
	int iVNumber;		//������ȣ
	char * pVName;		//�����̸�
	int iGrade;			//�������
	//char * pIntroduce;	//�����Ұ�
	bool bRent;			//ture �뿩���� false �뿩��
	bool bReservation;	//ture ���డ�� false ��������
}VIDEO;
typedef struct _RENTAL
{
	char* pMemberID;//�뿩�� ����� ���̵�
	char * pRMemberName;//�뿩�� ����̸�
	int iRVideoNum;		//�뿩�� ������ȣ
	char * pRVideoName;	//�뿩�� �����̸�

}RENTAL;
typedef struct _NODE
{
	void * pData;		//�����͸� ������ ���
	struct _NODE * pNext;//�ٸ� ���
	struct _NODE * pPre; //���� ���
}NODE;
typedef void(*ADDNODE)(struct _LINKEDLIST * pLinkedList, void * pData);
typedef void(*LINKNODE)(struct _LINKEDLIST * pLinkedList, void * pData, NODE * pSearch);
typedef bool(*DELETEAT)(struct _LINKEDLIST * pLinkedList, NODE * pSearch);
typedef bool(*DELETENODE)(struct _LINKEDLIST * pLinkedList);
typedef void(*INITLINKEDLIST)(struct _LINKEDLIST * pLinkedList, ADDNODE AddTop, ADDNODE AddBottom, ADDNODE AddPoint, DELETEAT DeleteAt,
	DELETENODE DeleteTop, DELETENODE DeleteBottom, DELETENODE DeleteAll);
typedef struct _LINKEDLIST
{
	NODE * pTop;
	NODE * pBottom;
	INITLINKEDLIST InitList;
	ADDNODE AddTop;
	ADDNODE AddBottom;;
	ADDNODE AddPoint;
	LINKNODE LinkNode;
	DELETEAT DeleteAt;
	DELETEAT DeleteTarget;
	DELETENODE DeleteTop;
	DELETENODE DeleteBottom;
	DELETENODE DeleteAll;
	int iCount;
}LINKEDLIST;

typedef struct _PACKET
{
	int iTotalSize;
	int iPacketGubun;
}PACKET;

void LinkNode(LINKEDLIST * pLinkedList, void * pData, NODE * pSearch);
void LinkFindNode(LINKEDLIST * pLinkedList, void * pData, NODE * pSearch);
void AddTop(LINKEDLIST * pLinkedList, void * pData);
void AddBottom(LINKEDLIST * pLinkedList, void * pData);
void AddPoint(LINKEDLIST * pLinkedList, void * pData);
bool DeleteAt(LINKEDLIST * pLinkedList, NODE * pSearch);
bool DeleteTop(LINKEDLIST * pLinkedList);
bool DeleteBottom(LINKEDLIST * pLinkedList);
bool DeleteAll(LINKEDLIST * pLinkedList);
void InitLinkedList(LINKEDLIST * pLinkedList, ADDNODE AddTop, ADDNODE AddBottom, ADDNODE AddPoint, DELETEAT DeleteAt, DELETENODE DeleteTop, DELETENODE DeleteBottom, DELETENODE DeleteAll);
NODE * FindFlag(LINKEDLIST* list, int iInputdata);
void InsertChar(const char * pTitle, char ** MemberData);
void InsertInt(const char * pTitle, int * MemberData);
void InputMemberData_Client(MEMBER* pMember, void* temp);
void InputMemberData_Server(MEMBER* pMember);
void InputVideoData(VIDEO * pVideo);
void InputMember_Client(LINKEDLIST* Memberlist, LINKEDLIST* LoginList, void* temp);
void InputMember_Server(LINKEDLIST* Memberlist, LINKEDLIST* LoginList);
void InputVideo(LINKEDLIST* list);
void PrintMember(NODE * pStart);
void PrintVideo(NODE * pStart);
void vPrintMember(LINKEDLIST* list);
void vPrintVideo(LINKEDLIST* list);
NODE * SSearchID(LINKEDLIST* list, char* iSearch);
NODE * SSearchName(LINKEDLIST* list, char* iSearch);
NODE * SSearchRentalName(LINKEDLIST* list, char* iSearch);
NODE * SSearchVideoName(LINKEDLIST* list, char* iSearch);
NODE * MSearchVideoName(LINKEDLIST* list, LINKEDLIST* SameNameRentalList, char* iSearch);
NODE * SSearchNumber(LINKEDLIST* list, int iSearch);
void SearchMember(LINKEDLIST* list);
void SearchVideo(LINKEDLIST* list);
void ModifyMember_Client(LINKEDLIST* list, char * temp);
void ModifyMember_Server(LINKEDLIST* list);
void ModifyVideo(LINKEDLIST* list);
void DeleteMember_Client(LINKEDLIST* Memberlist, char * temp);
void DeleteMember_Server(LINKEDLIST* list);
void DeleteVideo(LINKEDLIST* list);
int InputRentalData_Server(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, NODE** Rebtallist);
void InPutRentalData_Client(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* SameNameRentalList, RENTAL* pRental, void* Member, void* Video, int VideoNum);
void InputRental_Server(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* Rentallist);
void InputRental_Client(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* Rentallist, LINKEDLIST* SameNameRentalList, void* Member, void* Video, int VideoNum);
int ReturnVideo_Client(LINKEDLIST* Rentallist, LINKEDLIST* Videolist, int VideoNum);
void ReturnVideo(LINKEDLIST* Rentallist, LINKEDLIST* Videolist);
void PrintRentallist(LINKEDLIST* Rentallist);
typedef bool(*COMPAREFUNC)(NODE * pNode, void * pData);
NODE* Search(LINKEDLIST * pList, COMPAREFUNC compFunc, void* pData);
void PrintMemberRentalList(LINKEDLIST* Rentallist, char temp[]);
void File_Write_Member(LINKEDLIST* MemberList);
void File_Read_char(FILE * fp, char **pData);
void File_Read_int(FILE * fp, int *pData);
void File_Read_Member(LINKEDLIST* MemberList);
void File_Write_Video(LINKEDLIST* VideoList);
void File_Read_Video(LINKEDLIST* VideoList);
void File_Write_Rental(LINKEDLIST* RentalList);
void File_Read_Rental(LINKEDLIST* RentalList);
#endif