#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
//1����������ͷ�ļ������
# include <WinSock2.h>
# pragma comment(lib, "Ws2_32.lib")

SOCKET socketServer;

//����ǿ�ƹر��¼�
BOOL WINAPI CtrlFun(DWORD dwType)
{
	switch (dwType)
	{
	case CTRL_CLOSE_EVENT:
		//�ر�socket
		closesocket(socketServer);
		//�ر������
		WSACleanup();
		break;
	}
	return FALSE;
}

int main()
{
	//Ͷ�ݹر��¼�
	SetConsoleCtrlHandler(CtrlFun, TRUE);

	WORD wVersionRequested = MAKEWORD(2, 2);//�汾
	WSADATA wsaDATA;

	//2���������
	int iret = WSAStartup(wVersionRequested, &wsaDATA);
	if (iret != 0)
	{
		//�д�
		switch (iret)
		{
		case WSASYSNOTREADY:
			printf("�������������������");
			break;
		case WSAVERNOTSUPPORTED:
			printf("������������������");
			break;
		case WSAEINPROGRESS:
			printf("�������������������");
			break;
		case WSAEPROCLIM:
			printf("����������������Ӵﵽ���޻��������رղ���Ҫ���");
			break;
		case WSAEFAULT:
			printf("�����������������");
			break;
		}
		getchar();
		return 0;
	}

	//3��У��汾��ֻҪ��һ������2��˵��ϵͳ��֧������Ҫ��2.2�汾	
	if (2 != HIBYTE(wsaDATA.wVersion) || 2 != LOBYTE(wsaDATA.wVersion))
	{
		printf("�汾�����⣡");
		WSACleanup();//�ر������
		return 0;
	}

	// 4������SOCKET
	socketServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == socketServer)
	{
		//��������⣬���رվ��
		WSACleanup();
		return 0;
	}

	//5���󶨵�ַ��˿�
	struct sockaddr_in si;
	si.sin_family = AF_INET;//����Ҫ�ʹ���SOCKET����Ĳ���1����һ��
	si.sin_port = htons(9527);//��htons�꽫����תΪ�˿ںŵ��޷�������
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (bind(socketServer, (struct sockaddr*)&si, sizeof(si)) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();//ȡ������
		printf("������bindʧ�ܴ�����Ϊ��%d\n", err);
		closesocket(socketServer);//�ͷ�
		WSACleanup();//���������
	}

	//6����ͻ����շ���Ϣ
	while (1)
	{
		//��
		struct sockaddr sa;
		int iSaLen = sizeof(sa);
		char strRecvBuff[548] = { 0 };

		struct sockaddr_in* temp;
		if (recvfrom(socketServer, strRecvBuff, 548, 0, &sa, &iSaLen) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();//ȡ������
			printf("������recvfromʧ�ܴ�����Ϊ��%d\n", err);
			continue;
		}
		temp = (struct sockaddr_in*)&sa;
		printf("������recvfrom�˿ں�Ϊ��%d\n", ntohs(temp->sin_port));
		//��ntohs(temp1->sin_port)��
		printf("������recvfrom��Ϣ�ǣ�%s\n", strRecvBuff);

		//��
		if (sendto(socketServer, "This is a message from server~!", sizeof("This is a message from server~!"), 0, (const struct sockaddr*)temp, sizeof(sa)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();//ȡ������
			printf("������sendtoʧ�ܴ�����Ϊ��%d\n", err);
			continue;
		}

	}


	closesocket(socketServer);//��4������SOCKET��Ӧ������д����ͻ���SOCKET�����ҲҪ�ر�
	//�ر������
	WSACleanup();
	system("pause");
	return 0;
}