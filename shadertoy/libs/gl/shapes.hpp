#pragma once
#include <vector>
#include <cmath>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "opengl.hpp"

namespace gl {

template <typename Mesh>
class shape_generator
{
public:
	Mesh cube();
	Mesh box(glm::vec3 const & half_extents);
	Mesh sphere(float r = 1.0f, size_t hsegments = 30, size_t vsegments = 20);
	Mesh cylinder(float r, float h, size_t segments = 20);
	Mesh open_cylinder(float r, float h, size_t segments = 20);
	Mesh tube(float r, float h, size_t segments = 20) {return open_cylinder(r, h, segments);}
	Mesh cone(float r, float h, size_t segments = 20);  //!< kuzel
	Mesh disk(float r, size_t segments = 20);
	Mesh circle(float r = 1.0f, size_t segments = 20);  //!< kruh v xz rovine
	Mesh circle_xy(float r = 1.0f, size_t segments = 20);
	Mesh quad();
	Mesh quad_xy();
	Mesh quad_xz();
	Mesh plane();
	Mesh plane_xy();
	Mesh plane_xz();
	Mesh axis();
	Mesh ellipse();  // TODO: implement
	Mesh ellipsoid();  // 3d ellipse TODO: implement
	Mesh pyramid();  // TODO: implement
	Mesh torus();  // (kobliha) TODO: implement
	Mesh annulus(float r1, float r2, size_t segments = 20);
	Mesh ring(float r1, float r2, size_t segments = 20) {return annulus(r1, r2, segments);}
};


//! vytvori kocku zo stredom v bode (0,0,0) s dlzkou strany d=1
template <typename Mesh>
Mesh make_cube();

//! vytvori obdlznik zo stredom v (0,0,0) s polstranou velkosti half_extents
template <typename Mesh>
Mesh make_box(glm::vec3 const & half_extents);

//! vytvori gulu zo stredom v bode (0,0,0) a polomerom r
template <typename Mesh>
Mesh make_sphere(float r = 1.0f, size_t hsegments = 30, size_t vsegments = 20);

//! vytvori cylinder zo stredom v bode (0,0,0) a polomerom r
template <typename Mesh>
Mesh make_cylinder(float r, float h, size_t segments = 20);

//! vytvori zhora, zdola otvoreny cylinder zo stredom v bode (0,0,0) a polomerom r
template <typename Mesh>
Mesh make_open_cylinder(float r, float h, size_t segments = 20);

//! vytvori kuzel zo stredom v bode (0,0,0) a polomerom zakladne r
template <typename Mesh>
Mesh make_cone(float r, float h, size_t segments = 20);

//! vytvori disk zo stredom v bode (0,0,0) s polomerom r
template <typename Mesh>
Mesh make_disk(float r, size_t segments = 20);

//! vytvori kruh zo stredom v bode (0,0,0) s polomerom r v rovine xz
template <typename Mesh>
Mesh make_circle(float r = 1.0f, size_t segments = 20);

template <typename Mesh>
Mesh make_circle_xy(float r = 1.0f, size_t segments = 20);

//! disk s dierou zo stredom v bode (0,0,0)
template <typename Mesh>
Mesh make_annulus(float r1, float r2, size_t segments = 20);

template <typename Mesh>
Mesh make_quad_xy();  //!< (-1,-1), (1,1)

template <typename Mesh>
Mesh make_unit_quad_xy();  //!< (0,0), (1,1)

template <typename Mesh>
Mesh make_quad_xy(glm::vec2 const & origin, float size);

template <typename Mesh>
Mesh make_quad_xz();

template <typename Mesh>
Mesh make_quad_xz(glm::vec2 const & origin, float size);

template <typename Mesh>
Mesh make_quad_zy();

template <typename Mesh>
Mesh make_quad_zy(glm::vec2 const & origin, float size);

// pociatok roviny je v lavom dolnom rohu, w a h je pocet vrcholov roviny v dannom smere
template <typename Mesh>
Mesh make_plane_xy(unsigned w, unsigned h);

template <typename Mesh>
Mesh make_plane_xy(glm::vec3 const & origin, float size, unsigned w, unsigned h);

template <typename Mesh>
Mesh make_plane_xz(glm::vec3 const & origin = glm::vec3{0,0,0}, float size = 1.0f, unsigned w = 11, unsigned h = 11);

template <typename Mesh>
Mesh make_plane_xz(unsigned w, unsigned h, float size = 1.0f);  // TODO: odstran

//! vytvori osi ukazujuce v smere x, y a z s pociatkom v bode (0,0,0)
template <typename Mesh>
Mesh make_axis();


template <typename Mesh>
Mesh shape_generator<Mesh>::cube()
{
	return make_cube<Mesh>();
}

template <typename Mesh>
Mesh shape_generator<Mesh>::box(glm::vec3 const & half_extents)
{
	return make_box<Mesh>(half_extents);
}

template <typename Mesh>
Mesh shape_generator<Mesh>::sphere(float r, size_t hsegments, size_t vsegments)
{
	return make_sphere<Mesh>(r, hsegments, vsegments);
}

template <typename Mesh>
Mesh shape_generator<Mesh>::cylinder(float r, float h, size_t segments)
{
	return make_cylinder<Mesh>(r, h, segments);
}

template <typename Mesh>
Mesh shape_generator<Mesh>::open_cylinder(float r, float h, size_t segments)
{
	return make_open_cylinder<Mesh>(r, h, segments);
}

template <typename Mesh>
Mesh shape_generator<Mesh>::cone(float r, float h, size_t segments)
{
	return make_cone<Mesh>(r, h, segments);
}

template <typename Mesh>
Mesh shape_generator<Mesh>::disk(float r, size_t segments)
{
	return make_disk<Mesh>(r, segments);
}

template <typename Mesh>
Mesh shape_generator<Mesh>::circle(float r, size_t segments)
{
	return make_circle<Mesh>(r, segments);
}

template <typename Mesh>
Mesh shape_generator<Mesh>::circle_xy(float r, size_t segments)
{
	return make_circle_xy<Mesh>(r, segments);
}

template <typename Mesh>
Mesh shape_generator<Mesh>::quad_xy()
{
	return make_quad_xy<Mesh>();
}

template <typename Mesh>
Mesh shape_generator<Mesh>::annulus(float r1, float r2, size_t segments)
{
	return make_annulus<Mesh>(r1, r2, segments);
}


template <typename Mesh>
Mesh make_cube()
{
	return make_box<Mesh>(glm::vec3{.5});
}

template <typename Mesh>
Mesh make_box(glm::vec3 const & half_extents)
{
	glm::vec3 const & h = half_extents;

	float verts[6*4*(3+3)] = {  // position:3, normal:3
		// front
		-h.x, -h.y, h.z,  0, 0, 1,
		h.x, -h.y, h.z,  0, 0, 1,
		h.x, h.y, h.z,  0, 0, 1,
		-h.x, h.y, h.z,  0, 0, 1,
		// right
		h.x, -h.y, h.z,  1, 0, 0,
		h.x, -h.y, -h.z,  1, 0, 0,
		h.x, h.y, -h.z,  1, 0, 0,
		h.x, h.y, h.z,  1, 0, 0,
		// top
		-h.x, h.y, h.z,  0, 1, 0,
		h.x, h.y, h.z,  0, 1, 0,
		h.x, h.y, -h.z,  0, 1, 0,
		-h.x, h.y, -h.z,  0, 1, 0,
		// bottom
		-h.x, -h.y, -h.z,  0, -1, 0,
		h.x, -h.y, -h.z,  0, -1, 0,
		h.x, -h.y, h.z,  0, -1, 0,
		-h.x, -h.y, h.z,  0, -1, 0,
		// back
		h.x, -h.y, -h.z,  0, 0, -1,
		-h.x, -h.y, -h.z,  0, 0, -1,
		-h.x, h.y, -h.z,  0, 0, -1,
		h.x, h.y, -h.z,  0, 0, -1,
		// left
		-h.x, -h.y, -h.z,  -1, 0, 0,
		-h.x, -h.y, h.z,  -1, 0, 0,
		-h.x, h.y, h.z,  -1, 0, 0,
		-h.x, h.y, -h.z,  -1, 0, 0
	};

	unsigned indices[] = {
		0, 1, 2,  2, 3, 0,
		4, 5, 6,  6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
	};

	Mesh m{verts, sizeof(verts), indices, 2*6*3};
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 0},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 3*sizeof(GLfloat)}});

	return m;
}

