#include "Datatypes.h"

void Subject::add_student(Student *student)
{
	_student_list.push_back(student);
}