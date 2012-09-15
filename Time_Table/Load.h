#include "Datatypes.h"
#include <iostream>
#include <fstream>
#include <dynamic_bitset.hpp>
#pragma once

class Load
{
	string subject_file, student_file;
	vector<Subject *> &subjects;
	vector<Student *> &students;
		
public:
	Load(string subject_file, string student_file, vector<Subject *> &subjects, vector<Student *> &students )
		: subject_file(subject_file), student_file(student_file), subjects(subjects), students(students) {}

	void load_students_csv();
	void load_subjects_csv();
	void load() 
	{
		load_subjects_csv();
		load_students_csv();

		for(int i = 0; i != subjects.size(); i ++) {
			subjects[i]->set_student_mask(students);
			subjects[i]->set_division();
		}
		for(int i = 0; i != students.size(); i ++)
			students[i]->set_subject_mask(subjects);
	}
};