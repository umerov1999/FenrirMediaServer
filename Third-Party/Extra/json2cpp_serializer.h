#pragma once
#include "json.hpp"

class json2cpp_serializer {
public:
	virtual nlohmann::json toJson() const = 0;
};