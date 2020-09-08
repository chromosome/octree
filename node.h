#ifndef NODE_H
#define NODE_H

#include "common.h"

namespace dsa {

class node
{
public:
	using node_p = node*;

	enum type_e : size_t {
		GREY,
		BLACK
	};

private:
	point_t m_point;
	type_e  m_type;
	node_p  m_child[8] = {nullptr, nullptr, nullptr, nullptr,
						  nullptr, nullptr, nullptr, nullptr};

public:
	node(point_t p = {}, type_e t = GREY)
	: m_point(p)
	, m_type (t)
	{}

	auto point() -> const point_t {
		return m_point;
	}

	auto point(point_t p) {
		m_point = p;
	}
	
	auto type() -> const type_e {
		return m_type;
	}

	auto type(type_e t) {
		m_type = t;
	}

	auto child(size_t s) {
		return m_child[s];
	}

	auto make_child(point_t p, size_t s) -> node_p {
		return m_child[s] = node_p(new node(p, BLACK));
	}

	auto make_child(size_t s) -> node_p {
		auto child = make_child(m_point, s);
		make_grey();

		return child;
	}

	void make_grey() {
		clear();
		type(GREY);
	}

	void clear() {
		m_point = {};
	}
};

map<node::type_e, string> node_map = {
	{ node::GREY , "GREY" }, 
	{ node::BLACK, "BLACK"}
};

} // namespace dsa


ostream& operator << (ostream& out, dsa::node& n) {
	out << n.point() << ", " << dsa::node_map[n.type()];
	return out;
}

ostream& operator << (ostream& out, dsa::node::node_p& n) {
	out << n->point() << ", " << dsa::node_map[n->type()];
	return out;
}

#endif
