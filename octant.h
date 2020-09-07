#ifndef OCTANT_H
#define OCTANT_H

#include "common.h"

namespace dsa {

using vol_t = tuple<double, double, double, double, double, double>;

class octant
{
public:
	enum side_e : size_t {
		SRC = SIZE_MAX,
		FNE = 0b000,
		FNW = 0b001,
		FSE = 0b010,
		FSW = 0b011,
		BNE = 0b100,
		BNW = 0b101,
		BSE = 0b110,
		BSW = 0b111
	};

private:
	point_t m_center;
	dist_t  m_radius;
	side_e  m_side;

public:
	octant(point_t c = {0., 0., 0.}, dist_t r = {1., 1., 1.}, side_e s = SRC)
	: m_center(c)
	, m_radius(r)
	, m_side  (s)
	{}

	auto center() -> const point_t {
		return m_center;
	}

	auto center(point_t c) {
		m_center = c;
	}

	auto radius() -> const dist_t {
		return m_radius;
	}

	auto radius(dist_t r) {
		m_radius = r;
	}

	auto side() -> const side_e {
		return m_side;
	}

	auto side(side_e s) {
		m_side = s;
	}

	auto octant_of(point_t p) -> side_e {
		auto [px, py, pz] = p;
		auto [cx, cy, cz] = m_center;

		if (px > cx)
			if (py > cy)
				if (pz > cz)
					return FNE;
				else
					return BNE;
			else
				if (pz > cz)
					return FSE;
				else
					return BSE;
		else
			if (py > cy)
				if (pz > cz)
					return FNW;
				else
					return BNW;
			else
				if (pz > cz)
					return FSW;
				else
					return BSW;
	}

	auto subdivide(side_e s) -> octant {
		auto [cx, cy, cz] = m_center;
		auto [rx, ry, rz] = m_radius;

		double xf[8] = {1, -1,  1, -1,  1, -1,  1, -1};
		double yf[8] = {1,  1, -1, -1,  1,  1, -1, -1};
		double zf[8] = {1,  1,  1,  1, -1, -1, -1, -1};

		rx *= 0.5; ry *= 0.5; rz *= 0.5;

		cx += xf[s]*rx; cy += yf[s]*ry; cz += zf[s]*rz;

		return { {cx, cy, cz}, {rx, ry, rz}, s };
	}

	auto volume() -> vol_t {
		auto [cx, cy, cz] = m_center;
		auto [rx, ry, rz] = m_radius;

		return { cx - rx, cx + rx, cy - ry, cy + ry, cz - rz, cz + rz };
	}

	bool contains(point_t p) {
		auto [xmin, xmax, ymin, ymax, zmin, zmax] = volume();
		auto [px, py, pz] = p;

		if (px > xmin && px <= xmax && py > ymin && py <= ymax)
			return true;
		else
			return false;
	}

	bool intersects(octant& q) {
		auto [ocx, ocy, ocz] = m_center;
		auto [orx, ory, orz] = m_radius;

		auto [qcx, qcy, qcz] = q.center();
		auto [qrx, qry, qrz] = q.radius();

		if (   ((orx + qrx) > abs(ocx - qcy)) 
			&& ((ory + qry) > abs(ocy - qcy))
			&& ((orz + qrz) > abs(ocz - qcz)))
			return true;

		return false;
	}

};

map<octant::side_e, string> oct_map = { 
	{ octant::side_e::SRC, "SRC" },
	{ octant::side_e::FNE, "FNE" },
	{ octant::side_e::FNW, "FNW" },
	{ octant::side_e::FSE, "FSE" },
	{ octant::side_e::FSW, "FSW" },
	{ octant::side_e::BNE, "BNE" },
	{ octant::side_e::BNW, "BNW" },
	{ octant::side_e::BSE, "BSE" },
	{ octant::side_e::BSW, "BSW" } 
};

} // namespace dsa


ostream& operator << (ostream& out, dsa::octant& o) {
	out << "[side: "    << dsa::oct_map[o.side()]
		<< ", center: " << o.center() 
		<< ", radius: " << o.radius() << "]";
	return out;
}


#endif
