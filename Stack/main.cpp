#include "Stack.h"
#include <iostream>

void main()
{
	Stack<int> s;
	for (int i = 0; i < 10000; i++) s.Push(i);
	for (int i = 0; i < 9990; i++) s.Pop();

	Stack<int> s1(s), s2;
	s2 = s;

	while (s2.Size() > 0)
	{
		std::cout << s2.Top() << "\n";
		s2.Pop();
	}
	system("pause");
}