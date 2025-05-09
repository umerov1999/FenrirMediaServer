#pragma once

#include <iostream>
#include <vector>
#include <list>

namespace SoundMap {
	template<typename KEY, typename VALUE>
	class Pair {
	public:
		Pair(const KEY& key, const VALUE& value) {
			this->key = key;
			this->value = value;
		}
		const KEY& get_key() const {
			return key;
		}
		const VALUE& get_value() const {
			return value;
		}
		VALUE& get_value() {
			return value;
		}
	private:
		KEY key;
		VALUE value;
	};

	template<typename KEY, typename VALUE>
	class Map {
	public:
		Map() {}
		Map(const Map<KEY, VALUE>& v) {
			List = v.List;
		}
		VALUE operator[](const KEY& v) const {
			for (auto& i : List)
			{
				if (i.get_key() == v)
					return i.get_value();
			}
			throw std::runtime_error("ERROR: Map Key not found!");
		}
		VALUE& operator[](const KEY& v) {
			for (auto& i : List) {
				if (i.get_key() == v)
					return i.get_value();
			}
			List.push_back(Pair<KEY, VALUE>(v, VALUE()));
			return List.back().get_value();
		}
		bool exist(const KEY& v) const {
			for (auto& i : List) {
				if (i.get_key() == v)
					return true;
			}
			return false;
		}
		void erase(const KEY& v) {
			for (auto i = List.begin(); i != List.end(); i++) {
				if ((*i).get_key() == v) {
					List.erase(i);
					return;
				}
			}
		}
		VALUE at(const KEY& v) const {
			for (auto& i : List) {
				if (i.get_key() == v)
					return i.get_value();
			}
			throw std::runtime_error("ERROR: Map Key not found!");
		}
		VALUE& at(const KEY& v) {
			for (auto& i : List) {
				if (i.get_key() == v)
					return i.get_value();
			}
			throw std::runtime_error("ERROR: Map Key not found!");
		}
		void operator=(const Map<KEY, VALUE>& v) {
			List = v.List;
		}
		size_t size() const {
			return List.size();
		}
		auto begin() const {
			return List.begin();
		}
		auto end() const {
			return List.end();
		}
		auto begin() {
			return List.begin();
		}
		auto end() {
			return List.end();
		}
		void push_front(const KEY& key, const VALUE& value) {
			List.push_front(Pair<KEY, VALUE>(key, value));
		}
		std::list<Pair<KEY, VALUE>>& operator()() {
			return List;
		}
		template<typename SORTFNC>
		void sort(SORTFNC fnc) {
			List.sort(fnc);
		}
		void clear() {
			List.clear();
		}

	private:
		std::list<Pair<KEY, VALUE>> List;
	};
}

class USound {
public:
	~USound();
	void RegisterResourceMP3Sounds(const std::string& data);
	void RegisterResourceOGGSounds(size_t count, HMODULE hModule, LPCWSTR ResType, ...);
	void PlayResourceSound(int Res, bool Loop, bool NoStop) const;
	void PlayMemorySound(bool Loop, bool NoStop) const;
	void Stop();
private:
	SoundMap::Map<int, void*>ResourceSounds;
	std::vector<uint8_t> MemoryTrack;
};