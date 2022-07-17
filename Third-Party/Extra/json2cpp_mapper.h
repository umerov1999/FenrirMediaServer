#pragma once
#include "json.hpp"

void do_json2cpp_mapper_log(const std::string &what, bool is_error);
#define JSON2CPPLOG(name,except,is_error) do_json2cpp_mapper_log(std::string("[JSON MAPPER] {") + std::string("json2cpp_mapper.h") + ":" + std::to_string(__LINE__) + "} - name=" + std::string(name) + ", except " + std::string(except), is_error);
#define js2cpp_check_default(x,c) if(!json2cpp_mapper::inited(x)) c;

namespace json2cpp_mapper
{
	template<typename T>
	static bool inited(const T& value) {
		return value != T();
	}

	template<typename T>
	static void toDefault(T& value, T default_value) {
		if (value == T()) {
			value = default_value;
		}
	}

	template<typename T>
	static nlohmann::json serialize_list_to_json(std::list<T> list) {
		nlohmann::json ret = nlohmann::json(nlohmann::json::value_t::array);
		for (auto& i : list) {
			ret.push_back(i.toJson());
		}
		return ret;
	}

	template<typename T>
	static nlohmann::json primitive_list_to_json(std::list<T> list) {
		nlohmann::json ret = nlohmann::json(nlohmann::json::value_t::array);
		for (auto& i : list) {
			ret.push_back(i);
		}
		return ret;
	}

	template <typename T>
	static T optValue(const nlohmann::json& js, std::string name, T pdefault = T())
	{
		if (js.find(name) == js.end() || js.at(name).is_array() || js.at(name).is_null())
			return pdefault;
		T ret = pdefault;
		try {
			ret = js.at(name).get<T>();
		}
		catch (nlohmann::json::exception e) {
			JSON2CPPLOG(name, e.what(), true)
		}
		return ret;
	}

	static int optInt(const nlohmann::json& js, std::string name, int pdefault = int())
	{
		if (js.find(name) == js.end() || js.at(name).is_array() || js.at(name).is_null())
			return pdefault;
		int ret = pdefault;
		try {
			if (js.at(name).is_string()) {
				JSON2CPPLOG(name, "try string convert to int", false)
				ret = std::stoi(js.at(name).get<std::string>());
			}
			else {
				ret = js.at(name).get<int>();
			}
		}
		catch (nlohmann::json::exception e) {
			JSON2CPPLOG(name, e.what(), true)
		}
		catch (std::invalid_argument e) {
			JSON2CPPLOG(name, e.what(), true)
		}
		catch (std::out_of_range e) {
			JSON2CPPLOG(name, e.what(), true)
		}
		return ret;
	}

	template<typename T>
	static std::list<T> optPrimitiveList(const nlohmann::json& js, std::string name) {
		if (js.find(name) == js.end() || !js.at(name).is_array())
			return std::list<T>();
		try {
			nlohmann::json root = js.at(name);
			std::list<T>Ret;

			for (auto& i : root) {
				Ret.push_back(i.get<T>());
			}
			return Ret;
		}
		catch (nlohmann::json::exception e) {
			JSON2CPPLOG(name, e.what(), true)
			return std::list<T>();
		}
	}

	template<typename T>
	static std::list<T> optSerList(const nlohmann::json& js, std::string name) {
		if (js.find(name) == js.end() || !js.at(name).is_array())
			return std::list<T>();
		try {
			nlohmann::json root = js.at(name);
			std::list<T>Ret;

			for (auto& i : root) {
				Ret.push_back(T().fromJson(i));
			}
			return Ret;
		}
		catch (nlohmann::json::exception e) {
			JSON2CPPLOG(name, e.what(), true)
			return std::list<T>();
		}
	}
}