template <typename Mesh>
Mesh make_sphere(float r, size_t hsegments, size_t vsegments)
{
	float dp = M_PI / (float)vsegments;  // delta phi
	float dt = 2.0*M_PI / (float)hsegments;  // delta theta

	std::vector<float> verts;  // position:3, texcoord:2, normal:3
	for (size_t i = 0; i <= vsegments; ++i)
	{
		float phi = i*dp;
		for (size_t j = 0; j <= hsegments; ++j)  // horizontalne kruhy
		{
			float theta = j*dt;
			glm::vec3 n{sin(phi)*sin(theta), cos(phi), sin(phi)*cos(theta)};
			glm::vec3 p = r*n;
			glm::vec2 uv{1.0f - j/(float)hsegments, 1.0f - i/(float)vsegments};
			verts.push_back(p.x);
			verts.push_back(p.y);
			verts.push_back(p.z);
			verts.push_back(uv.x);
			verts.push_back(uv.y);
			verts.push_back(n.x);
			verts.push_back(n.y);
			verts.push_back(n.z);
		}
	}

	std::vector<unsigned> inds;
	for (size_t i = 0; i < vsegments*(hsegments+1); ++i)
	{
		inds.push_back(i + hsegments + 1);
		inds.push_back(i+1);
		inds.push_back(i);

		inds.push_back(i);
		inds.push_back(i + hsegments);
		inds.push_back(i + hsegments + 1);
	}

	Mesh m{verts.data(), verts.size()*sizeof(float), inds.data(), inds.size()};
	unsigned stride = (3+2+3)*sizeof(GLfloat);
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, stride, 0},  // position:3
		typename Mesh::vertex_attribute_type{1, 2, GL_FLOAT, stride, 3*sizeof(GLfloat)},  // texcoord:2
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, stride, (3+2)*sizeof(GLfloat)}});  // normal:3

	return m;
}

