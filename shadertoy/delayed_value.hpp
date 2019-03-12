#pragma once

template <typename T>
struct delayed_value
{
public:
	delayed_value(T const & init_value = T{})
		: delayed_value{init_value, init_value, 0.0f}
	{}

	delayed_value(T const & init_value, T const & result_value, float delay)
		: _value{init_value}
		, _result_value{result_value}
		, _remains{delay}
	{}

	T const & get() const {return _value;}

	bool update(float dt)
	{
		if (_remains > 0.0f)
		{
			_remains -= dt;
			if (_remains > 0.0f)
				return true;

			_value = _result_value;
		}

		return false;
	}

private:
	T _value, _result_value;
	float _remains;
};

using delayed_bool = delayed_value<bool>;
