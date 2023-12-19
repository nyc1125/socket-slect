#ifndef _CELLTimestamp_hpp_
#define _CELLTimestamp_hpp_

#include <chrono>

using namespace std::chrono;

class CELLTimestamp
{
public:
	CELLTimestamp() {
		update();
	}

	//����һ�µ�ǰʱ��
	void update()
	{
		_begin = high_resolution_clock::now();
	}
	//��ȡ��ǰ��
	double getElapsedSecond() {
		return getElapsedTimeInMicroSec() * 0.000001;
	}
	//��ȡ����
	double getElapsedTimeInMilliSec() {
		return getElapsedTimeInMicroSec() * 0.001;
	}
	//��ȡ΢��
	long long getElapsedTimeInMicroSec() {
		//����ǰʱ���ȥ��ʼʱ�䣺high_resolution_clock::now() - _begin
		//Ȼ�󽫽��ת��Ϊ΢��microseconds��duration_cast����ǿ��ת��
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
	}
protected:
	time_point<high_resolution_clock> _begin;
};

#endif