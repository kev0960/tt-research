#include <string>
#include <bitset>
#include <deque>
#include <vector>
#include <dynamic_bitset.hpp>
#include "Error.h"

using namespace std;

class Subject;
class Student;

extern boost::dynamic_bitset<> ZERO;
extern boost::dynamic_bitset<> ONE;

class ExamTime
{
	int time; // 몇 일 몇 교시인지 나타낸다.

public:
	// day 번째 hour 교시 (1 교시면 0, 1 째 날이면 0)
	ExamTime(int day, int hour)
	{
		time = day * 10 + hour; 
	}
	ExamTime(ExamTime &_time)
	{
		time = _time.time;
	}
	ExamTime(int x = 0) { time = x; }

	inline int hour() { return (time % 10); }
	inline int day() { return (time / 10); }
	inline int hour(int hour) { time = 10 * (time / 10) + hour; }
	inline int day(int day) { time = time % 10 + day * 10; }

	// 배열에 선형으로 시간표 배열시 몇 번째에 위치하는지 나타낸다.
	inline int n_th() { return 5 * day() + hour(); }

	void inc_hour()
	{
		// 4 교시 (3) 다음에 다음날 1교시로 넘어간다. 
		if(this->hour() < 3)
		{
			time = time + 10;
			time = 10 * (time / 10);
		}
		else
			time ++;
	}
	void inc_day() { time += 10; }

	bool operator==(ExamTime &_time) { return time == _time.time; }
	bool operator!= (ExamTime &_time) { return time != _time.time; }
	ExamTime& operator= (ExamTime &_time) 
	{ 
		time = _time.time;
		return (*this);
	}
};

class Student
{
	string _name;
	int _id; // 학생 id
	int _internal_id; // student 배열에서 몇 번째로 위치해있는지

	bitset<32> _available_time; // 7 + 7 + 4 + 7 + 7 교시에서 각각 가능한 시간에 0 이 된다.
	deque<Subject *> _subject_list;

	boost::dynamic_bitset<> _subject_mask;
public:
	Student(string name, int id, int internal_id) : _name(name), _id(id), _available_time(0), _internal_id(internal_id) {}
	
	inline int id() { return _id; }
	inline int internal_id() { return _internal_id; }
	inline string name() { return _name; }
	inline boost::dynamic_bitset<> subject_mask() { return _subject_mask; }

	void set_subject_mask(); 

	// 화요일 3교시면 time 은 12 가 된다.
	void set_time(int time);
	void set_time(bitset<32> &time);

	inline deque<Subject *>& subject_list() { return _subject_list; }

	// 이 시간대가 가능한 것인가?
	bool time_avail(int time);
};

class Subject
{
protected:
	
	string _name;
	int _id; // 과목 id

	const int _num_max_student; // 최대 학생수
	const int _num_min_student; // 최소 학생수

	int _num_student; // 전체 학생수
	int _num_division;  // 분반 개수

	int _teacher_avail; // 가능한 선생님 수
	deque<Student *> _student_list;

	int _hour; // 수업 전체 시수
	string _partition; // 시간 분할이 어떻게 되는지

	boost::dynamic_bitset<> _student_mask; // 학생 정보 가지는 hash

public:

	// division 개수를 설정해야 한다.
	Subject(string name, int _max_std, int _min_std, int hour, int id, int teacher_avail, string partition) 
		: _name(name), _num_student(0), _num_division(0), _num_max_student(_max_std), _num_min_student(_min_std),
		_hour(hour), _id(id), _teacher_avail(teacher_avail), _partition(partition) {}

	void add_student(Student *std);
	void set_student_mask(vector<Student*>& students);

	inline string name() { return _name; }
	inline int id() { return _id; }
	inline int num_student() { return _num_student; }
	inline int num_division() { return _num_division; }
	inline int teacher_avail() { return _teacher_avail; }
	inline int hour() { return _hour; }
	inline string partition() { return _partition; }
	inline int num_max_student() { return _num_max_student; }
	inline int num_min_student() { return _num_min_student; }
	inline boost::dynamic_bitset<> student_mask() { return _student_mask; }
	virtual inline bitset<32> teaching_time() { return 0; }

	// 두 분반 사이에 겹치는 시간수를 알린다. 
	virtual int count_overlap(Subject *subject) { return 0; }

	// 두 분반 사이에 겹치는 시간을 얻어온다. 
	virtual bitset<32> time_overlap (Subject *subject) { return 0; }
};

class SubjectClass : public Subject
{
	bitset<32> _teaching_time;
	int _class_id; // 분반 번호

public:

	SubjectClass(string name, int _max_std, int _min_std, int hour, int id, int class_id, int teacher_avail, string partition) 
		: Subject(name, _max_std, _min_std, hour, id, teacher_avail, partition), _class_id(class_id)
	{
	}
	SubjectClass(Subject &subject, int class_id, int _num_division) : Subject(subject.name(), subject.num_max_student(), 
		subject.num_min_student(), subject.hour(), subject.id(), subject.teacher_avail(), subject.partition()), _class_id(class_id)
	{
	}

	inline bitset<32> teaching_time() { return _teaching_time; }
	inline int class_id() { return _class_id; }

	int count_overlap(Subject *subject)
	{
		bitset<32> and = _teaching_time & subject->teaching_time(); 

		// AND 연산을 했을 때 0 이 아닌 비트가 바로 두 과목이 겹치는 시간 수 이다.
		return static_cast<int> ( and.count() );
	}
	bitset<32> time_overlap(Subject *subject)
	{
		return _teaching_time & subject->teaching_time();
	}	 
};