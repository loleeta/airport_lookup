/*
 * Lolita Lam
 * Vincent Schlappi 
 * CPSC 5520 Project 2
 * airports_server.cpp is the airport server of the airport distributed lookup
 * system. It employs a kd-tree structure with n nearest neighbors search to 
 * complete a valid user query placed from the places server. 
 * The algorithm to make this was borrowed from Stanford CS160L KD tree lecture
 * that can be found online. 
 * To compile: make clean
 *             make 
 * To run: ./airports_server&
 */

#include "places_airports.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

using namespace std;

#define PI 3.14159265358979323846
static const char *airportLocationsPath = "airport-locations.txt";

struct KDNode {
  Airport airp;             // Airport data loaded from file
  KDNode *left = nullptr;   // Pointer to left subtree
  KDNode *right = nullptr;  // Pointer to right subtree
};

// Kd-tree as vector of nodes
static vector<KDNode> kdNodes;

// Root node of the tree 
static KDNode *treeRoot = nullptr;

static void init();
static KDNode *buildKDSubtree(int subrangeLeft, int subrangeRight, int depth);
vector<Airport *> closest5(Target *argp);
double distance(double lat1, double lon1, double lat2, double lon2);
double deg2rad(double deg);
double rad2deg(double rad);

/* Given a target, the function searches through the kd-tree by partitioning
* the tree and searching through the subtrees */
SearchResult *closestairports_1_svc(Target *argp, struct svc_req *rqstp) {
  static SearchResult result;

  // Initialize data structures (no-op if already init)
  init();

  // Don't need to free memory,
  // because results are all pointers to static KDNodes data

  // Zero-out any previous results.
  result = { };

  result.err = 0;
  Airport *found = result.SearchResult_u.results;

  vector<Airport *> searchResult = closest5(argp);

  for (int i = 0; i < 5; ++i) {
    found[i].distanceToTarget = searchResult[i]->distanceToTarget;
    found[i].code = searchResult[i]->code;
    found[i].latitude = searchResult[i]->latitude;
    found[i].longitude = searchResult[i]->longitude;
    found[i].fullName = searchResult[i]->fullName;
    found[i].state = searchResult[i]->state;
  }

  return (&result);
}

/* Upon start up, initialize the server */
static void init() {
  static bool initialized = false;

  if (initialized) return;

  initialized = true;

  ifstream ifile(airportLocationsPath);

  if (!ifile) {
    fprintf(stderr, "Unable to open %s\n", airportLocationsPath);
    exit(1);
  }

  string line;
  string code;
  float latitude;
  float longitude;
  string city;
  string state;

  while (getline(ifile, line)) {
    // Check to see if comma exists and skip line if it does not
    auto commaIdx = line.rfind(',');
    if (commaIdx == string::npos) continue;

    state = line.substr(commaIdx + 1); // State is to right of comma
    line.erase(commaIdx);

    // Extract code, lat, long
    stringstream strm(line);
    strm >> code >> latitude >> longitude;
    code = code.substr(1, 3); // Remove code brackets
    strm >> ws; // Remove any whitespace

    getline(strm, city); // Rest of line is string

    // Temp variables to hold the read-in data. 
    KDNode node = {};
    node.airp.distanceToTarget = 0.0;
    node.airp.latitude = latitude;
    node.airp.longitude = longitude;
    node.airp.code = strdup(code.c_str());
    node.airp.fullName = strdup(city.c_str());
    node.airp.state = strdup(state.c_str());

    kdNodes.push_back(node);
  }

  treeRoot = buildKDSubtree(0, kdNodes.size(), 0);
}


