#include "Datatypes.h"

void Subject::add_student(Student *student)
{
	_student_list.push_back(student);
}

void Subject::set_student_mask(vector<Student *>& students)
{
	_student_mask.resize(students.size()); // 각 학생 별로 한 개의 비트에 설정된다.
	for (deque<Student *>::iterator itr = _student_list.begin(); itr != _student_list.end(); itr ++)
	{
		boost::dynamic_bitset<> one(students.size(), static_cast<unsigned long>(1));
		one <<= (*itr)->internal_id(); 
			
		_student_mask |= one; 
	}
}
void Subject::set_division()
{
	_num_student = _student_list.size();
	_num_division = _num_student / 15;
	if(_num_division == 0) _num_division ++;
	else if (_num_division == _num_student / _num_max_student) _num_division ++;
}