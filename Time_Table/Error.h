#include <exception>
#include <string>
#include <iostream>

#define PIN_ERROR 1
#define NO_AVAIL_TIMESLOT_EXAM_ERROR 2
#define NO_AVAIL_DAY_EXAM_ERROR 3
#define NO_AVAIL_CLASS_EXAM_ERROR 4
#define NO_AVAIL_CLASS_SWAP_ERROR 5

#pragma once

namespace Error
{
	struct Error : std::exception
	{
	private:
		std::string error_string; 
	
	public:
		Error(int code)
		{
			switch(code)
			{
			case 1:
				error_string = "Pin error ";
				break;
			case 2:
				error_string = "No avail timeslots on ExamSchedule";
				break;
			case 3:
				error_string = "No available day for ExamSchedule";
				break;
			case 4:
				error_string = "No available class for ExamSchedule";
				break;
			case 5:
				error_string = "Not able to swap classes ";
				break;
			}
		}

		void print_error() { std::cout << error_string << std::endl; }
	};
}