template <typename Mesh>
Mesh make_cylinder(float r, float h, size_t segments)
{
	float angle = 2.0*M_PI / (float)segments;

	// body
	std::vector<glm::vec3> verts;  // position:3, normal:3
	for (int i = 0; i < segments; ++i)  // bottom
	{
		glm::vec3 n{cos(i*angle), 0, sin(i*angle)};
		verts.push_back(r*n);
		verts.push_back(n);
	}

	int size = verts.size();  // top
	for (int i = 0; i < size; i += 2)
	{
		verts.push_back(verts[i]+glm::vec3{0,h,0});
		verts.push_back(verts[i+1]);
	}

	std::vector<unsigned> indices;
	for (int i = 0; i < segments-1; ++i)
	{
		indices.push_back(i+1);
		indices.push_back(i);
		indices.push_back(segments+i);
		indices.push_back(segments+i);
		indices.push_back(segments+i+1);
		indices.push_back(i+1);
	}
	indices.push_back(0);
	indices.push_back(segments-1);
	indices.push_back(segments+segments-1);
	indices.push_back(segments+segments-1);
	indices.push_back(segments);
	indices.push_back(0);

	// bottom disk
	verts.push_back(glm::vec3{0,0,0});
	verts.push_back(glm::vec3{0,-1,0});
	for (int i = 0; i < 2*segments; i += 2)
	{
		verts.push_back(verts[i]);
		verts.push_back(glm::vec3{0,-1,0});
	}

	unsigned off = 2*segments;
	for (int i = 0; i < segments-1; ++i)
	{
		indices.push_back(off);
		indices.push_back(off+1+i);
		indices.push_back(off+1+i+1);
	}
	indices.push_back(off);
	indices.push_back(off+segments);
	indices.push_back(off+1);

	// top disk
	verts.push_back(glm::vec3{0,h,0});
	verts.push_back(glm::vec3{0,1,0});
	for (int i = 0; i < 2*segments; i += 2)
	{
		verts.push_back(verts[2*segments+i]);
		verts.push_back(glm::vec3{0,1,0});
	}

	off = 3*segments+1;
	for (int i = 0; i < segments-1; ++i)
	{
		indices.push_back(off+1+i+1);
		indices.push_back(off+1+i);
		indices.push_back(off);
	}
	indices.push_back(off+1);
	indices.push_back(off+segments);
	indices.push_back(off);

	Mesh m{verts.data(), verts.size()*sizeof(glm::vec3), indices.data(), indices.size()};
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 0},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 3*sizeof(GLfloat)}});

	return m;
}

