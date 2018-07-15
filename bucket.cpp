#include <array>
#include <deque>
#include <algorithm>
#include <iostream>

const int BUCKETS_COUNT = 3;
const int BUCKETS_CAPACITY[] = {8, 5, 3};
const int BUCKETS_BEGIN_0 = 8;
const int BUCKETS_BEGIN_1 = 0;
const int BUCKETS_BEGIN_2 = 0;
const int BUCKETS_FINAL_0 = 4;
const int BUCKETS_FINAL_1 = 4;
const int BUCKETS_FINAL_2 = 0;

struct dump_action
{
	int from;
	int to;
	int water;
};

struct bucket_state
{
	std::array<int, BUCKETS_COUNT> bucket_s;
	dump_action cur_act;

	bool can_dump_water(int from, int to) const
	{
		return (from != to && bucket_s[from] != 0 && bucket_s[to] < BUCKETS_CAPACITY[to]);
	}

	bool is_final()
	{
		return (bucket_s[0] == BUCKETS_FINAL_0 && bucket_s[1] == BUCKETS_FINAL_1 &&
			bucket_s[2] == BUCKETS_FINAL_2);
	}

	bucket_state dump_water(int from, int to) const
	{
		bucket_state next = *this;
		int water_left = BUCKETS_CAPACITY[to] - bucket_s[to];
		if (bucket_s[from] >= water_left)
		{
			next.bucket_s[from] -= water_left;
			next.bucket_s[to] += water_left;
		}
		else {
			water_left = next.bucket_s[from];
			next.bucket_s[from] = 0;
			next.bucket_s[to] += water_left;
		}
		next.cur_act = { from, to, water_left };
		return next;
	}
};

using bucket_dump_log = std::deque<bucket_state>;

int solution_count = 0;
void print_bucket_dump_log(bucket_dump_log &states)
{
	std::locale::global(std::locale("chs"));
	++solution_count;
	std::wcout << L"======== solution " << solution_count << " ========" << std::endl;
	for (bucket_dump_log::size_type i = 1; i < states.size(); ++i)
	{
		auto state = states.at(i);
		std::wcout << L"从" << BUCKETS_CAPACITY[state.cur_act.from] << L"升水桶中倒"  << state.cur_act.water 
			<< L"升水到" << BUCKETS_CAPACITY[state.cur_act.to] << L"升水桶中" << std::endl;
	}
	std::wcout << L"\n" << std::endl;
}

bool is_processed_state(const bucket_dump_log &states, const bucket_state &new_state)
{
	auto it = std::find_if(states.begin(), states.end(), 
		[&new_state](bucket_state s) {return s.bucket_s == new_state.bucket_s;});
	return (it != states.end());
}

void search_state(bucket_dump_log &states)
{
	auto current = states.back();
	if (current.is_final())
	{
		print_bucket_dump_log(states);
		return;
	}
	for (int j = 0; j < BUCKETS_COUNT; ++j)
	{
		for (int i = 0; i < BUCKETS_COUNT; ++i)
		{
			if (current.can_dump_water(i, j))
			{
				auto next = current.dump_water(i, j);
				if (!is_processed_state(states, next))
				{
					states.push_back(next);
					search_state(states);
					states.pop_back();
				}
			}
		}
	}
}

void test_bucket()
{
	bucket_dump_log states;
	bucket_state init = { { BUCKETS_BEGIN_0 , BUCKETS_BEGIN_1, BUCKETS_BEGIN_2}, {-1, 0, BUCKETS_BEGIN_0 } };
	states.push_back(init);
	search_state(states);
}