#pragma once
#include <string>
#include <fstream>
#include <stdexcept>
#include <iterator>

namespace io {

class read_lines_range
{
public:
	read_lines_range();
	read_lines_range(std::string const & fname);

	struct iterator : public std::iterator<std::input_iterator_tag, std::string>
	{
		iterator(read_lines_range * r)
			: _r{r}
		{}

		std::string const & operator*() const {return _r->current();}

		iterator & operator++()
		{
			if (!_r->next())
				_r = nullptr;
			return *this;
		}

		bool operator!=(iterator rhs) const {return _r != rhs._r;}

	private:
		read_lines_range * _r;
	};

	iterator begin() const;
	iterator end() const;

private:
	bool next();
	std::string const & current() const;

	std::string _line;
	std::ifstream _fin;
};


/*! read text file lines
\code
for (string const & line : io::read_lines("test.txt"))
	// do something with line
\endcode */
read_lines_range read_lines(std::string const & fname);

}  // io
