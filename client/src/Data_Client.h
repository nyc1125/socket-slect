#pragma once
#include"Cmd_Client.h"
//数据包头
struct DataHead
{
	short cmd;//命令
	short dataLength;//数据长度
};
//数据包体
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
//登出
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
//新用户加入命令
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
	char result[32] = "请输入正确命令";
};