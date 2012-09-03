#include <exception>
#include <string>
#include <iostream>

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
			}
		}

		void print_error() { std::cout << error_string << std::endl; }
	};
}