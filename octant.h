#ifndef OCTANT_H
#define OCTANT_H

#include "common.h"

namespace dsa {

using vol_t = tuple<double, double, double, double, double, double>;

class octant
{
public:
	enum oct_e : size_t {
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
	oct_e  m_oct;

public:
	octant(point_t c = {0., 0., 0.}, dist_t r = {1., 1., 1.}, oct_e s = SRC)
	: m_center(c)
	, m_radius(r)
	, m_oct  (s)
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

	auto oct() -> const oct_e {
		return m_oct;
	}

	auto oct(oct_e s) {
		m_oct = s;
	}

	auto octant_of(point_t p) -> oct_e {
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

	auto subdivide(oct_e s) -> octant {
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

map<octant::oct_e, string> oct_map = { 
	{ octant::oct_e::SRC, "SRC" },
	{ octant::oct_e::FNE, "FNE" },
	{ octant::oct_e::FNW, "FNW" },
	{ octant::oct_e::FSE, "FSE" },
	{ octant::oct_e::FSW, "FSW" },
	{ octant::oct_e::BNE, "BNE" },
	{ octant::oct_e::BNW, "BNW" },
	{ octant::oct_e::BSE, "BSE" },
	{ octant::oct_e::BSW, "BSW" } 
};

} // namespace dsa


ostream& operator << (ostream& out, dsa::octant& o) {
	out << "[oct: "    << dsa::oct_map[o.oct()]
		<< ", center: " << o.center() 
		<< ", radius: " << o.radius() << "]";
	return out;
}


#endif
