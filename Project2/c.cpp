#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

//1����������ͷ�ļ������
# include <WinSock2.h>
# pragma comment(lib, "Ws2_32.lib")


SOCKET socketClient;

//����ǿ�ƹر��¼�
BOOL WINAPI CtrlFun(DWORD dwType)
{
	switch (dwType)
	{
	case CTRL_CLOSE_EVENT:
		//�ر�socket
		closesocket(socketClient);
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
	socketClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == socketClient)
	{
		//��������⣬���رվ��
		WSACleanup();
		return 0;
	}

	//��������ַ��˿�
	struct sockaddr_in si;
	si.sin_family = AF_INET;//����Ҫ�ʹ���SOCKET����Ĳ���1����һ��
	si.sin_port = htons(9527);//��htons�꽫����תΪ�˿ںŵ��޷�������
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//�ͻ��˲��ð󶨲���
	/*
	if(bind(socketServer,(struct sockaddr*)&si,sizeof(si))==SOCKET_ERROR)
	{
		int err = WSAGetLastError();//ȡ������
		printf("������bindʧ�ܴ�����Ϊ��%d\n",err);
		closesocket(socketServer);//�ͷ�
		WSACleanup();//���������
	}
	*/

	//5����ͻ����շ���Ϣ
	while (1)
	{
		//��
		char strSendBuff[548] = { 0 };
		//scanf("%s",strSendBuff);scanf�޷������м��пո����Ϣ
		gets_s(strSendBuff);
		//�û����û��������Ϣ��ֱ�ӻس��ͺ���
		if (strSendBuff[0] == '\0')
		{
			continue;
		}
		//���ͳ���ʹ��547����ΪҪ��һλ��/0
		if (sendto(socketClient, strSendBuff, 548, 0, (const struct sockaddr*)&si, sizeof(si)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();//ȡ������
			printf("�ͻ���sendtoʧ�ܴ�����Ϊ��%d\n", err);
			//continue;
		}

		//��
		struct sockaddr sa;
		int iSaLen = sizeof(sa);
		char strRecvBuff[548] = { 0 };

		if (recvfrom(socketClient, strRecvBuff, 548, 0, &sa, &iSaLen) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();//ȡ������
			printf("�ͻ���recvfromʧ�ܴ�����Ϊ��%d\n", err);
			//continue;
		}
		printf("�ͻ���recvfrom��Ϣ�ǣ�%s\n", strRecvBuff);

	}


	closesocket(socketClient);//��4������SOCKET��Ӧ������д����ͻ���SOCKET�����ҲҪ�ر�
	//�ر������
	WSACleanup();
	system("pause");
	return 0;
}