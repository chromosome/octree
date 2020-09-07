#ifndef COMMON_H
#define COMMON_H

#include <bits/stdc++.h>

using namespace std;

namespace dsa {

using point_t = tuple<double, double, double>;
using  dist_t = tuple<double, double, double>;

} // namespace dsa

template<typename T, unsigned N, unsigned Last>
struct tuple_printer {
	static void print (ostream& out, const T& value) {
		out << get<N>(value) << ", ";
		tuple_printer<T, N+1, Last>::print(out, value);
	}
};

template<typename T, unsigned N>
struct tuple_printer <T, N, N> {
	static void print (ostream& out, const T& value) {
		out << get<N>(value);
	}
};

template<typename... Args>
ostream& operator << (ostream& out, const tuple<Args...>& value) {
	out << "(";
	tuple_printer<tuple<Args...>, 0, sizeof...(Args)-1>::print(out, value);
	out << ")";
	return out;
}

#endif
