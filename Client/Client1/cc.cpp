#include <winsock2.h>
#include <stdio.h>
#include <string>
#include <Ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")	//把ws2_32.lib加到Link页的连接库  
//#define IP "192.168.212.54"			//在两台计算机上测试，IP为Server端的IP地址
//#define IP "127.0.0.1"				//在一台计算机上测试，IP为本地回送地址
//#define PORT 15001					//注意：客户端设置通信的端口 = 服务端的端口
#define BUFFER_SIZE 1024			//数据发送缓冲区大小
int main() {
	char buf[BUFFER_SIZE];								//客户端发送的消息
	int inputLen;
	char IP[60];
	int PORT;
	int connect_flag = 0;
	while (1)
	{
		printf("请输入服务器IP地址:");
		scanf("%s", &IP);
		printf("请输入服务器端口号:");
		scanf("%d", &PORT);
		while (1) {
			WSADATA WSAData;
			if (WSAStartup(MAKEWORD(2, 0), &WSAData) == SOCKET_ERROR) { //WSAStartup()函数对Winsock DLL进行初始化
				printf("Socket initialize fail!\n");
				continue;
			}
			SOCKET sock;											        //客户端进程创建套接字
			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) { //创建流套接字（与服务端保持一致）
				printf("Socket create fail!\n");
				WSACleanup();
				continue;
			}
			char res[1024] = { 0 };
			struct sockaddr_in ClientAddr;				//sockaddr_in结构用来标识TCP/IP协议下的地址
			ClientAddr.sin_family = AF_INET;			//ipv4
			ClientAddr.sin_port = htons(PORT);			//服务端端口	
			ClientAddr.sin_addr.s_addr = inet_addr(IP);	//服务端的IP地址
			if (connect_flag==0)
			{
				printf("正在连接...");
			}
			if (connect(sock, (LPSOCKADDR)&ClientAddr, sizeof(ClientAddr)) == SOCKET_ERROR) //向服务器进程发出连接请求
			{
				printf("连接失败!\n");
				closesocket(sock);
				WSACleanup();
				break;
			}
			if (connect_flag==1)
			{
				printf("Socket\\Client>");
			}
			connect_flag = 1;
			inputLen = 0;
			memset(buf, 0, sizeof(buf));
			while ((buf[inputLen++] = getchar()) != '\n') {	//用户输入指令，以空格结束
				;
			}
			if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't') {
				printf("The End.\n");
				send(sock, buf, BUFFER_SIZE, 0);
				break;
			}
			send(sock, buf, BUFFER_SIZE, 0);			//向服务器发送数据
			memset(buf, 0, sizeof(buf));

			while (recv(sock, buf, BUFFER_SIZE, 0) > 0) {
				printf("%s", buf);
			}

			//recv(sock, buf, BUFFER_SIZE, 0);			//接收服务器的返回值
			//printf("%s\n", buf);
			closesocket(sock);							 //关闭套接字
			WSACleanup();								//终止对Winsock DLL的使用，并释放资源
		}
	}
	return 0;
}