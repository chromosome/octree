#include "octree.h"

using namespace dsa;

int main(int argc, char const *argv[])
{
	octree tree;

	point_t p = {0.5, 0.5, 0.5};
	tree.insert(p);

	auto f = tree.find(p);
	cout << "found: ";
	if (f != nullptr)
		cout << f << endl;
	cout << endl;

	point_t p2 = {0.55, 0.55, 0.55};
	tree.insert(p2);

	auto tracks = tree.track(p);
	cout << "p track: ";
	for (const auto& t: tracks)
		cout << dsa::oct_map[t] << " ";
	cout << endl;

	tracks = tree.track(p2);
	cout << "p2 track: ";
	for (const auto& t: tracks)
		cout << dsa::oct_map[t] << " ";
	cout << endl;
	
	return 0;
}
