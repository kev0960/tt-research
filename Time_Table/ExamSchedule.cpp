#include "ExamSchedule.h"

void ExamSchedule::pin_subjects()
{
	for(auto i = 0; i < exam.size(); i ++)
	{
		// 일단 반드시 pin 을 해야 하는 시험 과목들의 
		// 수가 매우 적으므로 pin 되는 시험 시간표들을 처리하기
		// 위해서 따로 특별한 알고리즘을 사용할 필요 없이 간단히
		// 겹치지 않을 정도로만 처리해주면 된다. 
		if(exam[i]->pin() != -1)
		{
			ExamTime t(0, exam[i]->pin()); 
			while (t.day() < exam_days)
			{
				if(no_conflict(exam[i], t))
				{
					time_subject[t.n_th()].push_back(exam[i]);
					break;
				}
				t.inc_day();
			}
			if(t.day() == exam_days)
				throw Error::Error(PIN_ERROR); 
		}
	}
}

bool ExamSchedule::no_conflict(Subject *subject, ExamTime time)
{
	// 이 과목을 time 에 삽입하였을 때 겹치는 것이 있는지 확인한다. 
	for(int i = 0; i < time_subject[time.n_th()].size(); i ++)
	{
		ZERO.resize(subject->student_mask().size());
		if((subject->student_mask() = subject->student_mask() & time_subject[time.n_th()].at(i)->student_mask()) != ZERO)
		{
			return false;
		}
	}
	return true;
}