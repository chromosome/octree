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
		FTR = 0b000,
		FTL = 0b001,
		FBR = 0b010,
		FBL = 0b011,
		BTR = 0b100,
		BTL = 0b101,
		BBR = 0b110,
		BBL = 0b111
	};

private:
	point_t m_center;
	dist_t  m_radius;
	oct_e   m_oct;

public:
	octant(point_t c = {0., 0., 0.}, dist_t r = {1., 1., 1.}, oct_e s = SRC)
	: m_center(c)
	, m_radius(r)
	, m_oct   (s)
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
					return FTR;
				else
					return BTR;
			else
				if (pz > cz)
					return FBR;
				else
					return BBR;
		else
			if (py > cy)
				if (pz > cz)
					return FTL;
				else
					return BTL;
			else
				if (pz > cz)
					return FBL;
				else
					return BBL;
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

	auto volume() const -> vol_t {
		auto [cx, cy, cz] = m_center;
		auto [rx, ry, rz] = m_radius;

		return { cx - rx, cx + rx, cy - ry, cy + ry, cz - rz, cz + rz };
	}

	bool contains(point_t p) const {
		auto [xmin, xmax, ymin, ymax, zmin, zmax] = volume();
		auto [px, py, pz] = p;

		if (   px > xmin && px <= xmax 
			&& py > ymin && py <= ymax 
			&& pz > zmin && pz <= zmax)
			return true;

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
	{ octant::SRC, "SRC" },
	{ octant::FTR, "FTR" },
	{ octant::FTL, "FTL" },
	{ octant::FBR, "FBR" },
	{ octant::FBL, "FBL" },
	{ octant::BTR, "BTR" },
	{ octant::BTL, "BTL" },
	{ octant::BBR, "BBR" },
	{ octant::BBL, "BBL" } 
};

auto octs = {octant::FTR, octant::FTL, octant::FBR, octant::FBL, 
			 octant::BTR, octant::BTL, octant::BBR, octant::BBL};

} // namespace dsa


ostream& operator << (ostream& out, dsa::octant& o) {
	out << "[oct: "     << dsa::oct_map[o.oct()]
		<< ", center: " << o.center() 
		<< ", radius: " << o.radius() << "]";
	return out;
}


#endif
