#pragma once
#include <string>
#include <memory>
#include "gles2/texture_gles2.hpp"
#include "gles2/program_gles2.hpp"
#include "gles2/texture_loader_gles2.hpp"

template <typename R>
std::shared_ptr<R> from_file(std::string const & fname);

template <typename R>
std::shared_ptr<R> from_memory(std::string const & source);


template <>
inline std::shared_ptr<gles2::shader::program> from_file<gles2::shader::program>(std::string const & fname)
{
	return std::shared_ptr<gles2::shader::program> {
		new gles2::shader::program{fname}
	};
}

template <>
inline std::shared_ptr<gles2::texture2d> from_file<gles2::texture2d>(std::string const & fname)
{
	return std::shared_ptr<gles2::texture2d>{
		new gles2::texture2d{gles2::texture_from_file(fname)}
	};
}

template <>
inline std::shared_ptr<gles2::shader::program> from_memory(std::string const & source)
{
	std::shared_ptr<gles2::shader::program> result{new gles2::shader::program{}};
	result->from_memory(source);
	return result;
}


struct resource_holder
{
	void * ptr;

	template <typename R>
	std::shared_ptr<R> as() {return *(std::shared_ptr<R> *)ptr;}
};


template <typename T>
struct resource_holder_impl : public resource_holder
{
	resource_holder_impl() {}

	resource_holder_impl(std::shared_ptr<T> resource)
		: resource{resource}
	{
		ptr = (void *)&this->resource;
	}

	std::shared_ptr<T> resource;
};


class resource_loader
{
public:
	template <typename R>
	std::shared_ptr<R> from_file(std::string const & fname)
	{
		auto it = _lookup.find(fname);
		if (it != _lookup.end())
			return it->second->as<R>();

		resource_holder_impl<R> * h = new resource_holder_impl<R>{::from_file<R>(fname)};
		_lookup[fname] = h;
		return h->resource;
	}

	template <typename R>
	std::shared_ptr<R> from_memory(std::string const & id, std::string const & source)
	{
		auto it = _lookup.find(id);
		if (it != _lookup.end())
			return it->second->as<R>();

		resource_holder_impl<R> * h = new resource_holder_impl<R>{::from_memory<R>(source)};
		_lookup[id] = h;
		return h->resource;
	}

	template <typename R>
	bool insert(std::string const & id, std::shared_ptr<R> resource)
	{
		auto it = _lookup.find(id);
		if (it != _lookup.end())
			return false;

		resource_holder_impl<R> * h = new resource_holder_impl<R>{resource};
		_lookup[id] = h;
		return h->resource;
	}

	template <typename R>
	std::shared_ptr<R> find(std::string const & id)
	{
		auto it = _lookup.find(id);
		if (it != _lookup.end())
			return it->second->as<R>();
		else
			return std::shared_ptr<R>{};
	}

	~resource_loader()
	{
		// free resources
		for (auto const keyval : _lookup)
			delete keyval.second;
	}

private:
	std::map<std::string, resource_holder *> _lookup;
};
