/* airports.x
 * Lolita Lam, Vincent Schlappi
 * CPSC 5520 Project 2 */

/* Request to the airport program */
struct Target {
	double latitude;
	double longitude;
};

/* Airport result */
struct Airport {
	string code<4>;
	string fullName<64>;
	string state<3>;
	double latitude;
	double longitude;
	float distanceToTarget;
};

/* Results of the search */
typedef Airport AirResults[5];

/* result of a airport search */
union SearchResult switch (int err) {
	case 0:
		AirResults results;
	default:
        /* Error message details */
		string err<256>;
};

/* IDL program for airport server */
program AIRPORT_PROG {
	version AIRPORT_VERS {
		SearchResult closestAirports(Target) = 1;
	} = 1; /* version 1 */
} = 0x37778090;
