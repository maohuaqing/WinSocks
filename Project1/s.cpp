#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
//1、包含网络头文件网络库
# include <WinSock2.h>
# pragma comment(lib, "Ws2_32.lib")

SOCKET socketServer;

//处理强制关闭事件
BOOL WINAPI CtrlFun(DWORD dwType)
{
	switch (dwType)
	{
	case CTRL_CLOSE_EVENT:
		//关闭socket
		closesocket(socketServer);
		//关闭网络库
		WSACleanup();
		break;
	}
	return FALSE;
}

int main()
{
	//投递关闭事件
	SetConsoleCtrlHandler(CtrlFun, TRUE);

	WORD wVersionRequested = MAKEWORD(2, 2);//版本
	WSADATA wsaDATA;

	//2、打开网络库
	int iret = WSAStartup(wVersionRequested, &wsaDATA);
	if (iret != 0)
	{
		//有错
		switch (iret)
		{
		case WSASYSNOTREADY:
			printf("解决方案：重启。。。");
			break;
		case WSAVERNOTSUPPORTED:
			printf("解决方案：更新网络库");
			break;
		case WSAEINPROGRESS:
			printf("解决方案：重启。。。");
			break;
		case WSAEPROCLIM:
			printf("解决方案：网络连接达到上限或阻塞，关闭不必要软件");
			break;
		case WSAEFAULT:
			printf("解决方案：程序有误");
			break;
		}
		getchar();
		return 0;
	}

	//3、校验版本，只要有一个不是2，说明系统不支持我们要的2.2版本	
	if (2 != HIBYTE(wsaDATA.wVersion) || 2 != LOBYTE(wsaDATA.wVersion))
	{
		printf("版本有问题！");
		WSACleanup();//关闭网络库
		return 0;
	}

	// 4、创建SOCKET
	socketServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == socketServer)
	{
		//清理网络库，不关闭句柄
		WSACleanup();
		return 0;
	}

	//5、绑定地址与端口
	struct sockaddr_in si;
	si.sin_family = AF_INET;//这里要和创建SOCKET句柄的参数1类型一样
	si.sin_port = htons(9527);//用htons宏将整型转为端口号的无符号整型
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	if (bind(socketServer, (struct sockaddr*)&si, sizeof(si)) == SOCKET_ERROR)
	{
		int err = WSAGetLastError();//取错误码
		printf("服务器bind失败错误码为：%d\n", err);
		closesocket(socketServer);//释放
		WSACleanup();//清理网络库
	}

	//6、与客户端收发消息
	while (1)
	{
		//收
		struct sockaddr sa;
		int iSaLen = sizeof(sa);
		char strRecvBuff[548] = { 0 };

		struct sockaddr_in* temp;
		if (recvfrom(socketServer, strRecvBuff, 548, 0, &sa, &iSaLen) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();//取错误码
			printf("服务器recvfrom失败错误码为：%d\n", err);
			continue;
		}
		temp = (struct sockaddr_in*)&sa;
		printf("服务器recvfrom端口号为：%d\n", ntohs(temp->sin_port));
		//（ntohs(temp1->sin_port)）
		printf("服务器recvfrom消息是：%s\n", strRecvBuff);

		//发
		if (sendto(socketServer, "This is a message from server~!", sizeof("This is a message from server~!"), 0, (const struct sockaddr*)temp, sizeof(sa)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();//取错误码
			printf("服务器sendto失败错误码为：%d\n", err);
			continue;
		}

	}


	closesocket(socketServer);//与4、创建SOCKET对应，如果有创建客户端SOCKET句柄，也要关闭
	//关闭网络库
	WSACleanup();
	system("pause");
	return 0;
}