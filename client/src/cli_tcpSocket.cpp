#include "cli_tcpSocket.h"
#include"Data_Client.h"

bool init_Socket()
{
	//wsa  windows socket async windows�첽�׽���
	//parm1:����socket�İ汾�� 2.2 2.1 2.0   parm2:�������� 
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		cout << "WSAStartup failed code" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

bool close_Socket()
{
	if (0 != WSACleanup()) {
		cout << "WSACleanup failed code" << WSAGetLastError() << endl;
		return false;
	}
	return true;
	return false;
}

SOCKET creatClientSocket(const char* ip)
{
	//1��������Socket
	//parm1:af ��ַЭ���� ipv4 ipv6
	//parm2:type ����Э������ ��ʽ�׽��� ���ݰ�
	//parm3:protocl ʹ�þ����ĳ������Э��
	SOCKET cliSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cliSocket == INVALID_SOCKET) {
		cout << "�ͻ��˽���socketʧ��" << endl;
		return false;
	}
	//2�����������������
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 8888;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(cliSocket, (sockaddr*)&addr, sizeof(sockaddr_in)))
	{
		cout << "�ͻ������ӷ�����ʧ��" << endl;
		return false;
	}
	return cliSocket;
}


//�ͻ���ҵ������
int processor(SOCKET cliSocket)
{
	DataHead _rcvheader = {};//��������ͷ
	//���ܿͻ��˷��͵�����
	recv(cliSocket, (char*)&_rcvheader, sizeof(DataHead), 0);
	if (_rcvheader.dataLength <= 0)
	{
		/*cout << "�ͻ���" << inet_ntoa(socket_in.sin_addr) << " " << socket_in.sin_port << "���˳�\n";*/
		return -1;//�ͻ����˳�����-1
	}
	switch (_rcvheader.cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			//���ܷ�������Ӧ����
			LoginResult loginRet = {};//��¼���
			recv(cliSocket, (char*)&loginRet + sizeof(DataHead), sizeof(LoginResult) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "��������Ӧ��" << endl;
			cout << "��Ӧͷ��" << loginRet.cmd << "   ��Ӧ����:" << loginRet.dataLength << endl;
			cout << "��¼�����" << loginRet.result << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}

		case CMD_LOGOUT_RESULT:
		{
			//���ܷ�������Ӧ����
			LogoutResult logoutRet = {};//��¼���
			recv(cliSocket, (char*)&logoutRet + sizeof(DataHead), sizeof(LogoutResult) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "��������Ӧ��" << endl;
			cout << "��Ӧͷ��" << logoutRet.cmd << "   ��Ӧ����:" << logoutRet.dataLength << endl;
			cout << "�ǳ������" << logoutRet.result << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}
		case CMD_ERROR_RESULT: {
			//���ܷ�������Ӧ����
			ErrorResult errorRet = {};//��¼���
			recv(cliSocket, (char*)&errorRet + sizeof(DataHead), sizeof(ErrorResult) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "��������Ӧ��" << endl;
			cout << "��Ӧͷ��" << errorRet.cmd << "   ��Ӧ����:" << errorRet.dataLength << endl;
			cout << "������������" << errorRet.result << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}
		case CMD_NEWUSER_JOIN: {
			//���ܷ�������Ӧ����
			NewUser_Join newJoin;//��¼���
			recv(cliSocket, (char*)&newJoin+sizeof(DataHead), sizeof(NewUser_Join)-sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "���û����뵽�߳���" << endl;
			cout << "��Ӧͷ��" << newJoin.cmd << "   ��Ӧ����:" << newJoin.dataLength << endl;
			cout << "���û�ip:" << newJoin.ip << "   port:" << newJoin.port << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}
		default:
		{
			//���ܷ�������Ӧ����
			ErrorResult errorRet = {};//��¼���
			recv(cliSocket, (char*)&errorRet + sizeof(DataHead), sizeof(ErrorResult) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "��������Ӧ��" << endl;
			cout << "��Ӧͷ��" << errorRet.cmd << "   ��Ӧ����:" << errorRet.dataLength << endl;
			cout << "������������" << errorRet.result << endl;
			cout << "-------------------------------------------------" << endl;
			return 1;
		}
	}
}
