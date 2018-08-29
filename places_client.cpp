/*
 * Lolita Lam
 * Vincent Schlappi
 * CPSC 5520 Project 2
 * places_client.cpp is the client program for the airport lookup distributed
 * system. It receives an appropriate look-up from the user and contacts the
 * places server, and receives some response back, either the closest airports
 * to the city, or an error message. 
 * To compile: make clean
 * To run: ./client localhost [city] [state]
 *         For example: ./client localhost Seattle WA
 */

#include "places_airports.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>

using namespace std;

void displayResult(PlacesResult *res) {
	// Response from server is displayed
	if (res->err) {
		cout << "Error: " << res->PlacesResult_u.errStr << endl;
	} 
	else {
		PlacesAnswer *ans = &res->PlacesResult_u.result;
		cout << ans->cityInfo << endl;
		for (int i = 0; i < 5; i++) {
			cout << i + 1 << ": ";
			cout << "code=" << ans->airports[i].code;
			cout << ", name=" << ans->airports[i].fullName;
			cout << ", state=" << ans->airports[i].state;
			cout << ", dist=" << fixed  << setprecision(3)
					 << ans->airports[i].distanceToTarget << endl;
		}
	}
}

void places_prog_1(char* host, char* city, char* state)
{
	CLIENT *clnt;
	PlacesResult  *result_1;
	ClientRequest  placesquery_1_arg;

	placesquery_1_arg.city = city;
	placesquery_1_arg.state = state;

	#ifndef	DEBUG
	clnt = clnt_create(host, PLACES_PROG, PLACES_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror(host);
		exit (1);
	}
	#endif	/* DEBUG */

	result_1 = placesquery_1(&placesquery_1_arg, clnt);

	if (result_1 == (PlacesResult *) NULL) 
		clnt_perror (clnt, "call failed");
	else 
		displayResult(result_1);

	#ifndef	DEBUG
	clnt_freeres(clnt, (xdrproc_t )xdr_PlacesResult, (char*) result_1);
	clnt_destroy (clnt);
	#endif	 /* DEBUG */
}


//Assumption: user will enter correctly formatted state
// e.g. state will be capitalized, and multi-worded cities are 
// encapsulated in double-quote marks
int main (int argc, char *argv[])
{
	if(argc < 4) {
		printf("usage: %s server_host city state\n", argv[0]);
		exit(1);
	}

	char* host = argv[1];
	char* city = argv[2];
	char* state = argv[3];

	places_prog_1(host, city, state);
  	exit (0);
}
