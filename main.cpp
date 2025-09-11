/*
	0x46508fb7, 0x6677e201
	b0c152f9 ebf2831f
	ebf2831f b0c152f9

	0x000073af, 0x00000000
	00000001 000073af
	00000083 000000e5
	000000e5 00000083
	000073af 00000001

	0x738377c1, 0x00000000
	00000001 738377c1
	0000b0c5 0000cd55
	0000cd55 0000b0c5
	738377c1 00000001
	*/

#include <iostream>
#include <stack>
#include <vector>

using u8 = unsigned char;

struct State
{
	unsigned a[2];
	int ib, ie, jb, je;
	u8 s1 : 3;
	u8 s2 : 3;
	int n;

	inline unsigned getBit( int n)
	{
		return (a[n / 32] >> (n % 32)) & 1;
	}
	inline void setBit(unsigned bit, int n)
	{
		a[n / 32] &= ~(1U << (n % 32));
		a[n / 32] |= bit << (n % 32);
	}
};

inline void checkAndAddB(unsigned na0, unsigned na1, State s, std::vector<State>& next, int& size, u8 s1)
{
	s.s1 = s1;
	s.n++;
	if (na0 <= 1)
	{
		if (s.ie < s.ib && ((s.a[0] >> s.ib) & 1) != na0)
			return;
		s.a[0] |= na0 << s.ib;
		s.ib++;
	}
	if (na1 <= 1)
	{
		if (s.je < s.jb && ((s.a[1] >> s.jb) & 1) != na1)
			return;
		s.a[1] |= na1 << s.jb;
		s.jb++;
	}
	next[size] = s;
	size++;
}

void checkAndAddE(unsigned na0, unsigned na1, std::stack<State>& s, std::vector<State>& next, int size, u8 s2)
{
	for (int i = 0; i < size; i++)
	{
		State state = next[i];
		state.s2 = s2;
		if (na0 <= 1)
		{
			if (state.ie < state.ib && ((state.a[0] >> state.ie) & 1) != na0)
				continue;
			state.a[0] |= na0 << state.ie;
			state.ie--;
		}
		if (na1 <= 1)
		{
			if (state.je < state.jb && ((state.a[1] >> state.je) & 1) != na1)
				continue;
			state.a[1] |= na1 << state.je;
			state.je--;
		}
		s.push(state);
	}
}

bool check(const State& s, unsigned* b)
{
	int res;
	for (int n = s.n; n < 32; n++)
	{
		res = 0;
		for (int i = 0; i <= n; i++)
		{
			res ^= (s.a[0] >> i) & (s.a[1] >> (n - i)) & 1;
		}
		if (res != ((b[0] >> n) & 1))
			return false;
	}
	for (int n = 32; n <= 63 - s.n; n++)
	{
		res = 0;
		for (int i = n - 31; i < 32; i++)
		{
			res ^= (s.a[0] >> i) & (s.a[1] >> (n - i)) & 1;
		}
		if (res != ((b[1] >> (n - 32)) & 1))
			return false;
	}
	return true;
}

void binOut(unsigned* a)
{
	for (int i = 31; i >= 0; i--)
		std::cout << ((a[0] >> i) & 1);
	std::cout << ' ';
	for (int i = 31; i >= 0; i--)
		std::cout << ((a[1] >> i) & 1);
	std::cout << std::endl;
}

int main()
{
	std::stack<State> s;
	s.push({ {0, 0}, 0, 31, 0, 31, 0, 0, 0 });
	unsigned b[2] = { 0x46508fb7, 0x6677e201 };

	int bit;
	State current;
	std::vector<State> next(3);
	int size;
	while (!s.empty())
	{
		current = s.top();
		s.pop();
		size = 0;

		if (current.ib > current.ie && current.jb > current.je)
		{
			if (check(current, b))
			{
				std::cout << std::hex << current.a[0] << ' ' << current.a[1] << std::endl;
			}
			continue;
		}

		switch (current.s1)
		{
		case 0:
		{
			if ((b[0] >> current.n) & 1)
			{
				checkAndAddB(1, 1, current, next, size, 2);
			}
			else
			{
				checkAndAddB(0, 1, current, next, size, 1);
				checkAndAddB(1, 0, current, next, size, 1);
				checkAndAddB(0, 0, current, next, size, 3);
			}
			break;
		}
		case 1:
		{
			if ((b[0] >> current.n) & 1)
			{
				bit = (current.a[0] >> (current.ib - 1)) & 1;
				checkAndAddB(1+bit, 1+(1^bit), current, next, size, 2);
			}
			else
			{
				bit = (current.a[0] >> (current.ib - 1)) & 1;
				checkAndAddB(bit << 1, (bit ^ 1) << 1, current, next, size, 1);
			}
			break;
		}
		case 2:
		{
			int res = 0;
			for (int i = 0; i <= current.n; i++)
			{
				res ^= (current.a[0] >> i) & (current.a[1] >> (current.n - i)) & 1;
			}

			bit = ((b[0] >> current.n) & 1) ^ res;
			checkAndAddB(bit ^ 1, 1, current, next, size, 2);
			checkAndAddB(bit, 0, current, next, size, 2);
			break;
		}
		case 3:
		{
			if (((b[0] >> current.n) & 1) == 0)
			{
				checkAndAddB(2, 2, current, next, size, 0);
			}
			break;
		}
		}

		if (current.n == 0)
		{
			for (int i = 0; i < size; i++)
				s.push(next[i]);
			continue;
		}

		switch (current.s2)
		{
		case 0:
		{
			if ((b[1] >> (31 - current.n)) & 1)
			{
				checkAndAddE(1, 1, s, next, size, 2);
			}
			else
			{
				checkAndAddE(0, 1, s, next, size, 1);
				checkAndAddE(1, 0, s, next, size, 1);
				checkAndAddE(0, 0, s, next, size, 3);
			}
			break;
		}
		case 1:
		{
			if ((b[1] >> (31 - current.n)) & 1)
			{
				bit = (current.a[0] >> (current.ie + 1)) & 1;
				checkAndAddE(1 + bit, 1 + (bit ^ 1), s, next, size, 2);
			}
			else
			{
				bit = (current.a[0] >> (current.ie + 1)) & 1;
				checkAndAddE(bit << 1, (bit ^ 1) << 1, s, next, size, 1);
			}
			break;
		}
		case 2:
		{
			int res = 0;
			for (int i = 32 - current.n; i < 32; i++)
			{
				res ^= (current.a[0] >> i) & (current.a[1] >> (63 - current.n - i)) & 1;
			}

			bit = ((b[1] >> (31 - current.n)) & 1) ^ res;
			checkAndAddE(bit ^ 1, 1, s, next, size, 2);
			checkAndAddE(bit, 0, s, next, size, 2);
			break;
		}
		case 3:
		{
			if (((b[1] >> (31 - current.n)) & 1) == 0)
			{
				checkAndAddE(2, 2, s, next, size, 0);
			}
			break;
		}
		}

	}
	std::cout << "done" << std::endl;
}