#include "Datatypes.h"
#include <dynamic_bitset.hpp>
#include <functional>

#pragma once
// 순전히 시험 시간표를 제작하기 위한 클래스 이다.
class SubjectExam : public Subject
{
	int _priority;
	
	// 만일 특정 교시에 시험을 반드시 봐야 한다면 pin 값은 0 과 5 사이의 값이고
	// 그렇지 않다면 -1 이다. 
	int _pin; 
	
	// 시험 보는 시간
	ExamTime _exam_time; 

	// 과목 사이 관계 행렬을 통해 얻는 이 시험 과목의 중요도
	double _importance; 
	
	// 상보적인 과목을 가리키게 된다.
	int _paired;

	// 이미 추가된 과목
	bool _inserted; 

public:
	SubjectExam(string name, int max_std, int min_std, int hour, int id, int internal_id,
		int teacher_avail, string partition, int priority, int pin, int exam_days) 
		: Subject(name, max_std, min_std, hour, id, internal_id, teacher_avail, partition),
		_priority(priority), _pin(pin), _paired(-1), _inserted(false)
	{
	}

	inline int priority() const { return _priority; }
	inline int pin() const { return _pin; }
	inline double importance() const { return _importance; }
	inline int paired() const { return _paired; }
	inline bool inserted() const { return _inserted; }

	inline void set_importance(double importance) { _importance = importance; }
	inline void set_paired(int paired) { _paired = paired; }
	inline void set_inserted(bool inserted) { _inserted = inserted; }

	inline ExamTime exam_time() { return _exam_time; }
	void set_exam_time (ExamTime exam_time) 	{ _exam_time = exam_time; }

};

struct CompareSubjectExam : public std::binary_function<SubjectExam *, SubjectExam *, bool>
{
	bool operator() (const SubjectExam * lhs, const SubjectExam *rhs) const
	{
		return lhs->importance() < rhs->importance();
	}
};
