# airport_lookup
Three-tiered client-server system using SunRPC to look up airports, given a valid city name. 

### `client`
- Uses RPC to contact `places_server` with a location name.
- Prints the airport information returned from server.

### `places_server`
- Constructs a trie given US Census-compiled text file of incorporated and designated places.
- Uses RPC to contact `airports_server` with a latitude and longitude.
- Returns the five closest airports to the client.

### `airports_server`
- Constructs a KD tree given text file of airport locations.
- Queries the tree for k-nearest neighbors (k = 5).
- Uses RPC to return data to client. 

<img src="https://user-images.githubusercontent.com/3770476/44761383-cc896300-aaf7-11e8-9c6e-7abc3716fe27.png" width="50%">

## To Compile
`make clean`

`make` 

 `make debug` (if needed)


## To Run
**To start places server**

`./places_server localhost &`

**To start airport server**

`./airports_server&`

**To run client**

`./client localhost [city] [state]`


## Files
<dl>
  <dt>airport-locations.txt</dt>
  <dd>Airports text file for the airports server</dd>
  
  <dt>airports.x</dt>
  <dd>IDL file for creating airports client/server</dd>
  
  <dt>airports_clnt.c</dt>
  <dd>Auto-generated client stub</dd>
  
  <dt>airports_server.cpp</dt>
	<dd>The cpp file for airports server</dd>
  
  <dt>airports_svc.c</dt>
	<dd>Auto-generated server stub</dd>

  <dt>airports_xdr.c</dt>
	<dd>Auto-generated file for encoding data to RPC format</dd>
  
  <dt>Makefile</dt>
	<dd>File for dependencies and build rules</dd>

  <dt>places.x</dt>
	<dd>The IDL file for creating places client/server</dd>

  <dt>places_airport.h</dt>
	<dd>The header file containing object details</dd>

  <dt>places_client.cpp</dt>
	<dd>The cpp file for the client </dd>
  
  <dt>places_clnt.c</dt>
	<dd>Auto-generated client stub</dd>
  
  <dt>places_server.cpp</dt>
	<dd>The cpp file for the places client/server</dd>
  
  <dt>places_svc.c</dt>
	<dd>Auto-generated server stub</dd>
  
  <dt>places_xdr.c</dt>
	<dd>Auto-generated file for encoding data to RPC format</dd>
  
  <dt>places2k.txt</dt>
	<dd>The places text file for the places server</dd>
  
  <dt>Trie.h</dt>
	<dd>The header file containing the Trie structure</dd>
</dl>
