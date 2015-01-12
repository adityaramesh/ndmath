/*
** File Name: range_perf_test.cpp
** Author:    Aditya Ramesh
** Date:      01/10/2015
** Contact:   _@adityaramesh.com
*/

#include <chrono>
#include <ccbase/format.hpp>
#include <ndmath/range/range.hpp>

int main()
{
	using namespace std::chrono;

	static constexpr auto a = size_t{10000};
	static constexpr auto b = size_t{1000};
	static constexpr auto c = size_t{1000};

	auto n = char{};
	auto t1 = high_resolution_clock::time_point{};
	auto t2 = high_resolution_clock::time_point{};

	t1 = high_resolution_clock::now();
	asm("# BEFORE FIRST LOOP");
	nd::crange<a, b, c>([&] (const auto&) {
		n = 0;
		asm("");
	});
	asm("# AFTER FIRST LOOP");
	t2 = high_resolution_clock::now();
	cc::println(duration_cast<milliseconds>(t2 - t1).count());

	t1 = high_resolution_clock::now();
	asm("# BEFORE SECOND LOOP");
	for (auto i = 0u; i != a; ++i) {
		for (auto j = 0u; j != b; ++j) {
			for (auto k = 0u; k != c; ++k) {
				n = 0;
				asm("");
			}
		}
	}
	asm("# AFTER SECOND LOOP");
	t2 = high_resolution_clock::now();
	cc::println(duration_cast<milliseconds>(t2 - t1).count());
}
