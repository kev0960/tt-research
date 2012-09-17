#include "ExamSchedule.h"
#include <queue>
#include <utility>
#include <map>
#include <cmath>

// Matrix 라이브러리를 사용하기 위한 헤더파일
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>

void remove_ith_from_back(priority_queue<SubjectExam *, std::vector<SubjectExam *>, CompareSubjectExam> &queue, SubjectExam *s);

int ExamSchedule::count_conflict(Subject *subject1, Subject *subject2)
{
	boost::dynamic_bitset<> temp(subject1->student_mask() & subject2->student_mask());
	return temp.count(); // temp 에서 켜진 bit 수를 반환한다. (즉 이 것이 겹치는 학생 수)
}
void ExamSchedule::schedule()
{
	priority_queue<SubjectExam *, std::vector<SubjectExam *>, CompareSubjectExam> subject_exams;  // 추후에 importance 순으로 정렬하게 된다.
	using namespace boost::numeric::ublas;

	// 이 행렬의 eigenvalue 가 1 인 eigenvector 의 값으로 exam 들의 importance 를 결정
	matrix<double> exam_rel_matrix(exam.size(), exam.size()); 

	for(unsigned i = 0; i < exam_rel_matrix.size1(); i ++) {
		for(unsigned j = 0; j < exam_rel_matrix.size2(); j ++) {
			if(i == j) {
				exam_rel_matrix(i, j) = 0;
				continue; 
			}
			exam_rel_matrix(i, j) = (exam[i]->priority() + exam[j]->priority()) * (count_conflict(exam[i], exam[j]));
			exam_rel_matrix(i, j) ++; // 고립된 그래프들이 생성되지 않게 하기 위해서다. 
		}
	}

	for(unsigned i = 0; i < exam_rel_matrix.size2(); i ++) {
		// i 열의 성분들을 정규화 시킨다. 
		double sum = 0;
		for(unsigned j = 0; j < exam_rel_matrix.size1(); j ++) {
			sum += exam_rel_matrix(j, i);
		}

		for(unsigned j = 0; j < exam_rel_matrix.size1(); j ++) {
			exam_rel_matrix(j , i) /= sum;
		}
	}

	// 정규화가 완료 되었으면 Power Method 를 통해서 Eigenvalue 가 1 인 것의 Eigenvector 를 구한다. 
	boost::numeric::ublas::vector<double> eigen_vector(exam.size()); 
	for(unsigned i = 0; i < eigen_vector.size(); i ++) {
		eigen_vector(i) = static_cast<double>(1) / exam.size();
	}

	// Power Method 시작. 5회 정도만 반복 시행한다.
	for(int i = 0; i < 20; i ++) 	{
		eigen_vector = prod(exam_rel_matrix, eigen_vector);
	}

	for(unsigned i = 0; i < eigen_vector.size(); i ++)
		exam[i]->set_importance(eigen_vector(i));

	// importance scale 을 모두 매겼다면 priority_queue 에 삽입해서 큰 순으로
	// 나중에 빼게 된다. 
	for(int i = 0; i != exam.size(); i ++)
		subject_exams.push(exam[i]); 

	complementary_pairs();
	int i = 0;

	std::vector<SubjectExam*> in_priority;
	while (!subject_exams.empty()){
		// 가장 priority 가 큰 과목들 순으로 timetable 에 삽입하게 된다. 
		SubjectExam *subj = subject_exams.top();
		if(subj->inserted()) {
			subject_exams.pop();
			continue;
		}

		cout << subj->name() << " " << subj->importance() << " " << subj->num_student() << endl;
		
		try {
			insert_timetable(subj);
		} catch(Error::Error e) {
			e.print_error();

			subj->set_error_occured(true);
			
			// 만일 삽입이 불가능 하였을 경우 importance 값을 변경해서 이전에 
			// 삽입된 시간표 보다 먼저 삽입되도록 해보자.
			int pos = 1;
			for(; pos <= in_priority.size(); pos ++) {
				if(in_priority[in_priority.size() - pos]->error_occured() == false) {
					break;
				}
			}

			if( pos > in_priority.size() )
				throw Error::Error(NO_AVAIL_CLASS_SWAP_ERROR);

			double imp_temp = in_priority[in_priority.size() - pos]->importance();
			in_priority[in_priority.size() - pos]->set_importance(subj->importance());
			in_priority[in_priority.size() - pos]->set_inserted(false);
			time_subject[in_priority[in_priority.size() - pos]->exam_time().n_th()].pop_back();

			subj->set_importance(imp_temp);
			subject_exams.push(subj);
			subject_exams.push(in_priority[in_priority.size() - pos]);

			in_priority.erase(in_priority.begin() + in_priority.size() - pos);

			continue; 
		}

		// 만일 상보적인 과목이 있을 경우 이 과목은 지금 이 시간표와 같은 시간에
		// 가능하다면 집어 넣어버린다. 즉, paired 가 있을 경우
		if(subj->paired() != -1) {
			// 만일 그 시간표에 학생 conflict 가 없다면
			if(no_conflict(exam[subj->paired()], subj->exam_time())
				 && max_priority(subj->exam_time(), exam[subj->paired()]) < MAX_PRIORITY) {
				// 그리고 만일 그 시간표에 교실 개수가 충분하다면
				int class_num = 0;
				for(int i = 0; i != time_subject[subj->exam_time().n_th()].size(); i ++) {
					class_num += time_subject[subj->exam_time().n_th()][i]->num_division();
				}
				class_num += exam[subj->paired()]->num_division();

				if(class_num < MAX_CLASS_AVAIL) {
					time_subject[subj->exam_time().n_th()].push_back(exam[subj->paired()]);
					exam[subj->paired()]->set_exam_time(subj->exam_time());
					exam[subj->paired()]->set_inserted(true);
				}
			}
		}

		cout << subj->exam_time() << endl;

		// 처리한 과목 순서대로 벡터에 삽입한다.
		in_priority.push_back(subj);

		subject_exams.pop(); 
	}

	// 완성된 시간표를 출력해서 보여준다.
	ExamTime day(0);
	while (day.day() < exam_days) {
		for(int i = 0; i != time_subject[day.n_th()].size(); i ++) {
			cout << day << " " << time_subject[day.n_th()][i]->name() << endl;
		}
		day.inc_hour();
	}

	int total = 0;
	for(int i = 0; i < exam.size(); i ++) {
		if(exam[i]->inserted())
			total ++;
	}
	cout << total << " / " << exam.size() << endl;
}
void remove_ith_from_back(priority_queue<SubjectExam *, std::vector<SubjectExam *>, CompareSubjectExam> &queue, SubjectExam *s)
{
	deque<SubjectExam *> datas;
	for(;;) {
		if(queue.top() == s) {
			queue.pop();
			break;
		}
		datas.push_back(queue.top());
		queue.pop();
	}
	// s 직전 까지 것들이 pop 되었다. 

	for(int i = 0; i < datas.size(); i ++) {
		queue.push(datas.back());
		datas.pop_back();
	}
}
void ExamSchedule::insert_timetable(SubjectExam *subject)
{
	// 일단 이 과목이 들어갈 수 있는 날짜를 살펴보자
	ExamTime day(0); // 첫째날 
	vector<ExamTime> avail_days; 

	// 사용 가능한 날짜를 광범위하게 살펴보고
	while (day.day() < exam_days) {
		if(max_priority(day, subject) < MAX_PRIORITY)
			avail_days.push_back(day);

		day.inc_day();
	}

	if(avail_days.empty())
		throw Error::Error(NO_AVAIL_DAY_EXAM_ERROR);

	// 사용 가능한 교시를 정확히 찾아본다.
	vector<ExamTime> avail_times; 
	for(int i = 0; i != avail_days.size(); i ++) {
		for(day = avail_days[i]; day.day() <= avail_days[i].day(); day.inc_hour()) {
			if(no_conflict(subject, day))
				avail_times.push_back(day);
		}
	}

	if(avail_times.empty())
		throw Error::Error(NO_AVAIL_TIMESLOT_EXAM_ERROR);

	// 이제 그 사용 가능한 교시들 중에서 교실 수가 맞아서 time_slot 에 삽입할
	// 수 있는 과목을 찾는다.

	map<ExamTime, int> time_total_class; // 특정 time 에 과목을 추가하였을 때 차지하는 교실 수
	for(int i = 0; i != avail_times.size();  i++) {
		// avail_time 교시에 보는 과목들 교실 수 총합을 구한다.
		int sum_class = 0;
		for(auto itr = time_subject[avail_times[i].n_th()].begin(); itr != time_subject[avail_times[i].n_th()].end(); itr ++)
			sum_class += (*itr)->num_division();

		if(sum_class + subject->num_division() < MAX_CLASS_AVAIL)
			time_total_class[avail_times[i]] = sum_class + subject->num_division();
	}

	if(time_total_class.empty())
		throw Error::Error(NO_AVAIL_CLASS_EXAM_ERROR);

	double max_average = -1;
	int max_class_taken = 0;
	bool zero_priority_enable = false;

	ExamTime max_time = ExamTime(0);

	cout << " Num options : " << time_total_class.size() << endl;
	for(map<ExamTime, int>::iterator itr = time_total_class.begin(); itr != time_total_class.end(); itr ++) {
		double average = average_priority(itr->first, subject, 1 - 5.5 / static_cast<double>(time_total_class.size()) );

		if(!zero_priority_enable && average == 0) {
			zero_priority_enable = true;
			max_class_taken = itr->second;
			max_time = itr->first;
			continue; 
		}
		else if(zero_priority_enable && average == 0) {
			if(itr->second > max_class_taken) {
				max_class_taken = itr->second;
				max_time = itr->first;
			}
			continue;
		}

		double temp = (sqrt(static_cast<double>(itr->second)) / average);
		if(!zero_priority_enable && (sqrt(static_cast<double>(itr->second)) / average) > max_average) {
			max_average = (sqrt(static_cast<double>(itr->second)) / average); 
			max_time = itr->first;
		}
	}

	time_subject[max_time.n_th()].push_back(subject);
	subject->set_exam_time(max_time);
	subject->set_inserted(true);
}
int ExamSchedule::max_priority(ExamTime day, SubjectExam *subject)
{
	time_subject[day.n_th()].push_back(subject);
	// day 날에 시험보는 학생들 중 priority 최대값을 구한다. 
	int max = 0;
	map<Student*, int> student_priority; 

	ExamTime today = day;
	day.hour(0);
	while (day.day() == today.day()) {
		for(int i = 0; i != time_subject[day.n_th()].size(); i ++) {
			// 각 시간대에 포함되어 있는 과목들에 대해서 학생들에 해당하는 것들을 처리 
			for(int j = 0; j != time_subject[day.n_th()][i]->student_list().size(); j ++) {
				if(student_priority.find(time_subject[day.n_th()][i]->student_list().at(j)) == student_priority.end()) {
					student_priority[time_subject[day.n_th()][i]->student_list().at(j)] = 
						( time_subject[day.n_th()][i]->priority() == 10 ? 100 :time_subject[day.n_th()][i]->priority() );
				}
				else {
					student_priority[time_subject[day.n_th()][i]->student_list().at(j)] += 
						( time_subject[day.n_th()][i]->priority() == 10 ? 100 :time_subject[day.n_th()][i]->priority() );
				}
			}
		}
		day.inc_hour();
	}

	// 이로써 각 학생들의 student_priority 를 알 수 있게 된다.
	map<Student*, int>::iterator temp;
	for(auto itr = student_priority.begin(); itr != student_priority.end(); itr ++) {
		if(itr->second > max) {
			max = itr->second;
			temp = itr;
		}
	}

	time_subject[today.n_th()].pop_back();

	if(max >= (MAX_MATH_SCIENCE_SUBJECT + 1) * 100) return (MAX_PRIORITY + 1); // 그냥 불가능하다는 사실을 알려줌

	max = 0;
	// 만일 하루동안 최대 두 개의 수학 과학 과목만 듣는다면
	for(auto itr = student_priority.begin(); itr != student_priority.end(); itr ++) {
		int priority = itr->second;
		priority = (priority / 100) * 10 + priority % 100;
		if(priority > max) {
			max = priority;
			temp = itr;
		}
	}


	return max; 
}
double ExamSchedule::average_priority(ExamTime day, SubjectExam *subject, double factor)
{
	ExamTime prev_day = day;
	// 임시로 exam 에 추가해준다.
	time_subject[day.n_th()].push_back(subject);

	map<Student*, double> student_priority; 

	day.hour(0);
	while (day.day() == prev_day.day()) {
		for(int i = 0; i != time_subject[day.n_th()].size(); i ++) {
			// 각 시간대에 포함되어 있는 과목들에 대해서 학생들에 해당하는 것들을 처리 
			for(int j = 0; j != time_subject[day.n_th()][i]->student_list().size(); j ++) {
				if(student_priority.find(time_subject[day.n_th()][i]->student_list().at(j)) == student_priority.end()) {
					student_priority[time_subject[day.n_th()][i]->student_list().at(j)] = time_subject[day.n_th()][i]->priority();
				}
				else {
					student_priority[time_subject[day.n_th()][i]->student_list().at(j)] += time_subject[day.n_th()][i]->priority();
				}
			}
		}
		day.inc_hour();
	}

	// 임시로 삽입한 것을 뺀다.
	time_subject[prev_day.n_th()].pop_back();

	double sum = 0;
	for(auto itr = student_priority.begin(); itr != student_priority.end(); itr ++) {
		sum += static_cast<double>(itr->second);
	}

	if(student_priority.size() == 0) return 0;
	return sum / (pow(static_cast<double>(student_priority.size()), factor)); 
}
void ExamSchedule::pin_subjects()
{
	for(vector<SubjectExam *>::size_type i = 0; i < exam.size(); i ++) {
		// 일단 반드시 pin 을 해야 하는 시험 과목들의 
		// 수가 매우 적으므로 pin 되는 시험 시간표들을 처리하기
		// 위해서 따로 특별한 알고리즘을 사용할 필요 없이 간단히
		// 겹치지 않을 정도로만 처리해주면 된다. 
		if(exam[i]->pin() != -1) {
			ExamTime t(0, exam[i]->pin()); 
			while (t.day() < exam_days) {
				if(no_conflict(exam[i], t)) {
					time_subject[t.n_th()].push_back(exam[i]);
					exam[i]->set_inserted(true); // 처리 완료!
					break;
				}
				t.inc_day();
			}
			if(t.day() == exam_days)
				throw Error::Error(PIN_ERROR); 
		}
	}
}
void ExamSchedule::complementary_pairs()
{
	vector<int> number_list;

	// 학생이 한 명도 안 겹치는 과목인데, priority 가 꽤 커서 insert_timetable 시 
	// 위에서 처리되므로 서로 다른 시간대에 들어가 손해보는 것을 막기 위해서 도입하였다.
	typedef vector<vector<double>>::size_type array_size;
	vector<vector<double>> priority_table(exam.size());
	for(array_size i = 0; i != priority_table.size(); i ++)
		priority_table[i].resize(exam.size());

	for(array_size i = 0; i != priority_table.size(); i ++) {
		for(array_size j = 0; j != priority_table.size(); j ++) {
			if(count_conflict(exam[i], exam[j]) == 0) {
				priority_table[i][j] = 
					(static_cast<double>(exam[i]->priority() * exam[i]->num_student()) * exam[i]->importance() 
					+ static_cast<double>(exam[j]->priority() * exam[j]->num_student()) * exam[j]->importance());
			}
		}
	}

	// 1 부터 priority 까지 숫자를 보관하는데, 시간표에 추가한 과목들은 여기서 제거한다. 
	for(array_size i = 0; i != priority_table.size(); i ++)
		number_list.push_back(i); 

	// 이제 priority_table 에서 순위 높은 과목만 미리 추가해버리자.
	while (priority_table.size() > 35) {
		array_size max_i,  max_j;
		double max = -1;

		for(array_size i = 0; i != priority_table.size(); i ++) {
			for(array_size j = 0; j != priority_table.size(); j ++) {
				if(priority_table[i][j] > max) {
					max = priority_table[i][j];
					max_i = i;
					max_j = j;
				}
			}
		}

		if(max == 0)
			break;

		cout << exam[number_list[max_i]]->name() << " " << exam[number_list[max_j]]->name() << endl;

		// 두 과목은 이제 상보적인 관계이다
		exam[number_list[max_i]]->set_paired(number_list[max_j]);
		exam[number_list[max_j]]->set_paired(number_list[max_i]);

		// 이제 선택된 max_i 와 max_j 들을 시간표에 집어 넣고 
		// table 에서 제거한다. 
		if(max_i > max_j) swap(max_i, max_j); 

		priority_table.erase(priority_table.begin() + max_i); 
		priority_table.erase(priority_table.begin() + max_j - 1);

		number_list.erase(number_list.begin() + max_i);
		number_list.erase(number_list.begin() + max_j - 1);

		for(array_size i = 0; i != priority_table.size(); i ++) {
			priority_table[i].erase(priority_table[i].begin() + max_i);
			priority_table[i].erase(priority_table[i].begin() + max_j - 1);
		}
	}

}
bool ExamSchedule::no_conflict(Subject *subject, ExamTime time)
{
	// 이 과목을 time 에 삽입하였을 때 겹치는 것이 있는지 확인한다. 
	for(vector<vector<SubjectExam*>>::size_type i = 0; i < time_subject[time.n_th()].size(); i ++) {
		ZERO.resize(subject->student_mask().size());
		if((subject->student_mask() = subject->student_mask() & time_subject[time.n_th()].at(i)->student_mask()) != ZERO) 	{
			return false;
		}
	}
	return true;
}