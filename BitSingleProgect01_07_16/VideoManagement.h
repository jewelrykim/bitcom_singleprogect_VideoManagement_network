#ifndef DEFINE_LINKEDLIST	//define�� �ȵǾ������� true
typedef struct _MEMBER
{
	char* pName;		//����̸�
	int pAge;			//�������
	char* pPhone;		//�����ȭ��ȣ
	bool bManager;		//������Ȯ�� ture ������
}MEMBER;
typedef struct _VIDEO
{
	int iVNumber;		//������ȣ
	char * pVName;		//�����̸�
	int iGrade;			//�������
	char * pIntroduce;	//�����Ұ�
	bool bRent;			//ture �뿩���� false �뿩��
	bool bReservation;	//ture ���డ�� false ��������
}VIDEO;
typedef struct _RENTAL
{
	char * pRMemberName;//�뿩�� ����̸�
	char * pRVideoName;	//�뿩�� �����̸�
	int iRVideoNum;		//�뿩�� ������ȣ
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
typedef void(*INITLINKEDLIST)(struct _LINKEDLIST * pLinkedList, ADDNODE AddTop, ADDNODE AddBottom, DELETEAT DeleteAt,
	DELETENODE DeleteTop, DELETENODE DeleteBottom, DELETENODE DeleteAll);
typedef struct _LINKEDLIST
{
	NODE * pTop;
	NODE * pBottom;
	INITLINKEDLIST InitList;
	ADDNODE AddTop;
	ADDNODE AddBottom;;
	LINKNODE LinkNode;
	DELETEAT DeleteAt;
	DELETEAT DeleteTarget;
	DELETENODE DeleteTop;
	DELETENODE DeleteBottom;
	DELETENODE DeleteAll;
	int iCount;
}LINKEDLIST;




void LinkNode(LINKEDLIST * pLinkedList, void * pData, NODE * pSearch);
void AddTop(LINKEDLIST * pLinkedList, void * pData);
void AddBottom(LINKEDLIST * pLinkedList, void * pData);
bool DeleteAt(LINKEDLIST * pLinkedList, NODE * pSearch);
bool DeleteTop(LINKEDLIST * pLinkedList);
bool DeleteBottom(LINKEDLIST * pLinkedList);
bool DeleteAll(LINKEDLIST * pLinkedList);
void InitLinkedList(LINKEDLIST * pLinkedList, ADDNODE AddTop, ADDNODE AddBottom, DELETEAT DeleteAt, DELETENODE DeleteTop, DELETENODE DeleteBottom, DELETENODE DeleteAll);
NODE * FindFlag(LINKEDLIST* list, int iInputdata);
void InsertChar(const char * pTitle, char ** MemberData);
void InsertInt(const char * pTitle, int * MemberData);
void InputMemberData(NODE** pNode);
void InputVideoData(NODE** pNode);
void InputMember(LINKEDLIST* list);
void InputVideo(LINKEDLIST* list);
void PrintMember(NODE * pStart);
void PrintVideo(NODE * pStart);
void vPrintMember(LINKEDLIST* list);
void vPrintVideo(LINKEDLIST* list);
NODE * SSearchName(LINKEDLIST* list, char* iSearch);
NODE * SSearchNumber(LINKEDLIST* list, int iSearch);
void SearchMember(LINKEDLIST* list);
void SearchVideo(LINKEDLIST* list);
void ModifyMember(LINKEDLIST* list);
void ModifyVideo(LINKEDLIST* list);
void DeleteMember(LINKEDLIST* list);
void DeleteVideo(LINKEDLIST* list);
void InputRentalData(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, NODE** Rebtallist);
void InputRental(LINKEDLIST* Memberlist, LINKEDLIST* Videolist, LINKEDLIST* Rentallist);
void ReturnVideo(LINKEDLIST* Rentallist);
void PrintRentallist(LINKEDLIST* Rentallist);
typedef bool(*COMPAREFUNC)(NODE * pNode, void * pData);
NODE* Search(LINKEDLIST * pList, COMPAREFUNC compFunc, void* pData);
//void VideoManagemanetMain();
#endif