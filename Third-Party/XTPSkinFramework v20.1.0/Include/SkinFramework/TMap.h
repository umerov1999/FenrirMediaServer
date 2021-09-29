#pragma once
#include <iostream>
#include <list>
namespace ToolkitProMap
{
	template<typename KEY, typename VALUE>
	class Pair
	{
	public:
		Pair(const KEY& key, const VALUE& value)
		{
			this->key = key;
			this->value = value;
		}
		const KEY &get_key() const
		{
			return key;
		}
		const VALUE &get_value() const
		{
			return value;
		}
		VALUE &get_value()
		{
			return value;
		}
	private:
		KEY key;
		VALUE value;
	};

	template<typename KEY, typename VALUE>
	class Map
	{
	public:
		Map() {}
		Map(const Map<KEY, VALUE>&v)
		{
			List = v.List;
		}
		VALUE operator[](const KEY& v) const
		{
			for (auto& i : List)
			{
				if (i.get_key() == v)
					return i.get_value();
			}
			throw std::runtime_error("ERROR: Map Key not found!");
		}
		VALUE& operator[](const KEY& v)
		{
			for (auto &i : List)
			{
				if (i.get_key() == v)
					return i.get_value();
			}
			List.push_back(Pair<KEY, VALUE>(v, VALUE()));
			return List.back().get_value();
		}
		bool exist(const KEY& v) const
		{
			for (auto &i : List)
			{
				if (i.get_key() == v)
					return true;
			}
			return false;
		}
		void erase(const KEY& v)
		{
			for (auto i = List.begin(); i != List.end(); i++)
			{
				if ((*i).get_key() == v)
				{
					List.erase(i);
					return;
				}
			}
		}
		VALUE at(const KEY& v) const
		{
			for (auto &i : List)
			{
				if (i.get_key() == v)
					return i.get_value();
			}
			throw std::runtime_error("ERROR: Map Key not found!");
		}
		VALUE& at(const KEY& v)
		{
			for (auto &i : List)
			{
				if (i.get_key() == v)
					return i.get_value();
			}
			throw std::runtime_error("ERROR: Map Key not found!");
		}
		void operator=(const Map<KEY, VALUE>&v)
		{
			List = v.List;
		}
		size_t size() const
		{
			return List.size();
		}
		auto begin() const
		{
			return List.begin();
		}
		auto end() const
		{
			return List.end();
		}
		auto begin()
		{
			return List.begin();
		}
		auto end()
		{
			return List.end();
		}
		void push_front(const KEY& key, const VALUE& value)
		{
			List.push_front(Pair<KEY, VALUE>(key, value));
		}
		std::list<Pair<KEY, VALUE>>& operator()()
		{
			return List;
		}
		template<typename SORTFNC>
		void sort(SORTFNC fnc)
		{
			List.sort(fnc);
		}
		void clear()
		{
			List.clear();
		}

	private:
		std::list<Pair<KEY, VALUE>> List;
	};

	struct string_compare_no_case
	{
	public:
		string_compare_no_case()
		{
			Str = "";
		}
		string_compare_no_case(const string_compare_no_case&v)
		{
			Str = v.Str;
		}
		string_compare_no_case(const std::string&v)
		{
			Str = v;
		}
		string_compare_no_case(const char* v)
		{
			Str = v;
		}
		std::string operator()() const
		{
			return Str;
		}
		bool operator==(const string_compare_no_case&v) const
		{
			if (Str != v.Str)
			{
				if (Str.length() != v.Str.length())
					return false;
				std::string Temp1;
				for (auto&i : Str)
					Temp1.push_back(ToLower(i));
				std::string Temp2;
				for (auto&i : v.Str)
					Temp2.push_back(ToLower(i));
				return Temp1 == Temp2;
			}
			return true;
		}
		bool operator!=(const string_compare_no_case&v) const
		{
			return !operator==(v);
		}
	private:
		static char ToLower(char c)
		{
			if (c >= 'A' && c <= 'Z')
				return c + ('z' > 'Z' ? 'z' - 'Z' : 'Z' - 'z');
			return c;
		}
		std::string Str;
	};

	struct wstring_compare_no_case
	{
	public:
		wstring_compare_no_case()
		{
			Str = L"";
		}
		wstring_compare_no_case(const wstring_compare_no_case&v)
		{
			Str = v.Str;
		}
		wstring_compare_no_case(const std::wstring&v)
		{
			Str = v;
		}
		wstring_compare_no_case(const wchar_t* v)
		{
			Str = v;
		}
		std::wstring operator()() const
		{
			return Str;
		}
		bool operator==(const wstring_compare_no_case&v) const
		{
			if (Str != v.Str)
			{
				if (Str.length() != v.Str.length())
					return false;
				std::wstring Temp1;
				for (auto&i : Str)
					Temp1.push_back(ToWLower(i));
				std::wstring Temp2;
				for (auto&i : v.Str)
					Temp2.push_back(ToWLower(i));
				return Temp1 == Temp2;
			}
			return true;
		}
		bool operator!=(const wstring_compare_no_case&v) const
		{
			return !operator==(v);
		}
	private:
		static wchar_t ToWLower(wchar_t c)
		{
			if (c >= L'А' && c <= L'Я')
				return c + (L'я' > L'Я' ? L'я' - L'Я' : L'Я' - L'я');
			if (c >= L'A' && c <= L'Z')
				return c + (L'z' > L'Z' ? L'z' - L'Z' : L'Z' - L'z');
			return c;
		}
		std::wstring Str;
	};
}