//命令的枚举
enum CMD
{
	CMD_LOGIN = 1,//登录
	CMD_LOGIN_RESULT = 201,//登录结果
	CMD_LOGOUT = 2,//登出
	CMD_LOGOUT_RESULT = 202,
	CMD_NEWUSER_JOIN = 3,
	CMD_NEWUSER_JOIN_RESULT = 203,
	CMD_EXIT = 4
};

//数据包头
struct DataHead
{
	DataHead()
	{
		this->cmd = CMD_EXIT;
		this->dataLength = 0;
	}
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
	char data[956];
};
struct LoginResult :public DataHead
{
	LoginResult() {
		this->cmd = CMD_LOGIN_RESULT;
		this->dataLength = sizeof(LoginResult);
	}
	int result;
	char ret_char[92];
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
struct Exit :public DataHead
{
	Exit() {
		this->cmd = CMD_EXIT;
		this->dataLength = sizeof(Exit);
	}
	int result = 1;
};