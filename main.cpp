#include "octree.h"
#include "utils.h"

using namespace dsa;

int main(int argc, char const *argv[])
{
	auto model = read_obj_file("suzanne.obj");

	double xmax = 0, ymax = 0, zmax = 0;
	for (const auto& v: model) {
		xmax = get<0>(v) > xmax ? get<0>(v) : xmax;
		ymax = get<1>(v) > ymax ? get<1>(v) : ymax;
		zmax = get<2>(v) > zmax ? get<2>(v) : zmax;
	}

	cout << xmax << endl;
	cout << ymax << endl;
	cout << zmax << endl;

	octree tree(xmax, ymax, zmax);

	for (const auto& v: model)
		if (tree.insert(v))
			cout << "inserted: " << v << endl;

	cin.get();

	// point_t p = {0.9, 0.9, 0.9};
	// tree.insert(p);

	// if (tree.insert({1.2, 0, 0}))
	// 	cout << "wat" << endl;

	// if (tree.find(p))
	// 	cout << "found: " << p << endl;

	// point_t p2 = {0.91, 0.91, 0.91};
	// tree.insert(p2);

	// auto tracks = tree.track(p);
	// cout << "p track: ";
	// for (const auto& t: tracks)
	// 	cout << dsa::oct_map[t] << " ";
	// cout << endl;

	// tracks = tree.track(p2);
	// cout << "p2 track: ";
	// for (const auto& t: tracks)
	// 	cout << dsa::oct_map[t] << " ";
	// cout << endl;
	
	return 0;
}
