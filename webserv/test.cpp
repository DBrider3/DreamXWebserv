#include <stdio.h>
#include <iostream>
#include <ctime>

int main(void)
{
    int i;

	time_t temp1;
	struct tm* timeinfo;
	time(&temp1);
	timeinfo = localtime(&temp1);
		
	std::cout << "😮‍💨😮‍💨😮‍💨😮‍💨 전 시간 : 😮‍💨😮‍💨😮‍💨😮‍💨" << asctime(timeinfo);
    i = 0;
    while (++i < 100000000)
		;
    time_t temp2;
	struct tm* timeinfo2;
	time(&temp2);
	timeinfo2 = localtime(&temp2);
		
	std::cout << "😮‍💨😮‍💨😮‍💨😮‍💨 후 시간 : 😮‍💨😮‍💨😮‍💨😮‍💨" << asctime(timeinfo2);
	return (0);
}