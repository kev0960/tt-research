#include "Time.h"
#include "Datatypes.h"
#include "Load.h"
#include "ExamSchedule.h"
#include <iostream>
int main()
{
	vector<Subject *> subjects;
	vector<Student *> students;
	Load load("subject.txt", "timetable.csv", subjects, students);
	load.load();

	ExamSchedule exam(students, subjects, "exam.txt", 5); 
	exam.pin_subjects();
	exam.schedule();
}