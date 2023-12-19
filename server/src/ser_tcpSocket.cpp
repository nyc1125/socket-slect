#include "ser_tcpSocket.h"
#include"Data_Server.h"

bool init_Socket()
{
	//wsa  windows socket async windows�첽�׽���
	//parm1:����socket�İ汾�� 2.2 2.1 2.0   parm2:�������� 
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata)) {
		cout << "������socket����ʧ��" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

bool close_Socket()
{
	if (0 != WSACleanup()) {
		cout << "������socket�ر�ʧ��" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

SOCKET creatServerSocket()
{
	//1��������Socket
	//parm1:af ��ַЭ���� ipv4 ipv6
	//parm2:type ����Э������ ��ʽ�׽��� ���ݰ�
	//parm3:protocl ʹ�þ����ĳ������Э��
	SOCKET serSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serSocket == INVALID_SOCKET) {
		/*err("socket");*/
		cout << "������socket����ʧ��" << endl;
		return INVALID_SOCKET;
	}
	//2���󶨽��ܿͻ������ӵĶ˿�
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = 8888;
	addr.sin_addr.S_un.S_addr = ADDR_ANY;//inet_addr(ip);
	if (INVALID_SOCKET == bind(serSocket, (sockaddr*)&addr, sizeof(addr)))
	{
		cout << "�������󶨶˿�ʧ��" << endl;
	}
	else {
		cout << "�������󶨶˿ڳɹ�" << endl;
	}
	//��������˿�
	if (SOCKET_ERROR == listen(serSocket, 10)) {
		cout << "��������˿�ʧ��" << endl;
	}
	else {
		cout << "��������˿ڳɹ�" << endl;
	}
	return serSocket;
}

//�������
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
		case CMD_LOGIN:
		{
			//���ܿͻ��˷��͵�������
			Login login = {};
			recv(cliSocket, (char*)&login+sizeof(DataHead), sizeof(Login) - sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "�������CMD_LOGIN"  << "   ���ݳ��ȣ�" << login.dataLength << endl;
			cout << "�����û�����" << login.userName<< endl;
			cout << "��      �룺" << login.password << endl;
			cout << "-------------------------------------------------" << endl;

			//�ж��˻��������Ƿ���ȷserSocket
			//�˴��ж��߼�

			//��Ӧ�ͻ�������
			LoginResult loginResult = {};
			loginResult.result = 1;
			loginResult.cmd = CMD_LOGIN_RESULT;//201
			loginResult.dataLength = sizeof(LoginResult);//32
			send(cliSocket, (const char*)&loginResult, sizeof(loginResult), 0);//��Ӧ��Ϣ
			return 1;
		}

		case CMD_LOGOUT:
		{
			//���ܿͻ��˷��͵�����
			Logout logout = {};
			recv(cliSocket, (char*)&logout+ sizeof(DataHead), sizeof(Logout)- sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "�������CMD_LOGOUT"  << "   ���ݳ��ȣ�" << _rcvheader.dataLength << endl;
			cout << "�����û�����" << logout.userName << endl;
			cout << "-------------------------------------------------" << endl;

			//��Ӧ�ͻ�������
			LogoutResult logoutResult = {};
			logoutResult.result = 1;
			logoutResult.cmd = CMD_LOGOUT_RESULT;
			logoutResult.dataLength = sizeof(LogoutResult);
			send(cliSocket, (const char*)&logoutResult, sizeof(logoutResult), 0);//��Ӧ��Ϣ
			return 1;
		}
		case CMD_ERROR: {
			//���ܿͻ��˷��͵�����
			Error error = {};
			recv(cliSocket, (char*)&error+ sizeof(DataHead), sizeof(Error)- sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			cout << "�������CMD_ERROR" << _rcvheader.cmd << "   ���ݳ��ȣ�" << _rcvheader.dataLength << endl;
			cout << "�����û�����" << error.userName << endl;
			cout << "-------------------------------------------------" << endl;

			ErrorResult errorResult = {};
			errorResult.result;
			errorResult.cmd = CMD_ERROR;
			errorResult.dataLength = sizeof(ErrorResult);
			send(cliSocket, (const char*)&errorResult, sizeof(errorResult), 0);//��Ӧ��Ϣ
			return 1;
		}

		default:
		{
			//���ܿͻ��˷��͵�����
			Error error = {};
			recv(cliSocket, (char*)&error+ sizeof(DataHead), sizeof(Error)- sizeof(DataHead), 0);
			cout << "-------------------------------------------------" << endl;
			/*cout << "�ͻ���" << inet_ntoa(socket_in.sin_addr) << " " << socket_in.sin_port << "��������" << endl;*/
			cout << "�������CMD_ERROR" << _rcvheader.cmd << "   ���ݳ��ȣ�" << _rcvheader.dataLength << endl;
			cout << "�����û�����" << error.userName << endl;
			cout << "-------------------------------------------------" << endl;

			ErrorResult errorResult = {};
			errorResult.result;
			errorResult.cmd = CMD_ERROR;
			errorResult.dataLength = sizeof(ErrorResult);
			send(cliSocket, (const char*)&errorResult, sizeof(errorResult), 0);//��Ӧ��Ϣ
			return 1;
		}

	}
}

