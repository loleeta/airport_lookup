/* places.x
 * Lolita Lam, Vincent Schlappi
 * CPSC 5520 Project 2
 * To compile: rpcgen -a -C airport.x */

/* AIRPORT DEFINITIONS */
/******************************************/

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


/* END AIRPORTS DEFS */
/******************************************/

/* a struct to hold the client's request */
struct ClientRequest {
    string city<64>;
    string state<3>;
};

struct PlacesAnswer {
    string cityInfo<128>;
    AirResults airports;
};

/* result of call from places */
union PlacesResult switch(int err) {
    case 0:
        PlacesAnswer result;
    default:
        string errStr<256>;     /* error occured, returns nothing */
};


/* IDL program for places */
/* returns places_result: code, name, state, distance
 * uses: lat_long
 */
program PLACES_PROG {
    version PLACES_VERS {
        PlacesResult placesQuery(ClientRequest) = 1;
    } = 1; 
} = 0x38775898;