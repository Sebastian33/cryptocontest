#include <iostream>
#include <stack>
#include <vector>

struct State
{
	unsigned a[2];
	int ib, ie, jb, je;
	int n;
};

void checkAndAdd(unsigned na0, unsigned na1, std::stack<State>& s, std::vector<State>& tmp)
{
	for (State state : tmp)
	{
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
	s.push({ {0, 0}, 0,31,0,31, 0});
	unsigned b[2] = { 0x46508fb7, 0x6677e201 };
	/*b0c152f9 ebf2831f
	ebf2831f b0c152f9*/


	unsigned bi;
	unsigned a0, a1;
	int res;
	State current;
	while (!s.empty())
	{
		current = s.top();
		s.pop();					

		if (current.ib > current.ie && current.jb > current.je)
		//if(current.n>=31)
		{
			if(check(current, b))
				std::cout << std::hex << current.a[0] << ' ' << current.a[1] << std::endl;
			continue;
		}

		std::vector<State> tmp;
		res = 0;
		bi = (b[0] >> current.n) & 1;
		for (int i = 0; i <= current.n; i++)
		{
			res ^= (current.a[0] >> i) & (current.a[1] >> (current.n - i)) & 1;
		}

		bi ^= res;
		if (current.ib+current.jb == current.n)
		{
			if (bi)
			{
				tmp.push_back({ {current.a[0] | (1U << current.ib), current.a[1] | (1U << current.jb)},
					current.ib + 1, current.ie, current.jb + 1, current.je,
					current.n + 1 });
			}
			else
			{
				tmp.push_back({ {current.a[0], current.a[1]},
					current.ib + 1, current.ie, current.jb+1, current.je,
					current.n + 1 });
				tmp.push_back({ {current.a[0] | (1U << current.ib), current.a[1] },
					current.ib + 1, current.ie, current.jb+1, current.je,
					current.n + 1 });
				tmp.push_back({ {current.a[0], current.a[1] | (1U << current.jb)},
					current.ib + 1, current.ie, current.jb+1, current.je,
					current.n + 1 });
			}
		}
		else if ((current.a[1] >> (current.n - current.ib)) & (current.a[0] >> (current.n - current.jb)) & 1)
		{
			if (bi)
			{
				tmp.push_back({ {current.a[0] | (1U << current.ib), current.a[1] },
					current.ib + 1, current.ie, current.jb + 1, current.je,
					current.n + 1 });
				tmp.push_back({ {current.a[0], current.a[1] | (1U << current.jb)},
					current.ib + 1, current.ie, current.jb + 1, current.je,
					current.n + 1 });
			}
			else
			{
				tmp.push_back({ {current.a[0], current.a[1] },
					current.ib + 1, current.ie, current.jb + 1, current.je,
					current.n + 1 });
				tmp.push_back({ {current.a[0] | (1U << current.ib), current.a[1] | (1U << current.jb)},
					current.ib + 1, current.ie, current.jb + 1, current.je,
					current.n + 1 });
			}
		}
		else if ((current.a[0] >> (current.n - current.jb)) & 1)
		{
			tmp.push_back({ {current.a[0], current.a[1] | (bi << current.jb)},
					current.ib, current.ie, current.jb + 1, current.je,
					current.n + 1 });
		}
		else if ((current.a[1] >> (current.n - current.ib)) & 1)
		{
			tmp.push_back({ {current.a[0] | (bi << current.ib), current.a[1] },
					current.ib + 1, current.ie, current.jb, current.je,
					current.n + 1 });
		}
		else if (bi == 0)
			tmp.push_back({ {current.a[0], current.a[1] },
					current.ib, current.ie, current.jb, current.je,
					current.n + 1 });

		if (tmp.empty())
			continue;

		res = 0;
		bi = (b[1] >> (31 - current.n)) & 1;
		for (int i = 32 - current.n; i < 32; i++)
		{
			res ^= (current.a[0] >> i) & (current.a[1] >> (63 - current.n - i)) & 1;
		}

		bi ^= res;
		if (current.ie + current.je == 63-current.n)
		{
			if (bi)
			{
				checkAndAdd(1, 1, s, tmp);
			}
			else
			{
				checkAndAdd(0, 0, s, tmp);
				checkAndAdd(1, 0, s, tmp);
				checkAndAdd(0, 1, s, tmp);
			}
		}
		else if ((current.a[1] >> (63 - current.n - current.ie)) & (current.a[0] >> (63 - current.n - current.je)) & 1)
		{
			if (bi)
			{
				checkAndAdd(1, 0, s, tmp);
				checkAndAdd(0, 1, s, tmp);
			}
			else
			{
				checkAndAdd(0, 0, s, tmp);
				checkAndAdd(1, 1, s, tmp);
			}
		}
		else if ((current.a[0] >> (63 - current.n - current.je)) & 1)
		{
			checkAndAdd(2, bi, s, tmp);
		}
		else if ((current.a[1] >> (63 - current.n - current.ie)) & 1)
		{
			checkAndAdd(bi, 2, s, tmp);
		}
		else if (bi == 0)
		{
			for (State& state : tmp)
				s.push(state);
		}
	}

	std::cout << "done" << std::endl;
}