template <typename Mesh>
Mesh make_open_cylinder(float r, float h, size_t segments)
{
	float angle = 2.0*M_PI / (float)segments;

	// body
	std::vector<glm::vec3> verts;  // position:3, normal:3
	for (int i = 0; i < segments; ++i)  // bottom
	{
		glm::vec3 n{cos(i*angle), 0, sin(i*angle)};
		verts.push_back(r*n);
		verts.push_back(n);
	}

	int size = verts.size();  // top
	for (int i = 0; i < size; i += 2)
	{
		verts.push_back(verts[i]+glm::vec3{0,h,0});
		verts.push_back(verts[i+1]);
	}

	std::vector<unsigned> indices;  // TODO: use triangle-strip
	for (int i = 0; i < segments-1; ++i)
	{
		indices.push_back(i+1);
		indices.push_back(i);
		indices.push_back(segments+i);
		indices.push_back(segments+i);
		indices.push_back(segments+i+1);
		indices.push_back(i+1);
	}
	indices.push_back(0);
	indices.push_back(segments-1);
	indices.push_back(segments+segments-1);
	indices.push_back(segments+segments-1);
	indices.push_back(segments);
	indices.push_back(0);

	Mesh m{verts.data(), verts.size()*sizeof(glm::vec3), indices.data(), indices.size()};
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 0},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 3*sizeof(GLfloat)}});

	return m;
}

template <typename Mesh>
Mesh make_cone(float r, float h, size_t segments)
{
	float angle = 2.0*M_PI / (float)segments;

	// body
	std::vector<glm::vec3> verts;  // position:3, normal:3
	for (int i = 0; i < segments; ++i)  // bottom disk
	{
		glm::vec3 n{cos(i*angle), 0, sin(i*angle)};
		verts.push_back(r*n);
		verts.push_back(n);
	}

	verts.push_back(glm::vec3{0,h,0});  // top peak
	verts.push_back(glm::vec3{0,1,0});

	std::vector<unsigned> indices;
	for (int i = 0; i < segments-1; ++i)
	{
		indices.push_back(i+1);
		indices.push_back(i);
		indices.push_back(segments);
	}
	indices.push_back(0);
	indices.push_back(segments-1);
	indices.push_back(segments);

	// TODO: smooth normals

	// bottom disk
	verts.push_back(glm::vec3{0,0,0});
	verts.push_back(glm::vec3{0,-1,0});
	for (int i = 0; i < 2*segments; i += 2)
	{
		verts.push_back(verts[i]);
		verts.push_back(glm::vec3{0,-1,0});
	}

	unsigned off = segments+1;
	for (int i = 0; i < segments-1; ++i)
	{
		indices.push_back(off);
		indices.push_back(off+1+i);
		indices.push_back(off+1+i+1);
	}
	indices.push_back(off);
	indices.push_back(off+segments);
	indices.push_back(off+1);

	Mesh m{verts.data(), verts.size()*sizeof(glm::vec3), indices.data(), indices.size()};
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 0},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 3*sizeof(GLfloat)}});

	return m;
}