/* Recursive function to build the kd tree from kd nodes */
static KDNode *buildKDSubtree(int subrangeLeft, int subrangeRight, int depth) {
  // Base case
  int numElems = subrangeRight - subrangeLeft;
  if (numElems <= 0)
    return nullptr;

  if (depth % 2 == 0) {
    // Sort subrange by latitude when depth is event
    std::sort(kdNodes.begin() + subrangeLeft, kdNodes.begin() + subrangeRight,
      [](const KDNode &n1, const KDNode &n2) {
        return n1.airp.latitude < n2.airp.latitude;
      });
  } 
  else {
    // Sort subrange by longitude when depth is odd
    std::sort(kdNodes.begin() + subrangeLeft, kdNodes.begin() + subrangeRight,
      [](const KDNode &n1, const KDNode &n2) {
        return n1.airp.longitude < n2.airp.longitude;
      });
  }

  int midIdx = subrangeLeft + numElems / 2;

  // Recursively build tree
  KDNode *subRoot = &kdNodes[midIdx];
  subRoot->left = buildKDSubtree(subrangeLeft, midIdx, depth + 1);
  subRoot->right = buildKDSubtree(midIdx + 1, subrangeRight, depth + 1);

  return subRoot;
}

// Algorithm taken from CS106L KD Tree lecture
void collectClosest5(Target *t, KDNode *subTree, int depth,
                     vector<Airport *> *results) {
  // Base case, tree is empty, do nothing
  if (subTree == nullptr)
    return;

  // Calculate the distance to the target
  subTree->airp.distanceToTarget = (float) (distance(t->latitude, 
        t->longitude, subTree->airp.latitude, subTree->airp.longitude));

  results->push_back(&subTree->airp);
  std::sort(results->begin(), results->end(), 
    [](const Airport *a1, const Airport *a2) {
      return a1->distanceToTarget < a2->distanceToTarget;
    });

  // If vector has more than five, remove the furthest airport
  if (results->size() > 5)
    results->pop_back();

  // Find out which node is closer to target
  bool isLeftTreeCloser;
  if (depth % 2 == 0) 
    isLeftTreeCloser = t->latitude < subTree->airp.latitude;
  else 
    isLeftTreeCloser = t->longitude < subTree->airp.longitude;

  // Recursively search the half of the tree that contains the target
  if (isLeftTreeCloser) 
    collectClosest5(t, subTree->left, depth + 1, results);
  else 
    collectClosest5(t, subTree->right, depth + 1, results);

  // Radius of the max distance of the candidates
  float maxRadius = results->at(4)->distanceToTarget;

  // Point on the splitting line that is closest to target
  double ptLat = subTree->airp.latitude;
  double ptLon = subTree->airp.longitude;

  if (depth % 2 == 0)
    ptLon = t->longitude;
  else
    ptLat = t->latitude;

  double ptClosestDist = distance(t->latitude, t->longitude, ptLat, ptLon);

  // Might need to look in other tree when five points haven't been found
  if (results->size() < 5 || (ptClosestDist - 0.0001 <= maxRadius)) {
    if (isLeftTreeCloser)
      collectClosest5(t, subTree->right, depth + 1, results);
    else
      collectClosest5(t, subTree->left, depth + 1, results);
  }
}

/* Search iteratively for closest airports. Insert into vector and when size
 * exceeds 5, push the furtherest one out */
vector<Airport *> closest5(Target *argp) {
  vector<Airport *> results;
  collectClosest5(argp, treeRoot, 0, &results);
  return results;
}

/* Math function to calculate distance */
double distance(double lat1, double lon1, double lat2, double lon2) {
  double theta, dist;
  theta = lon1 - lon2;
  dist = sin(deg2rad(lat1)) * sin(deg2rad(lat2)) + cos(deg2rad(lat1)) *
      cos(deg2rad(lat2)) * cos(deg2rad(theta));
  dist = acos(dist);
  dist = rad2deg(dist);
  dist = dist * 60 * 1.1515;
  return (dist);
}

double deg2rad(double deg) {
  return (deg * PI / 180);
}

double rad2deg(double rad) {
  return (rad * 180 / PI);
}
