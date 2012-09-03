#include "SubjectExam.h"
#include <fstream>


class ExamSchedule
{
	vector<Student*> &students;
	vector<Subject*> &subjects; 

	vector<SubjectExam *> exam;

	vector<vector<Subject*>> time_subject; // 특정 시간대에 어떤 시험을 보는지 
	const int exam_days; // 시험 총 일수

public:
	ExamSchedule(vector<Student*> &students, vector<Subject*> &subjects, string exam_file, int exam_days)
		: students(students), subjects(subjects), exam_days(exam_days)
	{
		ifstream in(exam_file);

		for(auto i = 0; i < subjects.size(); i ++)
		{
			if(in.bad())
				break;

			int priority, pin;
			in >> priority >> pin;

			exam.push_back(new SubjectExam(
				subjects[i]->name(), subjects[i]->num_max_student(), subjects[i]->num_min_student(),
				subjects[i]->hour(), subjects[i]->id(), subjects[i]->teacher_avail(),subjects[i]->partition(),
				priority, pin, exam_days));	
		}

		time_subject.reserve(exam_days * 5); // 미리 공간을 예약해놓는다. 
	}

	void schedule();

	// 시험 시간이 고정되어 있는 것 부터 먼저 처리한다. 
	void pin_subjects(); 

	// 시간표가 겹치는지 check 한다.
	// 만일 겹치는 것이 없다면 true 를 리턴한다. 
	bool no_conflict(Subject *subject, ExamTime time);
};

