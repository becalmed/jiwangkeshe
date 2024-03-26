#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include <cstdio>
#include <array>
#include <memory>
#include <stdexcept>
#include <string>
#pragma comment(lib,"ws2_32.lib")	//把ws2_32.lib加到Link页的连接库  
#define PORT 15001					//服务器端通信端口
#define ERROR 0
#define BUFFER_SIZE 1024			//数据接收缓冲区大小
FILE* file;
using namespace std;
char message[BUFFER_SIZE] = { 0 };
char errowarn[BUFFER_SIZE] = { "输入的命令有错误！\n" };
int isconnect = 0;
char hostName[256];
struct hostent* hostInfo;
struct in_addr** addr_list;
char* appendToFile(const char* str);
struct sockaddr_in clientAddr;			//sockaddr_in结构用来标识tcp/ip协议下的地址
int addrLen = sizeof(clientAddr);
char ipAddr[INET_ADDRSTRLEN];//保存点分十进制的地址
int main()
{

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) == SOCKET_ERROR)//启动winsock ，WSAStartup()函数对Winsock DLL进行初始化 
	{
		printf("Socket initialize fail!\n");
		exit(1);
	}
	SOCKET sock;											//服务进程创建套接字句柄（用于监听）
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)   //调用socket()函数创建一个流套接字，参数（网络地址类型，套接字类型，网络协议）
	{
		printf("Socket create error!\n");
		WSACleanup();
		exit(1);
	}
	struct sockaddr_in ServerAddr;			//sockaddr_in结构用来标识TCP/IP协议下的地址
	ServerAddr.sin_family = AF_INET;		//sin_family字段必须设为AF_INET,表示ipv4
	ServerAddr.sin_port = htons(PORT);		//sin_port字段用于指定服务端口
	ServerAddr.sin_addr.s_addr = INADDR_ANY; //sin_addr字段用于把一个IP地址保存为一个4字节的数
	if (bind(sock, (LPSOCKADDR)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) //调用bind()函数将本地地址绑定到所创建的套接字上
	{
		printf("Bind fail!\n");
		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	if (listen(sock, 10) == SOCKET_ERROR) //监听模式
	{
		printf("Listen fail!\n");
		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	if (gethostname(hostName, sizeof(hostName)) != 0) {
		printf("gethostname failed\n");
		WSACleanup();
		return 1;
	}

	hostInfo = gethostbyname(hostName);
	if (hostInfo == NULL) {
		printf("gethostbyname failed\n");
		WSACleanup();
		return 1;
	}

	addr_list = (struct in_addr**)hostInfo->h_addr_list;

	printf("服务器启动成功  %s:%d\n",  inet_ntoa(*addr_list[0]), ntohs(ServerAddr.sin_port));


	while (1)
	{
		SOCKET msgsock;			//接收accept函数的返回值
		char buf[BUFFER_SIZE];  //数据接收缓冲区
		while (1)
		{
			if ((msgsock = accept(sock, (LPSOCKADDR)0, (int*)0)) == INVALID_SOCKET) //将连接转移给msgsock，sock继续等待接收
			{
				printf("接收失败!\n");
				continue;
			}
			if (isconnect == 1)
			{
				memset(buf, 0, sizeof(buf));											//初始化数据接收缓冲区
				recv(msgsock, buf, BUFFER_SIZE, 0);									  //接收客户端发送过来的数据
				if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't') {	//"exit"命令，退出程序
					printf("%s:%d断开连接\n", inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(clientAddr.sin_port));
					closesocket(msgsock);
					break;
				}
				if (buf[0] == '\0')
				{
					printf("%s:%d断开连接\n", inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(clientAddr.sin_port));
					break;
				}
				printf("Socket\\Server>%s", buf);
				char* addbuf = appendToFile(buf);
				int testint = system(buf);
				//printf("testint is%d", testint);
				//printf("addbuf is:%s", addbuf);
				if (testint == 0)//命令正确
				{
					system(addbuf);

					//system("ping www.baidu.com > log.txt");

					file = fopen("log.txt", "r");
					if (file == NULL) {
						printf("无法打开文件.\n");
						return 1;
					}
					while (fgets(message, sizeof(message), file)) {
						send(msgsock, message, strlen(message), 0);
					}

					// 关闭文件
					fclose(file);

					// 清空日志
					file = fopen("log.txt", "w");
					if (file == NULL) {
						perror("Error opening file");
						return 1;
					}

					// 写入一个空字符串，将文件内容清空
					if (fputs("", file) == EOF) {
						perror("写入日志文件失败");
						fclose(file);
						return 1;
					}

					// 关闭文件
					if (fclose(file) != 0) {
						perror("关闭日志文件失败");
						return 1;
					}
				}
				else
				{
					send(msgsock, errowarn, strlen(errowarn), 0);
				}
				//send(msgsock, "received", 9, 0);
				closesocket(msgsock);
			}
			else
			{

				// 获得对方的ip和端口号
				if (getpeername(msgsock, (LPSOCKADDR)&clientAddr, &addrLen) == SOCKET_ERROR) {
					printf("获取端口号错误\n");
					closesocket(msgsock);
					WSACleanup();
					break;
				}
				printf("客户端 %s:%d 已连接\n", inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(clientAddr.sin_port));
				send(msgsock, "连接成功!\n", 17, 0);
				isconnect = 1;
				closesocket(msgsock);
			}

		}
		isconnect = 0;
		// 清空日志
		file = fopen("log.txt", "w");
		if (file == NULL) {
			perror("Error opening file");
			return 1;
		}

		// 写入一个空字符串，将文件内容清空
		if (fputs("", file) == EOF) {
			perror("写入日志文件失败");
			fclose(file);
			return 1;
		}

		// 关闭文件
		if (fclose(file) != 0) {
			perror("关闭日志文件失败");
			return 1;
		}
	}
	closesocket(sock); //关闭套接字
	WSACleanup();	   //终止对Winsock DLL的使用，并释放资源
	return 0;
}

char* appendToFile(const char* str) {
	// 计算字符串长度，包括终止符 '\0'
	int length = 0;
	while (str[length] != '\n' && str[length] != '\0') {
		length++;
	}

	// 分配内存用于新字符串，需要额外的一个位置用于空字符
	char* newStr = (char*)malloc(length + 12); // 12 是 "> log.txt" 的长度
	if (newStr == NULL) {
		printf("内存分配失败\n");
		return NULL; // 内存分配失败，返回 NULL
	}

	// 复制原始字符串到新字符串
	strcpy(newStr, str);

	// 添加 "> log.txt"
	newStr[length] = ' ';
	newStr[length + 1] = '>';
	newStr[length + 2] = 'l';
	newStr[length + 3] = 'o';
	newStr[length + 4] = 'g';
	newStr[length + 5] = '.';
	newStr[length + 6] = 't';
	newStr[length + 7] = 'x';
	newStr[length + 8] = 't';
	newStr[length + 9] = '\0'; // 确保新字符串以空字符结尾

	return newStr; // 返回修改后的字符串
}