template <typename Mesh>
Mesh make_disk(float r, size_t segments)
{
	float angle = 2.0*M_PI / (float)segments;

	std::vector<glm::vec3> verts;
	verts.reserve(2*(segments+2));  // position:3, normal:3
	verts.push_back(glm::vec3{0,0,0});
	verts.push_back(glm::vec3{0,1,0});
	for (int i = 0; i < segments+1; ++i)
	{
		verts.push_back(glm::vec3{r * cos(i*angle), 0, r * sin(i*angle)});
		verts.push_back(glm::vec3{0,1,0});
	}

	std::vector<unsigned> indices;
	indices.reserve(segments+2);  // triangle-fan
	indices.push_back(0);
	for (unsigned i = segments+1; i > 0; --i)
		indices.push_back(i);

	Mesh m{verts.data(), verts.size()*2*sizeof(glm::vec3), indices.data(), indices.size()};
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 0},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 3*sizeof(GLfloat)}});
	m.draw_mode(Mesh::render_primitive_type::triangle_fan);
	return m;
}

template <typename Mesh>
Mesh make_circle(float r, size_t segments)
{
	float angle = 2.0*M_PI / (float)segments;

	std::vector<glm::vec3> verts;
	verts.reserve(2*(segments+1));  // position:3, normal:3
	for (size_t i = 0; i < segments+1; ++i)
	{
		verts.push_back(glm::vec3{r * cos(i*angle), 0, r * sin(i*angle)});
		verts.push_back(glm::vec3{0,1,0});
	}

	std::vector<unsigned> indices;
	indices.reserve(segments+1);  // line-strip
	for (int i = (int)segments; i >= 0; --i)
		indices.push_back(i);

	Mesh m{verts.data(), verts.size()*2*sizeof(glm::vec3), indices.data(), indices.size()};
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 0},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 3*sizeof(GLfloat)}});
	m.draw_mode(Mesh::render_primitive_type::line_strip);
	return m;
}

template <typename Mesh>
Mesh make_circle_xy(float r, size_t segments)
{
	float angle = 2.0*M_PI / (float)segments;

	std::vector<glm::vec3> verts;
	verts.reserve(2*(segments+1));  // position:3, normal:3
	for (size_t i = 0; i < segments+1; ++i)
	{
		verts.push_back(glm::vec3{r * cos(i*angle), r * sin(i*angle), 0});
		verts.push_back(glm::vec3{0,1,0});
	}

	std::vector<unsigned> indices;
	indices.reserve(segments+1);  // line-strip
	for (int i = (int)segments; i >= 0; --i)
		indices.push_back(i);

	Mesh m{verts.data(), verts.size()*2*sizeof(glm::vec3), indices.data(), indices.size()};
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 0},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 3*sizeof(GLfloat)}});
	m.draw_mode(Mesh::render_primitive_type::line_strip);
	return m;
}

template <typename Mesh>
Mesh make_annulus(float r1, float r2, size_t segments)
{
	std::vector<float> verts;  // position:3, texcoord:2, normal:3

	float dw = (2*M_PI) / (float)segments;
	for (size_t i = 0; i < segments+1; ++i)
	{
		float angle = i*dw;
		glm::vec3 unit_p{cos(angle), 0, sin(angle)};
		glm::vec3 v1 = r1*unit_p;
		glm::vec2 uv1{(angle*r1) / (2*M_PI), 0};
		glm::vec3 n{0,1,0};
		glm::vec3 v2 = r2*unit_p;
		glm::vec2 uv2{(angle*r2) / (2*M_PI), 1};
		verts.push_back(v1.x);
		verts.push_back(v1.y);
		verts.push_back(v1.z);
		verts.push_back(uv1.x);
		verts.push_back(uv1.y);
		verts.push_back(n.x);
		verts.push_back(n.y);
		verts.push_back(n.z);
		verts.push_back(v2.x);
		verts.push_back(v2.y);
		verts.push_back(v2.z);
		verts.push_back(uv2.x);
		verts.push_back(uv2.y);
		verts.push_back(n.x);
		verts.push_back(n.y);
		verts.push_back(n.z);
	}

	std::vector<unsigned> indices;
	for (size_t i = 2; i < 2*(segments+1); i += 2)
	{
		indices.push_back(i);
		indices.push_back(i+1);
		indices.push_back(i-1);
		indices.push_back(i-1);
		indices.push_back(i-2);
		indices.push_back(i);
	}

	Mesh m{verts.data(), verts.size()*sizeof(float), indices.data(), indices.size()};
	unsigned stride = (3+2+3)*sizeof(GLfloat);
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, stride, 0},
		typename Mesh::vertex_attribute_type{1, 2, GL_FLOAT, stride, 3*sizeof(GLfloat)},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, stride, (3+2)*sizeof(GLfloat)}});

	return m;
}

