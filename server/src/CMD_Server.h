#pragma once
//�����ö��
enum CMD
{
	CMD_LOGIN = 1,//��¼
	CMD_LOGIN_RESULT = 201,//��¼���
	CMD_LOGOUT = 2,//�ǳ�
	CMD_LOGOUT_RESULT = 202,
	CMD_NEWUSER_JOIN = 3,
	CMD_NEWUSER_JOIN_RESULT = 203,
	CMD_ERROR = 0,//�������
	CMD_ERROR_RESULT = 400
};