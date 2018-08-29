/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "places_airports.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

PlacesResult *
placesquery_1(ClientRequest *argp, CLIENT *clnt)
{
	static PlacesResult clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, placesQuery,
		(xdrproc_t) xdr_ClientRequest, (caddr_t) argp,
		(xdrproc_t) xdr_PlacesResult, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
