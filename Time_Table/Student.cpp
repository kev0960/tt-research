#include "Datatypes.h"
boost::dynamic_bitset<> ZERO(1, static_cast<unsigned long>(0));
boost::dynamic_bitset<> ONE(1, static_cast<unsigned long>(1));
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

// 과목 mask 를 설정한다
void Student::set_subject_mask()
{
	_subject_mask.resize(_subject_list.size()); // 각 과목을 포함하는지에 대한 유무를 알고있음
	for(deque<Subject*>::iterator itr = _subject_list.begin(); itr != _subject_list.end(); itr ++)
	{
		boost::dynamic_bitset<> one(_subject_list.size(), static_cast<unsigned long>(1));

		/***************************************
		* 참고로 과목 코드는 입력 받는 과목 순서와 동일해야한다!*
		* 즉 과목 코드는 0 부터 시작한다                              *
		***************************************/

		one << ((*itr)->id()); 
		_subject_mask |= one; 
	}
}
