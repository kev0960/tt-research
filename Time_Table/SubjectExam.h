#include "Datatypes.h"
#include <dynamic_bitset.hpp>

// 순전히 시험 시간표를 제작하기 위한 클래스 이다.
class SubjectExam : public Subject
{
	int _priority;
	
	// 만일 특정 교시에 시험을 반드시 봐야 한다면 pin 값은 0 과 5 사이의 값이고
	// 그렇지 않다면 -1 이다. 
	int _pin; 
	
	// 시험 보는 시간
	ExamTime _exam_time; 

public:
	SubjectExam(string name, int max_std, int min_std, int hour, int id, int teacher_avail, string partition, int priority, int pin, int exam_days) 
		: Subject(name, max_std, min_std, hour, id, teacher_avail, partition), _priority(priority), _pin(pin)
	{
	}

	inline int priority() { return _priority; }
	inline int pin() { return _pin; }
	inline ExamTime exam_time() { return _exam_time; }
	void set_exam_time (ExamTime exam_time) 	{ _exam_time = exam_time; }

};