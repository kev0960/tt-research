#include "Datatypes.h"

// 예를 들어 화요일 3교시면 time 이 12 가 된다. 
void Student::set_time(int time)
{
	bitset<32> one (1);

	switch(time / 10)
	{
	case 0:
		one <<= (time % 10);
		break;
	case 1:
		one <<= (time % 10 + 7);
		break;
	case 2:
		one <<= (time % 10 + 14);
		break;
	case 3:
		one <<= (time % 10 + 18);
		break;
	case 4:
		one <<= (time % 10 + 25);
		break;
	}
	_available_time |= one;
}
void Student::set_time(bitset<32> &time)
{
	_available_time |= time;
}

// 이 시간대가 가능한 것인지 체크해본다.
bool Student::time_avail(int time)
{
	bitset<32> one (1);

	switch(time / 10)
	{
	case 0:
		one <<= (time % 10);
		break;
	case 1:
		one <<= (time % 10 + 7);
		break;
	case 2:
		one <<= (time % 10 + 14);
		break;
	case 3:
		one <<= (time % 10 + 18);
		break;
	case 4:
		one <<= (time % 10 + 25);
		break;
	}

	if((_available_time & one) != 0)
		return false;

	return true; 
}