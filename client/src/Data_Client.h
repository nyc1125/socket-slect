#pragma once
#include"Cmd_Client.h"
//���ݰ�ͷ
struct DataHead
{
	short cmd;//����
	short dataLength;//���ݳ���
};
//���ݰ���
struct Login :public DataHead
{
	Login() {
		this->cmd = CMD_LOGIN;
		this->dataLength = sizeof(Login);
	}
	char userName[32];
	char password[32];
};
struct LoginResult :public DataHead
{
	LoginResult() {
		this->cmd = CMD_LOGIN_RESULT;
		this->dataLength = sizeof(LoginResult);
	}
	int result;
};
//�ǳ�
struct Logout :public DataHead
{
	Logout() {
		this->cmd = CMD_LOGOUT;
		this->dataLength = sizeof(Logout);
	}
	char userName[32];
};
struct LogoutResult :public DataHead
{
	LogoutResult() {
		this->cmd = CMD_LOGOUT_RESULT;
		this->dataLength = sizeof(LogoutResult);
	}
	int result;
};
//���û���������
struct NewUser_Join :public DataHead
{
	NewUser_Join() {
		this->cmd = CMD_NEWUSER_JOIN;
		this->dataLength = sizeof(NewUser_Join);
	}
	char ip[32];
	int port;
};
struct NewUser_Join_Result :public DataHead
{
	NewUser_Join_Result() {
		this->cmd = CMD_NEWUSER_JOIN_RESULT;
		this->dataLength = sizeof(NewUser_Join_Result);
	}
	char ip[32];
	int port;
};
struct Error :public DataHead
{
	Error() {
		this->cmd = CMD_ERROR;
		this->dataLength = sizeof(Error);
	}

	char userName[32];
};
struct ErrorResult :public DataHead
{
	ErrorResult() {
		this->cmd = CMD_ERROR_RESULT;
		this->dataLength = sizeof(ErrorResult);
	}
	char result[32] = "��������ȷ����";
};