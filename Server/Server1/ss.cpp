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
#pragma comment(lib,"ws2_32.lib")	//��ws2_32.lib�ӵ�Linkҳ�����ӿ�  
#define PORT 15001					//��������ͨ�Ŷ˿�
#define ERROR 0
#define BUFFER_SIZE 1024			//���ݽ��ջ�������С
FILE* file;
using namespace std;
char message[BUFFER_SIZE] = { 0 };
char errowarn[BUFFER_SIZE] = { "����������д���\n" };
int isconnect = 0;
char hostName[256];
struct hostent* hostInfo;
struct in_addr** addr_list;
char* appendToFile(const char* str);
struct sockaddr_in clientAddr;			//sockaddr_in�ṹ������ʶtcp/ipЭ���µĵ�ַ
int addrLen = sizeof(clientAddr);
char ipAddr[INET_ADDRSTRLEN];//������ʮ���Ƶĵ�ַ
int main()
{

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 0), &WSAData) == SOCKET_ERROR)//����winsock ��WSAStartup()������Winsock DLL���г�ʼ�� 
	{
		printf("Socket initialize fail!\n");
		exit(1);
	}
	SOCKET sock;											//������̴����׽��־�������ڼ�����
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)   //����socket()��������һ�����׽��֣������������ַ���ͣ��׽������ͣ�����Э�飩
	{
		printf("Socket create error!\n");
		WSACleanup();
		exit(1);
	}
	struct sockaddr_in ServerAddr;			//sockaddr_in�ṹ������ʶTCP/IPЭ���µĵ�ַ
	ServerAddr.sin_family = AF_INET;		//sin_family�ֶα�����ΪAF_INET,��ʾipv4
	ServerAddr.sin_port = htons(PORT);		//sin_port�ֶ�����ָ������˿�
	ServerAddr.sin_addr.s_addr = INADDR_ANY; //sin_addr�ֶ����ڰ�һ��IP��ַ����Ϊһ��4�ֽڵ���
	if (bind(sock, (LPSOCKADDR)&ServerAddr, sizeof(ServerAddr)) == SOCKET_ERROR) //����bind()���������ص�ַ�󶨵����������׽�����
	{
		printf("Bind fail!\n");
		closesocket(sock);
		WSACleanup();
		exit(1);
	}

	if (listen(sock, 10) == SOCKET_ERROR) //����ģʽ
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

	printf("�����������ɹ�  %s:%d\n",  inet_ntoa(*addr_list[0]), ntohs(ServerAddr.sin_port));


	while (1)
	{
		SOCKET msgsock;			//����accept�����ķ���ֵ
		char buf[BUFFER_SIZE];  //���ݽ��ջ�����
		while (1)
		{
			if ((msgsock = accept(sock, (LPSOCKADDR)0, (int*)0)) == INVALID_SOCKET) //������ת�Ƹ�msgsock��sock�����ȴ�����
			{
				printf("����ʧ��!\n");
				continue;
			}
			if (isconnect == 1)
			{
				memset(buf, 0, sizeof(buf));											//��ʼ�����ݽ��ջ�����
				recv(msgsock, buf, BUFFER_SIZE, 0);									  //���տͻ��˷��͹���������
				if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't') {	//"exit"����˳�����
					printf("%s:%d�Ͽ�����\n", inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(clientAddr.sin_port));
					closesocket(msgsock);
					break;
				}
				if (buf[0] == '\0')
				{
					printf("%s:%d�Ͽ�����\n", inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(clientAddr.sin_port));
					break;
				}
				printf("Socket\\Server>%s", buf);
				char* addbuf = appendToFile(buf);
				int testint = system(buf);
				//printf("testint is%d", testint);
				//printf("addbuf is:%s", addbuf);
				if (testint == 0)//������ȷ
				{
					system(addbuf);

					//system("ping www.baidu.com > log.txt");

					file = fopen("log.txt", "r");
					if (file == NULL) {
						printf("�޷����ļ�.\n");
						return 1;
					}
					while (fgets(message, sizeof(message), file)) {
						send(msgsock, message, strlen(message), 0);
					}

					// �ر��ļ�
					fclose(file);

					// �����־
					file = fopen("log.txt", "w");
					if (file == NULL) {
						perror("Error opening file");
						return 1;
					}

					// д��һ�����ַ��������ļ��������
					if (fputs("", file) == EOF) {
						perror("д����־�ļ�ʧ��");
						fclose(file);
						return 1;
					}

					// �ر��ļ�
					if (fclose(file) != 0) {
						perror("�ر���־�ļ�ʧ��");
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

				// ��öԷ���ip�Ͷ˿ں�
				if (getpeername(msgsock, (LPSOCKADDR)&clientAddr, &addrLen) == SOCKET_ERROR) {
					printf("��ȡ�˿ںŴ���\n");
					closesocket(msgsock);
					WSACleanup();
					break;
				}
				printf("�ͻ��� %s:%d ������\n", inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddr, sizeof(ipAddr)), ntohs(clientAddr.sin_port));
				send(msgsock, "���ӳɹ�!\n", 17, 0);
				isconnect = 1;
				closesocket(msgsock);
			}

		}
		isconnect = 0;
		// �����־
		file = fopen("log.txt", "w");
		if (file == NULL) {
			perror("Error opening file");
			return 1;
		}

		// д��һ�����ַ��������ļ��������
		if (fputs("", file) == EOF) {
			perror("д����־�ļ�ʧ��");
			fclose(file);
			return 1;
		}

		// �ر��ļ�
		if (fclose(file) != 0) {
			perror("�ر���־�ļ�ʧ��");
			return 1;
		}
	}
	closesocket(sock); //�ر��׽���
	WSACleanup();	   //��ֹ��Winsock DLL��ʹ�ã����ͷ���Դ
	return 0;
}

char* appendToFile(const char* str) {
	// �����ַ������ȣ�������ֹ�� '\0'
	int length = 0;
	while (str[length] != '\n' && str[length] != '\0') {
		length++;
	}

	// �����ڴ��������ַ�������Ҫ�����һ��λ�����ڿ��ַ�
	char* newStr = (char*)malloc(length + 12); // 12 �� "> log.txt" �ĳ���
	if (newStr == NULL) {
		printf("�ڴ����ʧ��\n");
		return NULL; // �ڴ����ʧ�ܣ����� NULL
	}

	// ����ԭʼ�ַ��������ַ���
	strcpy(newStr, str);

	// ��� "> log.txt"
	newStr[length] = ' ';
	newStr[length + 1] = '>';
	newStr[length + 2] = 'l';
	newStr[length + 3] = 'o';
	newStr[length + 4] = 'g';
	newStr[length + 5] = '.';
	newStr[length + 6] = 't';
	newStr[length + 7] = 'x';
	newStr[length + 8] = 't';
	newStr[length + 9] = '\0'; // ȷ�����ַ����Կ��ַ���β

	return newStr; // �����޸ĺ���ַ���
}