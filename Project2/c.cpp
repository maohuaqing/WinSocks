#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

//1、包含网络头文件网络库
# include <WinSock2.h>
# pragma comment(lib, "Ws2_32.lib")


SOCKET socketClient;

//处理强制关闭事件
BOOL WINAPI CtrlFun(DWORD dwType)
{
	switch (dwType)
	{
	case CTRL_CLOSE_EVENT:
		//关闭socket
		closesocket(socketClient);
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
	socketClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == socketClient)
	{
		//清理网络库，不关闭句柄
		WSACleanup();
		return 0;
	}

	//服务器地址与端口
	struct sockaddr_in si;
	si.sin_family = AF_INET;//这里要和创建SOCKET句柄的参数1类型一样
	si.sin_port = htons(9527);//用htons宏将整型转为端口号的无符号整型
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//客户端不用绑定操作
	/*
	if(bind(socketServer,(struct sockaddr*)&si,sizeof(si))==SOCKET_ERROR)
	{
		int err = WSAGetLastError();//取错误码
		printf("服务器bind失败错误码为：%d\n",err);
		closesocket(socketServer);//释放
		WSACleanup();//清理网络库
	}
	*/

	//5、与客户端收发消息
	while (1)
	{
		//发
		char strSendBuff[548] = { 0 };
		//scanf("%s",strSendBuff);scanf无法处理中间有空格的消息
		gets_s(strSendBuff);
		//用户如果没有输入信息，直接回车就忽略
		if (strSendBuff[0] == '\0')
		{
			continue;
		}
		//发送长度使用547是因为要留一位给/0
		if (sendto(socketClient, strSendBuff, 548, 0, (const struct sockaddr*)&si, sizeof(si)) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();//取错误码
			printf("客户端sendto失败错误码为：%d\n", err);
			//continue;
		}

		//收
		struct sockaddr sa;
		int iSaLen = sizeof(sa);
		char strRecvBuff[548] = { 0 };

		if (recvfrom(socketClient, strRecvBuff, 548, 0, &sa, &iSaLen) == SOCKET_ERROR)
		{
			int err = WSAGetLastError();//取错误码
			printf("客户端recvfrom失败错误码为：%d\n", err);
			//continue;
		}
		printf("客户端recvfrom消息是：%s\n", strRecvBuff);

	}


	closesocket(socketClient);//与4、创建SOCKET对应，如果有创建客户端SOCKET句柄，也要关闭
	//关闭网络库
	WSACleanup();
	system("pause");
	return 0;
}