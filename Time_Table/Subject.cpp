#include "Datatypes.h"

void Subject::add_student(Student *student)
{
	_student_list.push_back(student);
}

void Subject::set_student_mask(vector<Student *>& students)
{
	_student_mask.resize(students.size()); // 각 학생 별로 한 개의 비트에 설정된다.
	for (vector<Student *>::iterator itr = students.begin(); itr != students.end(); itr ++)
	{
		boost::dynamic_bitset<> one(students.size(), static_cast<unsigned long>(1));
		one <<= (*itr)->internal_id(); 

		_student_mask |= one; 
	}
}