template <typename Mesh>
Mesh make_quad_xy()
{
	return make_quad_xy<Mesh>(glm::vec2(-1,-1), 2.0f);
}

template <typename Mesh>
Mesh make_unit_quad_xy()
{
	return make_quad_xy<Mesh>(glm::vec2{0,0}, 1);
}

template <typename Mesh>
Mesh make_quad_xy(glm::vec2 const & origin, float size)
{
	auto & o = origin;
	float verts[(3+2+3)*4] = {  // position:3, texcoord:2, normal:3
		o.x, o.y, 0,  0, 0,  0, 0, 1,
		o.x + size, o.y, 0,  1, 0,  0, 0, 1,
		o.x + size, o.y + size, 0,  1, 1,  0, 0, 1,
		o.x, o.y + size, 0,  0,1,  0, 0, 1
	};

	unsigned indices[6] = {0,1,2, 2,3,0};

	Mesh m{verts, sizeof(verts), indices, 6};
	unsigned stride = (3+2+3)*sizeof(float);
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, stride, 0},
		typename Mesh::vertex_attribute_type{1, 2, GL_FLOAT, stride, 3*sizeof(float)},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, stride, (3+2)*sizeof(float)}});

	return m;
}

template <typename Mesh>
Mesh make_quad_xz()
{
	return make_quad_xz<Mesh>(glm::vec2(-1,-1), 2.0f);
}

template <typename Mesh>
Mesh make_quad_xz(glm::vec2 const & origin, float size)
{
	auto & o = origin;
	float verts[(3+2+3)*4] = {  // position:3, texcoord:2, normal:3
		o.x, 0, -o.y,  0, 0,  0, 1, 0,
		o.x + size, 0, -o.y,  1, 0,  0, 1, 0,
		o.x + size, 0, -(o.y + size),  1, 1,  0, 1, 0,
		o.x, 0, -(o.y + size),  0, 1,  0, 1, 0
	};

	unsigned indices[6] = {0,1,2,  2,3,0};

	Mesh m{verts, sizeof(verts), indices, 6};
	unsigned stride = (3+2+3)*sizeof(float);
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, stride, 0},
		typename Mesh::vertex_attribute_type{1, 2, GL_FLOAT, stride, 3*sizeof(float)},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, stride, (3+2)*sizeof(float)}});

	return m;
}

template <typename Mesh>
Mesh make_quad_zy()
{
	return make_quad_zy<Mesh>(glm::vec2{-1,-1}, 2.0f);
}

template <typename Mesh>
Mesh make_quad_zy(glm::vec2 const & origin, float size)
{
	auto & o = origin;
	float verts[(3+2+3)*4] = {  // position:3, texcoord:2, normal:3
		0, o.y, o.x,  0, 0,  1, 0, 0,
		0, o.y, o.x + size,  1, 0,  1, 0, 0,
		0, o.y + size, o.x + size,  1, 1,  1, 0, 0,
		0, o.y + size, o.x,  0, 1,  1, 0, 0
	};

	unsigned indices[6] = {0,1,2,  2,3,0};

	Mesh m{verts, sizeof(verts), indices, 6};
	unsigned stride = (3+2+3)*sizeof(float);
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, stride, 0},
		typename Mesh::vertex_attribute_type{1, 2, GL_FLOAT, stride, 3*sizeof(float)},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, stride, (3+2)*sizeof(float)}});

	return m;
}

