#ifndef OCTREE_H
#define OCTREE_H

#include "node.h"
#include "octant.h"

namespace dsa {

using node_p = node::node_p;
using type_e = node::type_e;
using side_e = octant::side_e;

void debug(node_p n, octant o) {
	auto s = o.side();

	if (n == nullptr)
		cout << "WHITE" << endl;
	else
		cout << n << endl;

	cout << "octant: " << o << endl;
}

class octree
{
private:
	node_p m_root = nullptr;
	const  octant m_source;
	size_t m_size = 0;
	

	/**	Subdivide -------------------------------------------------------------
	 */
	auto subdivide(node_p n, octant o, point_t p) -> tuple<node_p, octant> {
		auto s = o.octant_of(p);
		auto os = o.subdivide(s);
		auto ss  = os.side();

		return { n->child(ss), os };
	}


	/** Search ----------------------------------------------------------------
	 */
	bool search(point_t p, function<void(node_p, octant)> fb = nullptr,
						   function<void(node_p, octant)> fg = nullptr,
						   function<void(node_p, octant)> fw = nullptr) {

		if (fb == nullptr)
			fb = [](node_p n, octant o){};
		if (fg == nullptr)
			fg = [](node_p n, octant o){};
		if (fw == nullptr)
			fw = [](node_p n, octant o){};

		node_p current = m_root;
		octant o;

		while ((current != nullptr) && (current->type() == node::GREY)) {

			fg(current, o);

			tie(current, o) = subdivide(current, o, p);
		}

		if (current != nullptr) {
			fb(current, o);
			return true;
		}
		else {
			fw(current, o);
			return false;
		}
	}


	/** BFS -------------------------------------------------------------------
	 */
	void bfs(function<bool(node_p, octant)> f) {
		if (m_root == nullptr)
			return;
		else if (m_root-> type() == node::BLACK) {
			f(m_root, m_source);
			return;
		}

		queue<tuple<node_p, octant>> Q;
		Q.push({m_root, m_source});

		while(!Q.empty()) {
			auto [n, o] = Q.front();
			Q.pop();

			auto octs = {octant::FNE, octant::FNW, octant::FSE, octant::FSW, 
						 octant::BNE, octant::BNW, octant::BSE, octant::BSW};
			for (auto& s: octs) {

				auto c = n->child(s);
				auto oc = o.subdivide(s);

				if (f(c, oc)) {
					Q.push({c, oc});
				}
			}
		}
	}


	/** Volume Search ---------------------------------------------------------
	 */
	void volume_search(octant o, function<void(node_p, octant)> fb = nullptr,
								 function<void(node_p, octant)> fg = nullptr, 
								 function<void(node_p, octant)> fw = nullptr) {
		if (fb == nullptr)
			fb = [](node_p n, octant q){};
		if (fg == nullptr)
			fg = [](node_p n, octant q){};
		if (fw == nullptr)
			fw = [](node_p n, octant q){};

		auto func = 
			[&o, &fb, &fg, &fw] (node_p n, octant q) {
				if ((n != nullptr) && o.intersects(q)) {

					if ((n->type() == node::BLACK)) {
						fb(n, q);
						return false;
					}

					fg(n, q);
					return true;
				}

				fw(n, q);
				return false;
			};

		bfs(func);
	}


public:

	/** Descend ---------------------------------------------------------------
	 */
	node_p descend(point_t p) {
		node_p m = nullptr;
		auto func = [&m] (node_p n, octant o) { m = n; };
		search(p, func, func, func);
		return m;
	}


	/** Find ------------------------------------------------------------------
	 */
	node_p find(point_t p) {
		node_p entry = nullptr;
		auto func = 
			[&entry, p] (node_p n, octant o) {
				if  (p == n->point())
					entry = n;
			};

		search(p, func);
		return entry;
	}


	/** Track -----------------------------------------------------------------
	 */
	vector<side_e> track(point_t p) {
		vector<side_e> path;

		auto func = 
			[&path] (node_p n, octant o) { 
				path.push_back(o.side());
			};

		search(p, func, func);
		return path;
	}


	/** Insert ----------------------------------------------------------------
	 */
	bool insert(point_t p) {
		node_p n = nullptr;

		if ((n = insert(node_p(new node(p)))) != nullptr) {
			n->point(p);

			return true;
		}

		return false;
	}


	/** Insert ----------------------------------------------------------------
	 */
	node_p insert(node_p n) {
		point_t p = n->point();

		octant o  = m_source;
		octant oc = m_source;
		cout << "[insert]: " << p << endl;

		node_p current = m_root;
		node_p child   = current;

		if (m_root == nullptr) {
			m_root = node_p(new node({}, node::BLACK));

			cout << "root: ";
			debug(m_root, o);
			cout << "[inserting]: " << p << endl;
			cout << "[done]" << endl << endl;

			m_size++;
			return m_root;
		}

		if (current->type() == node::BLACK) {

			if (p == current->point())
				return current;
			else {
				auto [black, ob] = subdivide(current, o, current->point());

				black = current->make_child(ob.side());
			}
		}

		// current = descend(p);
		child = current;

		while((child != nullptr) && (child->type() == node::GREY)) {
			current = child;
			o = oc;

			tie(child, oc) = subdivide(current, o, p);

			cout << "current: ";
			debug(current, o);

			cout << "child: ";
			debug(child, oc);

			cout << endl;

			if (child == nullptr) {
				child = current->make_child({}, oc.side());
				cout << "[inserting]: " << p << endl;
			}
			else if (child->type() == node::BLACK) {
				if (p == child->point())
					return nullptr;

				auto [black, ob] = subdivide(child, oc, child->point());

				black = child->make_child(ob.side());
			}
		}

		cout << "[done]" << endl << endl;

		m_size++;
		return child;
	}
};

} // namespace dsa

#endif
