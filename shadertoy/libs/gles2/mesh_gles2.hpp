// experimental mesh implementation
#pragma once
#include <vector>
#include <string>
#include <memory>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace gles2 {

enum class buffer_usage {  //!< \sa glBufferData():usage
	stream_draw,
	stream_read,
	stream_copy,
	static_draw,
	static_read,
	static_copy,
	dynamic_draw,
	dynamic_read,
	dynamic_copy
};

enum class buffer_target {  //!< \sa glBufferData():target
	array,
	atomic_counter,
	copy_read,
	copy_write,
	draw_indirect,
	dispatch_indirect,
	element_array,
	pixel_pack,
	pixel_unpack,
	query,
	shader_storage,
	texture,
	transform_feedback,
	uniform
};

enum class render_primitive {  //!< \sa glDrawElements():mode
	points,
	line_strip,
	line_loop,
	lines,
	triangle_strip,
	triangle_fan,
	triangles,
};

class gpu_buffer
{
public:
	gpu_buffer() {}  //!< create unusable (but safe destructible) buffer
	gpu_buffer(buffer_target target, size_t size, buffer_usage usage);
	gpu_buffer(buffer_target target, void const * buf, size_t size, buffer_usage usage);
	gpu_buffer(gpu_buffer && other);
	~gpu_buffer();
	void data(buffer_target target, void const * buf, size_t size, unsigned offset = 0);
	unsigned id() const;
	void bind(buffer_target target) const;  // TODO: bind() with native parameter
	void operator=(gpu_buffer && other);

	gpu_buffer(gpu_buffer const &) = delete;
	void operator=(gpu_buffer const &) = delete;

private:
	unsigned _id = 0;
};

struct attribute  // buffer attribute
{
	unsigned index;  //!< attribute index from shader program (glGetAttribLocation())
	int size;  //!< number of attribute elements (4 for vec4, 3 for vec3, ...)
	int type;  //!< GL_FLOAT, ...
	int normalized;  //!< GL_FALSE or GL_TRUE
	unsigned stride;  //!< a vertex data size ((3+2+3)*sizeof(float) in a case of position:3, coords:2, normal:3)
	int start_idx; //!< index where the data starts

	attribute(unsigned index, int size, int type, unsigned stride, int start_idx = 0, int normalized = 0);
};

class mesh
{
public:
	using vertex_attribute_type = attribute;
	using render_primitive_type = render_primitive;

	mesh() {}
	mesh(size_t vbuf_size_in_bytes, size_t index_count, buffer_usage usage = buffer_usage::static_draw);
	mesh(void const * vbuf, size_t vbuf_size, unsigned const * ibuf, size_t ibuf_size, buffer_usage usage = buffer_usage::static_draw);
	mesh(mesh && other);
	virtual ~mesh() {}
	void render() const;
	void attach_attributes(std::initializer_list<attribute> attribs);
	void append_attribute(attribute const & a);
	void draw_mode(render_primitive_type mode);
	void data(void const * vsubbuf, unsigned size, unsigned offset = 0);
	void data(void const * vbuf, unsigned vbuf_size, unsigned const * ibuf, unsigned ibuf_size);
	void data(void const * vsubbuf, unsigned vsubbuf_size, unsigned vsubbuf_offset, unsigned const * isubbuf, unsigned isubbuf_size, unsigned isubbuf_offset);
	void operator=(mesh && other);
	void attribute_location(unsigned attr_idx, int loc_value);  //!< sets attribute location
	void attribute_location(std::initializer_list<int> locations);

	mesh(mesh const &) = delete;
	void operator=(mesh const &) = delete;

private:
	gpu_buffer _vbuf, _ibuf;  //!< vertex and index buffers
	size_t _nindices;
	std::vector<attribute> _attribs;
	int _draw_mode;  //!< GL_POINTS, GL_LINES, GL_TRIANGLES, ... \sa glDrawElements()
};

struct vertex
{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;

	vertex() {}
	vertex(glm::vec3 const & position) : vertex{position, glm::vec2{0,0}} {}
	vertex(glm::vec3 const & position, glm::vec2 const & uv) : position(position), uv(uv), normal(0,0,0), tangent(0,0,1) {}
	vertex(glm::vec3 const & position, glm::vec3 const & normal) : vertex{position, glm::vec2{0,0}, normal} {}
	vertex(glm::vec3 const & position, glm::vec2 const & uv, glm::vec3 const & normal) : position(position), uv(uv), normal(normal), tangent(0,0,1) {}
	vertex(glm::vec3 const & position, glm::vec2 const & uv, glm::vec3 const & normal, glm::vec3 const & tangent) : position(position), uv(uv), normal(normal), tangent(tangent) {}
};

mesh mesh_from_vertices(std::vector<vertex> const & verts, std::vector<unsigned> const & indices);

}  // gles2
