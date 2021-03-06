// File: renderable.hh
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once

#include <memory>
#include "geometry/ray.hh"
#include "geometry/aabb.hh"
#include "material/texture.hh"
using std::shared_ptr;
using std::make_shared;

class Trace;
class Renderable;
typedef shared_ptr<Renderable> rdptr;

class Renderable {
	private:
		shared_ptr<Texture> texture = nullptr;

	public:
		virtual ~Renderable(){};

		Renderable(){ }

		Renderable(const shared_ptr<Texture>& _texture):
			texture(_texture) { }

		virtual bool infinity() const
		{ return false; }

		virtual bool have_inside() const
		{ return true; }

		void set_texture(const shared_ptr<Texture>& _texture)
		{ texture = _texture; }

		const shared_ptr<Texture>& get_texture() const
		{ return texture; }

		virtual shared_ptr<Trace> get_trace(const Ray& ray, real_t max_dist) const = 0;
		// judge visibility and return ptr if visible

		shared_ptr<Trace> get_trace(const Ray& ray) const
		{ return get_trace(ray, -1); }

		virtual AABB get_aabb() const = 0;
};

struct IntersectInfo {
	Vec norm, inter_point;
	shared_ptr<Surface> surf;

	real_t forward_density, inter_dist;
	bool contain;
};

// a combination of renderable object and a ray, to integrate some calculations
class Trace {
	protected:
		const Ray& ray;			// ray must have longer lifecycle than any trace !

		virtual	shared_ptr<Surface> transform_get_property() const = 0;

		// two useful internal members
		mutable bool toward = true;
		mutable real_t inter_dist = std::numeric_limits<real_t>::infinity();

	public:
		Trace(const Ray& _ray): ray(_ray){}

		// we have `const Ray&, const Renderable*` member, so forbid copy of a Trace
		Trace(const Trace&) = delete;
		Trace& operator = (const Trace&) = delete;

		virtual ~Trace(){ }

		virtual const Renderable* get_obj() const = 0;

		virtual Vec intersection_point() const {
			m_assert(std::isfinite(inter_dist) && inter_dist >= 0);
			return ray.get_dist(inter_dist);
		}

		virtual bool intersect(real_t) const = 0;

		virtual real_t intersection_dist() const = 0;

		virtual Vec normal() const = 0;
		// return Vec::zero if no normal exists
		// return a `normalized` normal vector

		virtual real_t get_forward_density() const
		{ return ray.density; }

		virtual bool contain() const
		{ return false; }

		virtual shared_ptr<Surface> get_property() const {
			shared_ptr<Surface> ret = get_obj()->get_texture()->get_property();
			if (ret) return ret;
			return transform_get_property();
		}

		virtual IntersectInfo get_intersect_info() const {
			return IntersectInfo{normal(),
					intersection_point(),
					get_property(),
					get_forward_density(),
					intersection_dist(),
					contain()};
		}
};

