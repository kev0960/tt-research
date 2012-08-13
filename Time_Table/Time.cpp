#include "Time.h"

void set_time(bitset<32> &t, int time)
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
	t |= one;
}
bool time_avail(bitset<32> &t, int time)
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

	if((t & one) != 0)
		return false;

	return true; 
}

// t 에 time 을 배정할 수 있는지 확인한다. 
inline bool time_avail(bitset<32> &t, bitset<32> time)
{
	if((t & time) != bitset<32>(0))
		return false;
	return true;
}

// 교시를 1 칸 씩 미룬다. 즉, 12 교시를 23 교시로.
// 이 때 34 교시는 56 교시로 미뤄지고 67 교시는 12 교시로 미뤄진다. 
void next_time (bitset<32> &time)
{
	if(time.count() == 1)
	{
		time = time << 1;  // 왼쪽으로 1 쉬프트

		// 금요일 7 교시에 월요일 1 교시로 넘어간다. 
		if(time == bitset<32>(0))
			time = 1;
	}
	else if(time.count() == 2)
	{
		switch (start_bit(time))
		{
		case 2:
		case 5:
		case 9:
		case 12:
		case 16:
		case 20:
		case 23:
		case 27:
			time = time << 2;
			break;

		case 30:
			time = bitset<32>("11"); // 000...011 이 된다.
			break;

		default:
			time = time << 1;
		}
	}
	else if(time.count() == 3)
	{
		switch(start_bit(time))
		{
		case 0:
		case 7:
		case 14:
		case 18:
		case 25:
			time = time << 1;
			break;

		case 29:
			time = bitset<32>("111");
			break;

		default:
			time = time << 3;
			break;
		}
	}
}

// time 이 시작하는 첫 교시를 찾는다. 
// *source from*
// http://stackoverflow.com/questions/757059/position-of-least-significant-bit-that-is-set

int start_bit(bitset<32> time)
{
	// ulong 이 언제나 32 비트로 리턴하는지 확인!

	unsigned int v = time.to_ulong();  // find the number of trailing zeros in 32-bit v 
	static const int MultiplyDeBruijnBitPosition[32] = 
	{
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	return MultiplyDeBruijnBitPosition[((uint32_t)((v & -v) * 0x077CB531U)) >> 27];

}