template <typename Mesh>
Mesh make_plane_xy(unsigned w, unsigned h)
{
	assert(w > 1 && h > 1 && "invalid dimensions");

	// vertices
	float dx = 1.0f/(w-1);
	float dy = 1.0f/(h-1);
	std::vector<float> verts((3+2+3)*w*h);  // position:3, texcoord:2, normal:3

	float * vdata = verts.data();
	for (int j = 0; j < h; ++j)
	{
		float py = j*dy;
		for (int i = 0; i < w; ++i)
		{
			float px = i*dx;
			*vdata++ = px;  // position
			*vdata++ = py;
			*vdata++ = 0;
			*vdata++ = px;  // texcoord
			*vdata++ = py;
			*vdata++ = 0;  // normal
			*vdata++ = 0;
			*vdata++ = 1;
		}
	}

	// indices
	unsigned nindices = 2*(w-1)*(h-1)*3;
	std::vector<unsigned> indices(nindices);
	unsigned * idata = indices.data();
	for (int j = 0; j < h-1; ++j)
	{
		unsigned yoffset = j*w;
		for (int i = 0; i < w-1; ++i)
		{
			int n = i + yoffset;
			*(idata++) = n;
			*(idata++) = n+1;
			*(idata++) = n+1+w;
			*(idata++) = n+1+w;
			*(idata++) = n+w;
			*(idata++) = n;
		}
	}

	Mesh m{verts.data(), (3+2+3)*verts.size(), indices.data(), indices.size()};
	unsigned stride = (3+2+3)*sizeof(float);
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, stride, 0},
		typename Mesh::vertex_attribute_type{1, 2, GL_FLOAT, stride, 3*sizeof(float)},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, stride, (3+2)*sizeof(float)}});

	return m;
}

template <typename Mesh>
Mesh make_plane_xy(glm::vec3 const & origin, float size, unsigned w, unsigned h)
{
	assert(w > 1 && h > 1 && "invalid dimensions");

	// vertices
	float dx = 1.0f/(w-1);
	float dy = 1.0f/(h-1);
	std::vector<float> verts((3+2+3)*w*h);  // position:3, texcoord:2, normal:3

	float * vdata = verts.data();
	for (int j = 0; j < h; ++j)
	{
		float py = j*dy;
		for (int i = 0; i < w; ++i)
		{
			float px = i*dx;
			*vdata++ = origin.x + size*px;  // position
			*vdata++ = origin.y + size*py;
			*vdata++ = origin.z;
			*vdata++ = px;  // texcoord
			*vdata++ = py;
			*vdata++ = 0;  // normal
			*vdata++ = 0;
			*vdata++ = 1;
		}
	}

	// indices
	unsigned nindices = 2*(w-1)*(h-1)*3;
	std::vector<unsigned> indices(nindices);

	unsigned * idata = &indices[0];
	for (int j = 0; j < h-1; ++j)
	{
		unsigned yoffset = j*w;
		for (int i = 0; i < w-1; ++i)
		{
			int n = i + yoffset;
			*(idata++) = n;
			*(idata++) = n+1;
			*(idata++) = n+1+w;
			*(idata++) = n+1+w;
			*(idata++) = n+w;
			*(idata++) = n;
		}
	}

	Mesh m{verts.data(), (3+2+3)*verts.size(), indices.data(), indices.size()};
	unsigned stride = (3+2+3)*sizeof(float);
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, stride, 0},
		typename Mesh::vertex_attribute_type{1, 2, GL_FLOAT, stride, 3*sizeof(float)},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, stride, (3+2)*sizeof(float)}});

	return m;
}

