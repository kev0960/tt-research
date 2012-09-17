#include "SubjectExam.h"
#include <fstream>

#pragma once

class ExamSchedule
{
	vector<Student*> &students;
	vector<Subject*> &subjects; 

	vector<SubjectExam *> exam;

	vector<vector<SubjectExam*>> time_subject; // 특정 시간대에 어떤 시험을 보는지 
	const int exam_days; // 시험 총 일수

public:
	ExamSchedule(vector<Student*> &students, vector<Subject*> &subjects, string exam_file, int exam_days)
		: students(students), subjects(subjects), exam_days(exam_days)
	{
		ifstream in(exam_file);

		for(vector<Subject*>::size_type i = 0; i < subjects.size(); i ++)
		{
			if(in.bad())
				break;

			int priority, pin, is_test;
			in >> priority >> pin >> is_test;

			if(!is_test) continue;
			exam.push_back(new SubjectExam(
				subjects[i]->name(), subjects[i]->num_max_student(), subjects[i]->num_min_student(),
				subjects[i]->hour(), subjects[i]->id(), subjects[i]->internal_id(), subjects[i]->teacher_avail(),subjects[i]->partition(),
				priority, pin, exam_days));
			exam[exam.size() - 1]->set_student_mask(subjects[i]->student_mask());
			exam[exam.size() - 1]->set_student_list(subjects[i]->student_list());
			exam[exam.size() - 1]->set_division();
		}

		time_subject.resize(exam_days * MAX_CLASS_TIME); // 미리 공간을 예약해놓는다. 
	}

	void schedule();
	void insert_timetable(SubjectExam *subject); 

	int max_priority(ExamTime day, SubjectExam *subject); // day 날에 시험보는 학생들 중 priority 최대값을 리턴한다.
	double average_priority(ExamTime day, SubjectExam *subject, double factor); // day 날에 시험보는 학생들 중 subject 삽입시 평균 priority
	
	void complementary_pairs(); // 학생이 서로 안 겹치는 두 과목에 대해 priority 가 큰 쌍들을 찾는다. 

	// 시험 시간이 고정되어 있는 것 부터 먼저 처리한다. 
	void pin_subjects(); 

	// 시간표가 겹치는지 check 한다.
	// 만일 겹치는 것이 없다면 true 를 리턴한다. 
	bool no_conflict(Subject *subject, ExamTime time);

	// 두 과목의 겹치는 학생 수를 계산한다.
	int count_conflict(Subject *subject1, Subject *subject2);
};

