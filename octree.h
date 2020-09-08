#ifndef OCTREE_H
#define OCTREE_H

#include "node.h"
#include "octant.h"

namespace dsa {

using node_p = node::node_p;
using type_e = node::type_e;
using oct_e  = octant::oct_e;
using lin_f = function<void(node_p, octant)>;
using vol_f = function<bool(node_p, octant)>;

void debug(node_p n, octant o) {
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
		auto ss  = os.oct();

		return { n->child(ss), os };
	}


	/** Search ----------------------------------------------------------------
	 */
	bool search(point_t p, lin_f fb = lin_f([](node_p n, octant q){}), 
						   lin_f fg = lin_f([](node_p n, octant q){}), 
						   lin_f fw = lin_f([](node_p n, octant q){})) {

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
	void bfs(vol_f fc = vol_f([](node_p n, octant q){ return true; }), 
			 lin_f fp = lin_f([](node_p n, octant q){})) {

		if (m_root == nullptr)
			return;
		else if (m_root-> type() == node::BLACK) {
			fc(m_root, m_source);
			return;
		}

		queue<tuple<node_p, octant>> Q;
		Q.push({m_root, m_source});

		while(!Q.empty()) {
			auto [n, o] = Q.front();
			Q.pop();

			for (auto& s: octs) {

				auto c = n->child(s);
				auto oc = o.subdivide(s);

				if (fc(c, oc)) {
					Q.push({c, oc});
				}
			}

			fp(n, o);
		}
	}


	/** Volume Search ---------------------------------------------------------
	 */
	void volume_search(octant o, lin_f fb = lin_f([](node_p n, octant q){}), 
								 lin_f fg = lin_f([](node_p n, octant q){}), 
								 lin_f fw = lin_f([](node_p n, octant q){})) {

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


	/**	Clear -----------------------------------------------------------------
	 */
	auto clear() {
		auto func = [] (node_p n, octant o) { 
			// cout << "deleted: " << n << endl;
			delete n;
		};

		volume_search(m_source, func, func);
	}


public:

	/**	Octree ----------------------------------------------------------------
	 */
	octree() {}


	/**	Octree ----------------------------------------------------------------
	 */
	octree(double x, double y, double z)
	: m_source({}, {x, y, z})
	{}


	/**	Octree ----------------------------------------------------------------
	 */
	octree(dist_t r)
	: m_source({}, r)
	{}


	/**	~Octree ---------------------------------------------------------------
	 */
	~octree() {
		clear();
	}


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
	bool find(point_t p) {
		node_p entry = nullptr;
		auto func = 
			[&entry, p] (node_p n, octant o) {
				if  (p == n->point())
					entry = n;
			};

		search(p, func);
		return entry != nullptr ? true : false;
	}


	/** Track -----------------------------------------------------------------
	 */
	vector<oct_e> track(point_t p) {
		vector<oct_e> path;

		auto func = 
			[&path] (node_p n, octant o) { 
				path.push_back(o.oct());
			};

		search(p, func, func);
		return path;
	}


	/** Insert ----------------------------------------------------------------
	 */
	bool insert(point_t p) {
		node_p n = nullptr;

		if (!m_source.contains(p))
			return false;

		n = insert(node_p(new node(p)));
		if (n != nullptr) {
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
		// cout << "[insert]: " << p << endl;

		node_p current = m_root;
		node_p child   = current;

		if (m_root == nullptr) {
			m_root = node_p(new node({}, node::BLACK));

			// cout << "root: ";
			// debug(m_root, o);
			// cout << "[inserting]: " << p << endl;
			// cout << "[done]" << endl << endl;

			m_size++;
			return m_root;
		}

		if (current->type() == node::BLACK) {

			if (p == current->point())
				return current;
			else {
				auto [black, ob] = subdivide(current, o, current->point());

				black = current->make_child(ob.oct());
				current->point(o.center());
			}
		}

		// current = descend(p);
		child = current;

		while((child != nullptr) && (child->type() == node::GREY)) {
			current = child;
			o = oc;

			tie(child, oc) = subdivide(current, o, p);

			// cout << "current: ";
			// debug(current, o);

			// cout << "child: ";
			// debug(child, oc);

			// cout << endl;

			if (child == nullptr) {
				child = current->make_child({}, oc.oct());
				// cout << "[inserting]: " << p << endl;
			}
			else if (child->type() == node::BLACK) {
				if (p == child->point())
					return nullptr;

				auto [black, ob] = subdivide(child, oc, child->point());

				black = child->make_child(ob.oct());
				child->point(oc.center());
			}
		}

		// cout << "[done]" << endl << endl;

		m_size++;
		return child;
	}
};

} // namespace dsa

#endif
