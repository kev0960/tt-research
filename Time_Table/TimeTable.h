#include "Datatypes.h"
#pragma once

class TimeTable
{
	vector<Subject *> &subjects;
	vector<Student *> &students;
	vector<Subject *> subject_class;

public:
	TimeTable (vector<Subject *> &subjects, vector<Student *> &students) 
		: subjects(subjects), students(students) {}


};