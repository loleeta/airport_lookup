/*
* Lolita Lam
* Vincent Schlappi
* CPSC 5520 Project 2
* places_server.cpp is the client/server program for the airport lookup 
* distributed system. It accepts incoming connections and requests from
* a client to look up an airport, sets up a connection and requests 
* information on the 5 closest airports for the client's location from the
* airport server, and returns this information back to the client. This
* program reads in the places2k.txt file and builds a Trie structure for
* fast lookup. 
* 
* To compile: make clean
*             make 
* To run: ./places_server localhost &
*/

//#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include "places_airports.h"
#include "Trie.h"
using namespace std;

extern "C" char* airportsHost;
static Trie trie;

CityRecord * parseLineForCity(string line);
string trim(string &str);
void copyAirportData(Airport *dst, Airport *src);

/* Creates connection and queries airport server
 * Returns results from airport server to client */
void airport_prog_1(Target *target, PlacesResult *placesResult) {
	CLIENT *clnt;
	SearchResult *foundAirports;

	#ifndef	DEBUG
	clnt = clnt_create(airportsHost, AIRPORT_PROG, AIRPORT_VERS, "udp");

	if (clnt == NULL) {
		clnt_pcreateerror(airportsHost);
		placesResult->err = 1;
		placesResult->PlacesResult_u.errStr =
				strdup("Places client/server: can't connect to airports\n");
	}
	#endif	/* DEBUG */

	// Send target information to airports server and receive response
	foundAirports = closestairports_1(target, clnt);

	// Response is null, return
	if (foundAirports == (SearchResult *)NULL) {
		clnt_perror(clnt, "places client/server - call failed");
		placesResult->err = 1;
    	placesResult->PlacesResult_u.errStr = 
				strdup("places client/server - call failed");
    	return;
	}

	// Result received, check for error
	if (foundAirports->err) {
    // Received an error from airport, return early
		placesResult->err = foundAirports->err;
		placesResult->PlacesResult_u.errStr = strdup(foundAirports->SearchResult_u.err);
		clnt_freeres(clnt, (xdrproc_t)xdr_SearchResult, (char*)foundAirports);
		clnt_destroy(clnt);
		return;
	}

	// Copy the results into the placesResult before freeing memory
	copyAirportData(placesResult->PlacesResult_u.result.airports,
									foundAirports->SearchResult_u.results);

	#ifndef	DEBUG
	clnt_freeres(clnt, (xdrproc_t)xdr_SearchResult, (char*)foundAirports);
	clnt_destroy(clnt);
 	#endif	 /* DEBUG */
}


/* Upon start up, initialize data structures */
static void init() {
  static bool isInit = false;
	if (isInit) return;
	isInit = true;

	string filePath = "places2k.txt";
	ifstream inFile(filePath);
	if (inFile.bad()) { exit(0); }

	string line;
	while(getline(inFile, line)) {
		CityRecord *city = parseLineForCity(line);
		trie.put(city);
	}
}

/* Receives argument from client and looks up location in trie
 * Creates a Target to query airport 
 * Returns results to client */
PlacesResult * placesquery_1_svc(ClientRequest *argp, struct svc_req *rqstp) {
	static PlacesResult placesResult;
	string cityInfo = "";
	bool foundCity = false;

	// Initialize trie by loading file
	init();

  	// Free prev memory
	xdr_free((xdrproc_t) xdr_PlacesResult, (char*)&placesResult);

	// sets all fields to 0
  	placesResult = {};

	string reqCity = argp->city;
	string reqState = argp->state;
	Target target;

	// Search for client request in trie
	vector<CityRecord*> citiesFound = trie.findFirstCity(reqCity);
	if (citiesFound.empty()) {
		placesResult.err = 1;
		placesResult.PlacesResult_u.errStr = strdup("No cities found in trie");
	}

	// If multiple cities of the entered name are found, search through 
	// returned vector. Match with the state. 
	if (citiesFound.size() > 0) {
		for (unsigned i = 0; i < citiesFound.size(); i++) {
			if (citiesFound[i]->state == reqState) {
				target.latitude = citiesFound[i]->latitude;
				target.longitude = citiesFound[i]->longitude; 
				foundCity = true;
				cityInfo = citiesFound[i]->cityName + ", " + 
					citiesFound[i]->state + ": " + 
					to_string(citiesFound[i]->latitude) + 
					", " + to_string(citiesFound[i]->longitude);
				break; 
			}
		}
		if (!foundCity) {
			placesResult.err = 1;
			placesResult.PlacesResult_u.errStr =
				strdup("Ambiguous query. Please refine.");
		}
	}

	if (citiesFound.size() == 1) {
		if (citiesFound[0]->state == reqState) {
			target.latitude = citiesFound[0]->latitude;
			target.longitude = citiesFound[0]->longitude;
			cityInfo = citiesFound[0]->cityName + ", " + 
					citiesFound[0]->state + ": " + 
					to_string(citiesFound[0]->latitude) + 
					", " + to_string(citiesFound[0]->longitude);
		}
		else {
			placesResult.err = 1;
			placesResult.PlacesResult_u.errStr =
				strdup("Ambiguous query. Please refine.");
		}
	}

	// Do not send request to airports server if could not find city info
	if (placesResult.err) {
		return &placesResult;
	}

	// Send target request to airports server
	// Copy the airport results into the places placesResult
	airport_prog_1(&target, &placesResult);

	if (placesResult.err == 0)
		placesResult.PlacesResult_u.result.cityInfo = strdup(cityInfo.c_str());

	return(&placesResult);
}

/* Parses string for city information and creates a CityRecord */
CityRecord * parseLineForCity(string line) {
	string city, state;
	double latitude, longitude;

	state = line.substr(0, 2);
	city = line.substr(9, 64);
	city = trim(city);
	latitude = stod(line.substr(143, 10));
	longitude = stod(line.substr(153, 11));
	//cout << fixed << setprecision(20) << latitude << endl;

	CityRecord * newCity = new CityRecord(latitude, longitude, city, state);
	return newCity;
}

/* Trims whitespace off string */
string trim(string &str) {
	size_t first = str.find_first_not_of(' ');
	if (string::npos == first)
		return str;

	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

/* Copy results from one result to the other before returning to client */
void copyAirportData(Airport *dst, Airport *src) {
	for (int i = 0; i < 5; ++i) {
		dst[i].code = strdup(src[i].code);
		dst[i].fullName = strdup(src[i].fullName);
		dst[i].state = strdup(src[i].state);
		dst[i].latitude = src[i].latitude;
		dst[i].longitude = src[i].longitude;
		dst[i].distanceToTarget = src[i].distanceToTarget;
	}
}
