
# Lolita

# Use a c++ linker when making executable to account for c++ files
LINK.o = $(LINK.cc)

# Compile c++ files with all warnings as errors
CXXFLAGS = -g -Wall -pedantic -std=c++11

# All object files used by all programs
ALL_OBJS =\
 airports_clnt.o \
 airports_server.o \
 airports_svc.o \
 places_airports_xdr.o \
 places_client.o \
 places_clnt.o \
 places_server.o \
 places_svc.o

# All header files that should force a recompile of all objects
ALL_HEADERS = places_airports.h

# Libraries used when linking executables
LDLIBS += -lnsl

# Targets

all : client places_server airports_server
all : CXXFLAGS += -O3
all : CFLAGS += -O3

debug : client places_server airports_server

client : places_client.o places_clnt.o places_airports_xdr.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

places_server : places_server.o places_svc.o airports_clnt.o places_airports_xdr.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

airports_server : airports_server.o airports_svc.o places_airports_xdr.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(ALL_OBJS) : $(ALL_HEADERS)

clean:
	$(RM) $(ALL_OBJS) client places_server airports_server
