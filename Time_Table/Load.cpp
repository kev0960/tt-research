#include "Load.h"

// csv  파일로 부터 학생 정보를 받아온다
void Load::load_students_csv()
{
	ifstream in(student_file);

	string temp;
	while (in.good())
	{
		// 그냥 의미없는 번호
		getline(in, temp, ',');

		if(temp.length() == 0)
			break;

		// 학생이 듣는 과목 코드를 얻는다.
		getline(in, temp, ',');
		int subj_id = atoi(temp.c_str());

		// 과목명 얻는다 (의미없음)
		getline(in, temp, ',');

		// 학생의 번호 얻는다.
		getline(in, temp, ',');
		int student_id = atoi(temp.c_str());

		// 학생의 이름 얻는다 
		getline(in, temp, ',');
		string student_name = temp;

		// 만일 이미 등록된 학생이라면
		bool registered = false;
		for(int i = 0; i < students.size(); i ++)
		{
			if(students[i]->id()== student_id)
			{
				for(int j = 0; j < static_cast<int>(subjects.size()); j ++)
				{
					if(subjects[i]->id() == subj_id)
					{
						students[i]->subject_list().push_back(subjects[j]);
						subjects[j]->add_student(students[i]);
						registered = true;
						break;
					}
				}
			}
		}
		// 등록되지 않았다면 새로 등록해준다.

		if(!registered)
		{
			Student *std = new Student(student_name, student_id);
			for(int j = 0; j < subjects.size(); j ++)
			{
				if(subjects[j]->id() == subj_id)
				{
					std->subject_list().push_back(subjects[j]);
					subjects[j]->add_student(std);
					break;
				}
			}

			students.push_back(std);
		}
	}

	in.close();
}

// csv 파일로 부터 과목 정보를 받아온다.
void Load::load_subjects_csv()
{
	ifstream in(subject_file);

	string temp;
	while (in.good())
	{
		// 과목 코드 얻는다.
		getline(in, temp, ','); 
		int code = atoi(temp.c_str()); 

		if(temp.length() == 0)
			break;

		// 과목 이름 얻는다.
		getline(in, temp, ',');
		string name = temp;

		// 최소 학생 수
		getline(in, temp, ',');
		int min_student = atoi(temp.c_str());

		// 최대 학생수
		getline(in, temp, ',');
		int max_student = atoi(temp.c_str());

		// 선생님 가용 수
		getline(in, temp, ',');
		int teacher = atoi(temp.c_str());

		// 수업 시간 수
		getline(in, temp, ',');
		int hour = atoi(temp.c_str());

		// 시간 분할 방법
		getline(in, temp, ',');
		string str = temp;

		subjects.push_back(new Subject(name, max_student, min_student, hour, code, teacher, str));
	}

	in.close();
}