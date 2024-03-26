#include <winsock2.h>
#include <stdio.h>
#include <string>
#include <Ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")	//��ws2_32.lib�ӵ�Linkҳ�����ӿ�  
//#define IP "192.168.212.54"			//����̨������ϲ��ԣ�IPΪServer�˵�IP��ַ
//#define IP "127.0.0.1"				//��һ̨������ϲ��ԣ�IPΪ���ػ��͵�ַ
//#define PORT 15001					//ע�⣺�ͻ�������ͨ�ŵĶ˿� = ����˵Ķ˿�
#define BUFFER_SIZE 1024			//���ݷ��ͻ�������С
int main() {
	char buf[BUFFER_SIZE];								//�ͻ��˷��͵���Ϣ
	int inputLen;
	char IP[60];
	int PORT;
	int connect_flag = 0;
	while (1)
	{
		printf("�����������IP��ַ:");
		scanf("%s", &IP);
		printf("������������˿ں�:");
		scanf("%d", &PORT);
		while (1) {
			WSADATA WSAData;
			if (WSAStartup(MAKEWORD(2, 0), &WSAData) == SOCKET_ERROR) { //WSAStartup()������Winsock DLL���г�ʼ��
				printf("Socket initialize fail!\n");
				continue;
			}
			SOCKET sock;											        //�ͻ��˽��̴����׽���
			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) { //�������׽��֣������˱���һ�£�
				printf("Socket create fail!\n");
				WSACleanup();
				continue;
			}
			char res[1024] = { 0 };
			struct sockaddr_in ClientAddr;				//sockaddr_in�ṹ������ʶTCP/IPЭ���µĵ�ַ
			ClientAddr.sin_family = AF_INET;			//ipv4
			ClientAddr.sin_port = htons(PORT);			//����˶˿�	
			ClientAddr.sin_addr.s_addr = inet_addr(IP);	//����˵�IP��ַ
			if (connect_flag==0)
			{
				printf("��������...");
			}
			if (connect(sock, (LPSOCKADDR)&ClientAddr, sizeof(ClientAddr)) == SOCKET_ERROR) //����������̷�����������
			{
				printf("����ʧ��!\n");
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
			while ((buf[inputLen++] = getchar()) != '\n') {	//�û�����ָ��Կո����
				;
			}
			if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't') {
				printf("The End.\n");
				send(sock, buf, BUFFER_SIZE, 0);
				break;
			}
			send(sock, buf, BUFFER_SIZE, 0);			//���������������
			memset(buf, 0, sizeof(buf));

			while (recv(sock, buf, BUFFER_SIZE, 0) > 0) {
				printf("%s", buf);
			}

			//recv(sock, buf, BUFFER_SIZE, 0);			//���շ������ķ���ֵ
			//printf("%s\n", buf);
			closesocket(sock);							 //�ر��׽���
			WSACleanup();								//��ֹ��Winsock DLL��ʹ�ã����ͷ���Դ
		}
	}
	return 0;
}