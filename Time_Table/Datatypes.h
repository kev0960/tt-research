#include <string>
#include <bitset>
#include <deque>
#include <vector>
using namespace std;

class Subject;
class Student;

class Student
{
	string _name;
	int _id; // 학생 id
	bitset<32> _available_time; // 7 + 7 + 4 + 7 + 7 교시에서 각각 가능한 시간에 0 이 된다.
	deque<Subject *> _subject_list;

public:
	Student(string name, int id) : _name(name), _id(id), _available_time(0) {}
	
	int id() { return _id; }
	string name() { return _name; }

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

public:

	// division 개수를 설정해야 한다.
	Subject(string name, int _max_std, int _min_std, int hour, int id, int teacher_avail, string partition) 
		: _name(name), _num_student(0), _num_division(0), _num_max_student(_max_std), _num_min_student(_min_std),
		_hour(hour), _id(id), _teacher_avail(teacher_avail), _partition(partition) {}

	void add_student(Student *std);

	inline string name() { return _name; }
	inline int id() { return _id; }
	inline int num_student() { return _num_student; }
	inline int num_division() { return _num_division; }
	inline int teacher_avail() { return _teacher_avail; }
	inline int hour() { return _hour; }
	inline string partition() { return _partition; }
	inline int num_max_student() { return _num_max_student; }
	inline int num_min_student() { return _num_min_student; }
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