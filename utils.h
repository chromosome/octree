#ifndef UTILS_H
#define UTILS_H

#include "common.h"

using namespace dsa;

vector<point_t> read_obj_file(string filename) {
	string name;
	vector<point_t> vertices;

	ifstream input(filename);
	string line;

	while (getline(input, line)) {
		stringstream line_stream(line);
		
		string s;
		double x, y, z;

		line_stream >> s;
		if (s == "v") {
			line_stream >> x >> y >> z;
			vertices.push_back({x, y, z});
		}
	}

	return vertices;
}

#endif
