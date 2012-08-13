#include "SubjectExam.h"
#include <fstream>

class ExamSchedule
{
	vector<Student*> &students;
	vector<Subject*> &subjects; 

	vector<SubjectExam *> exam;

	const int  exam_days; // 시험 총 일수

public:
	ExamSchedule(vector<Student*> &students, 	vector<Subject*> &subjects, string exam_file, int exam_days)
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

		boost::dynamic_bitset<> exam_time (exam_days * 6); 

		for(auto i = 0; i < exam.size(); i ++)
			exam[i]->set_exam_time(exam_time);
	}

	void schedule();

	// 시험 시간이 고정되어 있는 것 부터 먼저 처리한다. 
	void pin_subjects(); 
};

