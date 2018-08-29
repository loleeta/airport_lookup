#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <queue>
#include "places_airports.h"
using namespace std;

/* Struct holding city information to be inserted into trie node */
struct CityRecord {
	double latitude;
	double longitude;
	string cityName;
	string state;
	CityRecord(double lat, double lon, string name, string sta) {
		latitude = lat;
		longitude = lon;
		cityName = name;
		state = sta;
	}
};

/* Node holding vector of cities, and link to next node */
struct TrieNode {
	vector<CityRecord*> cities;
	TrieNode *next[256] = { 0 };
};

/* Trie structure and member functions */
struct Trie {
	TrieNode *root = nullptr;

	/* Public function to insert a CityRecord */
	void put(CityRecord * c) {
		string cityLower = c->cityName;
		lowerCase(cityLower);
		root = put(root, cityLower, c, 0);
	}


	/* Public function to return CityRecord if found
	 * In: string with city and state
	 * Out: a CityRecord or nullptr 
	Target * get(string city) {
		string state = city.substr(city.length() - 2, 2);
		city.erase(city.length() - 3, 3); //remove state from string
		lowerCase(city);
		TrieNode * n = getNode(root, city, 0);
		if (n == nullptr)
			return nullptr;
		if (!n->cities.empty()) {
			for (unsigned i = 0; i < n->cities.size(); i++) {
				if (n->cities[i]->state == state)
					return getCoord(n->cities[i]);
			}
		}
		return nullptr;
	} */

	/* Public function that returns a vector of CityRecords
	 * with an ambiguous search
	 * e.g., "San" returns two "San Diego" records with different states
	 */
	vector<CityRecord*> findFirstCity(string prefix) {
		lowerCase(prefix);
		TrieNode * n = getNode(root, prefix, 0);
		TrieNode * city = findFirstCompletion(n);
		if (city == nullptr)
			return vector<CityRecord*>();
		return city->cities;
	}

	private:
	/* Private function that recursively finds a node, given a city name
	 */
	TrieNode * getNode(TrieNode * t, const string& city, unsigned depth) {
		if (t == nullptr) return nullptr;
		if (depth == city.length()) return t;
		unsigned char c = city[depth];
		return getNode(t->next[c], city, depth + 1);
	}

	/* Private function that inserts a CityRecord into trie
	 */
	TrieNode* put(TrieNode* subtree, const string& cityName, CityRecord* insertVal, unsigned depth) {
		if (subtree == nullptr)
			subtree = new TrieNode();
		if (depth == cityName.length()) {
			subtree->cities.push_back(insertVal);
			return subtree;
		}
		unsigned char c = cityName[depth];
		subtree->next[c] = put(subtree->next[c], cityName, insertVal, depth + 1);
		return subtree;
	}

	/* Makes a string into lower case */
	void lowerCase(string& s) {
		transform(s.begin(), s.end(), s.begin(), ::tolower);
	}

	/* Returns a node for an ambiguous search. Given a node, traverse as far down as possible 
	 * and return a node with a city in it
	 */
	TrieNode * findFirstCompletion(TrieNode * n) {
		// base case
		if (n == nullptr) return nullptr;

		// When the current node has a result, return that result and end recursion

		// When it does not have anything saved in it,
		// Search all of the next pointers until finding the first non-null entry
		// Return the result of the first completion of that entry

		if (!n->cities.empty()) {
			return n;
		}
		for (int i = 0; i < 256; i++) {
			if (n->next[i] != nullptr)
				return findFirstCompletion(n->next[i]);
		}
		return nullptr;
	}

	/* Returns a Target with latitude, longituge if city has been found 
	Target * getCoord(CityRecord * city) {
		Target * coord;
		if (city != nullptr)
			coord = Target{city->latitude, city->longitude};
		else
			return nullptr;
		return coord;
	}*/
};