template <typename Mesh>
Mesh make_plane_xz(glm::vec3 const & origin, float size, unsigned w, unsigned h)
{
	assert(w > 1 && h > 1 && "invalid dimensions");

	// vertices
	float dx = 1.0f/(w-1);
	float dy = 1.0f/(h-1);
	std::vector<float> verts((3+2+3+3)*w*h);  // position:3, texcoord:2, normal:3, tangent:3

	float * vdata = verts.data();
	for (int j = 0; j < h; ++j)
	{
		float pz = j*dy;
		for (int i = 0; i < w; ++i)
		{
			float px = i*dx;
			*vdata++ = origin.x + size*px;  // position
			*vdata++ = origin.y;
			*vdata++ = origin.z - (size*pz);
			*vdata++ = px;  // texcoord
			*vdata++ = pz;
			*vdata++ = 0;  // normal
			*vdata++ = 1;
			*vdata++ = 0;
			*vdata++ = 1;  // tangent
			*vdata++ = 0;
			*vdata++ = 0;
		}
	}

	// indices
	unsigned nindices = 2*(w-1)*(h-1)*3;
	std::vector<unsigned> indices(nindices);

	unsigned * idata = indices.data();
	for (int j = 0; j < h-1; ++j)
	{
		unsigned yoffset = j*w;
		for (int i = 0; i < w-1; ++i)
		{
			int n = i + yoffset;
			*(idata++) = n;
			*(idata++) = n+1;
			*(idata++) = n+1+w;
			*(idata++) = n+1+w;
			*(idata++) = n+w;
			*(idata++) = n;
		}
	}

	Mesh m{verts.data(), 4*verts.size(), indices.data(), indices.size()};
	unsigned stride = (3+2+3+3)*sizeof(float);
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, stride, 0},
		typename Mesh::vertex_attribute_type{1, 2, GL_FLOAT, stride, 3*sizeof(float)},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, stride, (3+2)*sizeof(float)},
		typename Mesh::vertex_attribute_type{3, 3, GL_FLOAT, stride, (3+2+3)*sizeof(float)}});

	return m;
}

template <typename Mesh>
Mesh make_plane_xz(unsigned w, unsigned h, float size)
{
	assert(w > 1 && h > 1 && "invalid dimensions");

	// vertices
	float dx = 1.0f/(w-1);
	float dy = 1.0f/(h-1);
	std::vector<float> verts((3+2+3+3)*w*h);  // position:3, texcoord:2, normal:3, tangent:3

	float * vdata = verts.data();
	for (int j = 0; j < h; ++j)
	{
		float pz = j*dy;
		for (int i = 0; i < w; ++i)
		{
			float px = i*dx;
			*vdata++ = size*px;  // position
			*vdata++ = 0;
			*vdata++ = -size*pz;
			*vdata++ = px;  // texcoord
			*vdata++ = pz;
			*vdata++ = 0;  // normal
			*vdata++ = 1;
			*vdata++ = 0;
			*vdata++ = 1;  // tangent
			*vdata++ = 0;
			*vdata++ = 0;
		}
	}

	// indices
	unsigned nindices = 2*(w-1)*(h-1)*3;
	std::vector<unsigned> indices(nindices);
	unsigned * idata = &indices[0];
	for (int j = 0; j < h-1; ++j)
	{
		unsigned yoffset = j*w;
		for (int i = 0; i < w-1; ++i)
		{
			int n = i + yoffset;
			*(idata++) = n;
			*(idata++) = n+1;
			*(idata++) = n+1+w;
			*(idata++) = n+1+w;
			*(idata++) = n+w;
			*(idata++) = n;
		}
	}

	Mesh m{verts.data(), 4*verts.size(), indices.data(), indices.size()};
	unsigned stride = (3+2+3+3)*sizeof(float);
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, stride, 0},
		typename Mesh::vertex_attribute_type{1, 2, GL_FLOAT, stride, 3*sizeof(float)},
		typename Mesh::vertex_attribute_type{2, 3, GL_FLOAT, stride, (3+2)*sizeof(float)},
		typename Mesh::vertex_attribute_type{3, 3, GL_FLOAT, stride, (3+2+3)*sizeof(float)}});

	return m;
}

template <typename Mesh>
Mesh make_axis()
{
	std::vector<float> vertices{  // position, color
		0,0,0, 1,0,0,
		1,0,0, 1,0,0,
		0,0,0, 0,1,0,
		0,1,0, 0,1,0,
		0,0,0, 0,0,1,
		0,0,1, 0,0,1};

	std::vector<unsigned> indices{0,1, 2,3, 4,5};

	Mesh m{vertices.data(), vertices.size()*sizeof(float), indices.data(), indices.size()};
	m.attach_attributes({
		typename Mesh::vertex_attribute_type{0, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 0},
		typename Mesh::vertex_attribute_type{1, 3, GL_FLOAT, (3+3)*sizeof(GLfloat), 3*sizeof(GLfloat)}});
	m.draw_mode(Mesh::render_primitive_type::lines);

	return m;
}

}  // gl
