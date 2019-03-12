#include <algorithm>
#include <stdexcept>
#include <vector>
#include <initializer_list>
#include <cassert>
#include "gl/opengl.hpp"
#include "mesh_gles2.hpp"

using std::swap;
using std::move;
using std::vector;
using std::runtime_error;
using std::logic_error;
using std::initializer_list;

namespace gles2 {

GLenum opengl_cast(buffer_usage u);
GLenum opengl_cast(buffer_target t);
GLenum opengl_cast(render_primitive p);

gpu_buffer::gpu_buffer(buffer_target target, size_t size, buffer_usage usage)
{
	glGenBuffers(1, &_id);
	GLenum t = opengl_cast(target);
	glBindBuffer(t, _id);
	glBufferData(t, size, nullptr, opengl_cast(usage));
	glBindBuffer(t, 0);  // unbind
	assert(glGetError() == GL_NO_ERROR && "opengl error");
}

gpu_buffer::gpu_buffer(buffer_target target, void const * buf, size_t size, buffer_usage usage)
{
	glGenBuffers(1, &_id);
	GLenum t = opengl_cast(target);
	glBindBuffer(t, _id);
	glBufferData(t, size, buf, opengl_cast(usage));
	glBindBuffer(t, 0);  // unbind
	assert(glGetError() == GL_NO_ERROR && "opengl error");
}

gpu_buffer::gpu_buffer(gpu_buffer && other)
	: _id{other._id}
{
	other._id = 0;
}

void gpu_buffer::operator=(gpu_buffer && other)
{
	swap(_id, other._id);
}

gpu_buffer::~gpu_buffer()
{
	if (_id)
		glDeleteBuffers(1, &_id);
	assert(glGetError() == GL_NO_ERROR && "opengl error");
}

void gpu_buffer::data(buffer_target target, void const * buf, size_t size, unsigned offset)
{
	assert(_id && "uninitialized buffer");
	GLenum t = opengl_cast(target);
	glBindBuffer(t, _id);
	glBufferSubData(t, offset, size, buf);
	glBindBuffer(t, 0);
	assert(glGetError() == GL_NO_ERROR && "opengl error");
}

unsigned gpu_buffer::id() const
{
	return _id;
}

void gpu_buffer::bind(buffer_target target) const
{
	assert(_id && "uninitialized buffer");
	glBindBuffer(opengl_cast(target), _id);
	assert(glGetError() == GL_NO_ERROR && "opengl error");
}

attribute::attribute(unsigned index, int size, int type, unsigned stride, int start_idx, int normalized)
	: index{index}, size{size}, type{type}, normalized{normalized}, stride{stride}, start_idx{start_idx}
{}

mesh::mesh(size_t vbuf_size_in_bytes, size_t index_count, buffer_usage usage)
	: _vbuf{buffer_target::array, vbuf_size_in_bytes, usage}, _ibuf(buffer_target::element_array, index_count*sizeof(unsigned), usage), _nindices{index_count}, _draw_mode{GL_TRIANGLES}
{}

mesh::mesh(void const * vbuf, size_t vbuf_size, unsigned const * ibuf, size_t ibuf_size, buffer_usage usage)
	: _vbuf{buffer_target::array, vbuf, vbuf_size, usage}, _ibuf(buffer_target::element_array, ibuf, ibuf_size*sizeof(unsigned), usage), _nindices{ibuf_size}, _draw_mode{GL_TRIANGLES}
{}

mesh::mesh(mesh && other)
	: _vbuf{move(other._vbuf)}
	, _ibuf{move(other._ibuf)}
	, _nindices{other._nindices}
	, _draw_mode{other._draw_mode}
{
	swap(_attribs, other._attribs);
}

void mesh::operator=(mesh && other)
{
	swap(_nindices, other._nindices);
	swap(_attribs, other._attribs);
	_vbuf = move(other._vbuf);
	_ibuf = move(other._ibuf);
	swap(_draw_mode, other._draw_mode);
}

void mesh::render() const
{
	_vbuf.bind(buffer_target::array);

	for (attribute const & a : _attribs)
	{
		if (a.index == -1)  // invalid attribute (missing in shader program)
			continue;

		glEnableVertexAttribArray(a.index);
		glVertexAttribPointer(a.index, a.size, a.type, a.normalized, a.stride, (GLvoid *)(intptr_t)a.start_idx);

		assert(glGetError() == GL_NO_ERROR && "opengl error");
	}

	_ibuf.bind(buffer_target::element_array);

	glDrawElements(_draw_mode, _nindices, GL_UNSIGNED_INT, 0);

	for (attribute const & a : _attribs)
	{
		if (a.index != -1)  // only valid attributes
			glDisableVertexAttribArray(a.index);
	}

	assert(glGetError() == GL_NO_ERROR && "opengl error");
}

void mesh::attach_attributes(std::initializer_list<attribute> attribs)
{
	_attribs.assign(attribs);
}

void mesh::append_attribute(attribute const & a)
{
	_attribs.push_back(a);
}

void mesh::draw_mode(render_primitive mode)
{
	_draw_mode = opengl_cast(mode);
}

void mesh::data(void const * vsubbuf, unsigned size, unsigned offset)
{
	_vbuf.data(buffer_target::array, vsubbuf, size, offset);
}

void mesh::data(void const * vbuf, unsigned vbuf_size, unsigned const * ibuf, unsigned ibuf_size)
{
	data(vbuf, vbuf_size, 0, ibuf, ibuf_size, 0);
}

void mesh::data(void const * vsubbuf, unsigned vsubbuf_size, unsigned vsubbuf_offset, unsigned const * isubbuf, unsigned isubbuf_size, unsigned isubbuf_offset)
{
	_vbuf.data(buffer_target::array, vsubbuf, vsubbuf_size, vsubbuf_offset);
	_ibuf.data(buffer_target::element_array, isubbuf, isubbuf_size*sizeof(unsigned), isubbuf_offset);
}

void mesh::attribute_location(unsigned attr_idx, int loc_value)
{
	assert(_attribs.size() > attr_idx && "out of range");
	_attribs[attr_idx].index = loc_value;
}

void mesh::attribute_location(initializer_list<int> locations)
{
	assert(_attribs.size() >= locations.size() && "not enough attributes to set");
	int idx = 0;
	for (auto it = locations.begin(); it != locations.end(); ++it, ++idx)
		_attribs[idx].index = *it;
}


mesh mesh_from_vertices(std::vector<vertex> const & verts, std::vector<unsigned> const & indices)
{
	vector<float> vbuf;
	vbuf.resize(verts.size()*(3+2+3+3));

	float * fptr = vbuf.data();
	for (vertex const & v : verts)
	{
		*fptr++ = v.position.x;
		*fptr++ = v.position.y;
		*fptr++ = v.position.z;
		*fptr++ = v.uv.x;
		*fptr++ = v.uv.y;
		*fptr++ = v.normal.x;
		*fptr++ = v.normal.y;
		*fptr++ = v.normal.z;
		*fptr++ = v.tangent.x;
		*fptr++ = v.tangent.y;
		*fptr++ = v.tangent.z;
	}

	mesh m(vbuf.data(), vbuf.size()*sizeof(float), indices.data(), indices.size());
	// TODO: vertex by mal poskytnut attributy
	unsigned stride = (3+2+3+3)*sizeof(GLfloat);
	m.append_attribute(attribute{0, 3, GL_FLOAT, stride});  // position
	m.append_attribute(attribute{1, 2, GL_FLOAT, stride, 3*sizeof(GLfloat)});  // texcoord
	m.append_attribute(attribute{2, 3, GL_FLOAT, stride, (3+2)*sizeof(GLfloat)});  // normal
	m.append_attribute(attribute{3, 3, GL_FLOAT, stride, (3+2+3)*sizeof(GLfloat)});  // tangent

	return m;
}


GLenum opengl_cast(buffer_usage u)
{
	switch (u)
	{
		case buffer_usage::stream_draw: return GL_STREAM_DRAW;
		case buffer_usage::static_draw: return GL_STATIC_DRAW;
		case buffer_usage::dynamic_draw: return GL_DYNAMIC_DRAW;
		default:
			throw logic_error{"bad cast, unknown buffer usage"};
	}
}

GLenum opengl_cast(buffer_target t)
{
	switch (t)
	{
		case buffer_target::array: return GL_ARRAY_BUFFER;
		case buffer_target::element_array: return GL_ELEMENT_ARRAY_BUFFER;
		default:
			throw logic_error{"bad cast, unknown buffer target"};
	}
}

GLenum opengl_cast(render_primitive p)
{
	switch (p)
	{
		case render_primitive::points: return GL_POINTS;
		case render_primitive::line_strip: return GL_LINE_STRIP;
		case render_primitive::line_loop: return GL_LINE_LOOP;
		case render_primitive::lines: return GL_LINES;
		case render_primitive::triangle_strip: return GL_TRIANGLE_STRIP;
		case render_primitive::triangle_fan: return GL_TRIANGLE_FAN;
		case render_primitive::triangles: return GL_TRIANGLES;
		default:
			throw logic_error{"bad cast, unknown render primitive"};
	}
}

}  // gles2
