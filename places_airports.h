/*
 * Combined airports and places header files.
 */

#ifndef _PLACES_H_RPCGEN
#define _PLACES_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

/* airports.h section */

struct Target {
	double latitude;
	double longitude;
};
typedef struct Target Target;

struct Airport {
	char *code;
	char *fullName;
	char *state;
	double latitude;
	double longitude;
	float distanceToTarget;
};
typedef struct Airport Airport;

typedef Airport AirResults[5];

struct SearchResult {
	int err;
	union {
		AirResults results;
		char *err;
	} SearchResult_u;
};
typedef struct SearchResult SearchResult;

#define AIRPORT_PROG 0x37778899
#define AIRPORT_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define closestAirports 1
extern  SearchResult * closestairports_1(Target *, CLIENT *);
extern  SearchResult * closestairports_1_svc(Target *, struct svc_req *);
extern int airport_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define closestAirports 1
extern  SearchResult * closestairports_1();
extern  SearchResult * closestairports_1_svc();
extern int airport_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_Target (XDR *, Target*);
extern  bool_t xdr_Airport (XDR *, Airport*);
extern  bool_t xdr_AirResults (XDR *, AirResults);
extern  bool_t xdr_SearchResult (XDR *, SearchResult*);

#else /* K&R C */
extern bool_t xdr_Target ();
extern bool_t xdr_Airport ();
extern bool_t xdr_AirResults ();
extern bool_t xdr_SearchResult ();

#endif /* K&R C */

/* END OF airports.h section */


struct ClientRequest {
	char *city;
	char *state;
};
typedef struct ClientRequest ClientRequest;

struct PlacesAnswer {
	char *cityInfo;
	AirResults airports;
};
typedef struct PlacesAnswer PlacesAnswer;

struct PlacesResult {
	int err;
	union {
		PlacesAnswer result;
		char * errStr;
	} PlacesResult_u;
};
typedef struct PlacesResult PlacesResult;

#define PLACES_PROG 0x37778898
#define PLACES_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define placesQuery 1
extern  PlacesResult * placesquery_1(ClientRequest *, CLIENT *);
extern  PlacesResult * placesquery_1_svc(ClientRequest *, struct svc_req *);
extern int places_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define placesQuery 1
extern  PlacesResult * placesquery_1();
extern  PlacesResult * placesquery_1_svc();
extern int places_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_ClientRequest (XDR *, ClientRequest*);
extern  bool_t xdr_PlacesAnswer (XDR *, PlacesAnswer*);
extern  bool_t xdr_PlacesResult (XDR *, PlacesResult*);

#else /* K&R C */
extern bool_t xdr_ClientRequest ();
extern bool_t xdr_PlacesAnswer ();
extern bool_t xdr_PlacesResult ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_PLACES_H_RPCGEN */
