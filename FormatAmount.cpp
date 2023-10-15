// FormatAmount.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

#include "common.h"

#define MAX_AMOUNT 9999999.99
std::string FormatAmount(std::string& amount)
{
	TRACE("Amount IN %s\n", amount.c_str());


	bool flgCntLead = true;
	bool flgDotFound = false;
	trim(amount);
	size_t ln = amount.length();
	//Check for nonnumerical
	for (int i = 0; i < ln; i++)
	{
		if (int(amount.c_str()[i]) < 48 || int(amount.c_str()[i]) > 57)
		{
			if (amount.c_str()[i] != '.')
			{
				TRACE("Amount contains non numerical values.%s\n", "\n");
				return "Amount contains non numerical values.";
			}
			else
			{
				if (flgDotFound)
				{
					TRACE("Amount contains two dots.%s\n", "\n");
					return "Amount contains two dots.";
				}
				else
				{
					flgDotFound = true;
				}
			}
		}

	}
	long double amt{ stold(amount) };
	if (amt < .001)
	{
		TRACE("Invalid amount input (zero value) %s\n", "\n");
		return "Invalid amount input  (zero value) ";

	}         
	if (amt > MAX_AMOUNT)
	{
		TRACE("Invalid amount input (amount too large)\n", "\n");
		return  "Invalid amount input  (amount too large)";

	}
	if(( long(amt * 1000) - ((long)(amt * 100.0)) * 10) > 0)
	{
		TRACE("Invalid amount input (more then two decimals) %s\n", "\n");
		return "Invalid amount input  (more then two decimals) ";
	}
	long long i = amt * 100;
	char buff[11];
	buff[10] = 0;
	sprintf_s(buff, "%010d", i);
	amount = buff;
	return "OK";
}

int main()
{
	std::string str("123");
	FormatAmount(str);
	std::cout << str << std::endl;

	str = +".123";
	FormatAmount(str);
	std::cout << str << std::endl;

	str = "123.";
	FormatAmount(str);
	std::cout << str << std::endl;

	str = "123.1";
	FormatAmount(str);
	std::cout << str << std::endl;
	str = +".12";
	FormatAmount(str);
	std::cout << str << std::endl;

	str = "0.01";
	FormatAmount(str);
	std::cout << str << std::endl;

	str = "0.1";
	FormatAmount(str);
	std::cout << str << std::endl;

	str = "1234567899";
	FormatAmount(str);
	std::cout << str << std::endl;

	str = "12345678.90";
	FormatAmount(str);
	std::cout << str << std::endl;
    
	str = "99999999.99";
	FormatAmount(str);
	std::cout << str << std::endl;

	std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
