// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <utility>
#include <limits>
#include <unordered_map>
#include <memory>
#include <map>
#include <deque>
#include <climits>
#include <set>

//------------------------- PROVIDED BY THE COURSE ----------------------------

// Type for beacon IDs
using BeaconID = std::string;

// Return value for cases where required beacon was not found
BeaconID const NO_ID = "----------";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
std::string const NO_NAME = "-- unknown --";

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for color (RGB)
struct Color
{
    int r = NO_VALUE;
    int g = NO_VALUE;
    int b = NO_VALUE;
};

// Equality and non-equality comparisons for Colors
inline bool operator==(Color c1, Color c2) { return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b; }
inline bool operator!=(Color c1, Color c2) { return !(c1==c2); }

// Return value for cases where color was not found
Color const NO_COLOR = {NO_VALUE, NO_VALUE, NO_VALUE};

// Type for light transmission cost (used only in the second assignment)
using Cost = int;

// Return value for cases where cost is unknown
Cost const NO_COST = NO_VALUE;

// ----------------------------------------------------------------------------


enum State { WHITE, GRAY, BLACK };

struct Beacon
{
    BeaconID id = NO_ID;
    std::string name = NO_NAME;
    Coord coords = NO_COORD;
    Color color = NO_COLOR;
    int brightness = NO_VALUE;
    BeaconID target = NO_ID;
    std::vector<BeaconID> sources = {};
};

struct Xpoint
{
    Coord coords = NO_COORD;
    std::unordered_map<Coord, std::pair<std::shared_ptr<Xpoint>, Cost>, CoordHash> fibres = {};
    State state = WHITE;
    std::shared_ptr<Xpoint> pi = nullptr;
    Cost route_cost = 0;
    Cost d = INT_MAX;
};

struct Prio_que_op
{
  // Operator for min-priority queue. Used in Dijkstra's-algorithm.
    inline bool operator()(const std::shared_ptr<Xpoint> lhs, const std::shared_ptr<Xpoint> rhs)
    {
        return lhs->d > rhs->d;
    }
};

// This is the class you are supposed to implement
class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1)
    // Short rationale for estimate: map.size() is constant
    int beacon_count();

    // Estimate of performance: O(n)
    // Short rationale for estimate: map.clear() is linear in the size
    void clear_beacons();

    // Estimate of performance: O(n)
    // Short rationale for estimate: looping through a map
    std::vector<BeaconID> all_beacons();

    // Estimate of performance: O(log n)
    // Short rationale for estimate: multimap.insert() is logarithmic in size, the method adds only one item
    bool add_beacon(BeaconID id, std::string const& name, Coord xy, Color color);

    // Estimate of performance: Average case ϴ(1), worst case O(n)
    // Short rationale for estimate: map.find() is constant on average
    std::string get_name(BeaconID id);

    // Estimate of performance: Average case ϴ(1), worst case O(n)
    // Short rationale for estimate: map.find() is constant on average
    Coord get_coordinates(BeaconID id);

    // Estimate of performance: Average case ϴ(1), worst case O(n)
    // Short rationale for estimate: map.find() is constant on average
    Color get_color(BeaconID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n)
    // Short rationale for estimate: looping through a map, reserving memory for a vector
    std::vector<BeaconID> beacons_alphabetically();

   // Estimate of performance: O(n)
    // Short rationale for estimate: looping through a map, reserving memory for a vector
    std::vector<BeaconID> beacons_brightness_increasing();

    // Estimate of performance: O(1)
    // Short rationale for estimate: map.begin() is constant
    BeaconID min_brightness();

    // Estimate of performance: O(1)
    // Short rationale for estimate: map.rbegin() is constant
    BeaconID max_brightness();

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: looping through a map and then sorting found ids
    std::vector<BeaconID> find_beacons(std::string const& name);

    // Estimate of performance: best case Ω(log n), worst case O(n)
    // Short rationale for estimate: equal_range is logarithmic in size, then looping through the found range
    bool change_beacon_name(BeaconID id, std::string const& newname);

     // Estimate of performance: best case Ω(log n), worst case O(n)
    // Short rationale for estimate: equal_range is logarithmic in size, then looping through the found range
    bool change_beacon_color(BeaconID id, Color newcolor);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: Average case ϴ(1), worst case O(n)
    // Short rationale for estimate: map.find() is constant on average
    bool add_lightbeam(BeaconID sourceid, BeaconID targetid);

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: sorting a vector
    std::vector<BeaconID> get_lightsources(BeaconID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: iterating through targets
    std::vector<BeaconID> path_outbeam(BeaconID id);

    // Non-compulsory operations

    // Estimate of performance: O(n)
    // Short rationale for estimate: removing an item from maps/vectors
    bool remove_beacon(BeaconID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: calls path_inbeam_recursive which calls itself for each source, then converts the deque into vector
    std::vector<BeaconID> path_inbeam_longest(BeaconID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: the method calls itself for each source
    Color total_color(BeaconID id);

    // Phase 2 operations

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: Looping through all xpoints and then sorting them.
    std::vector<Coord> all_xpoints();

    // Estimate of performance: ϴ(log n) on average, O(n) worst case
    // Short rationale for estimate: Inserting into a set is logaritmic,
    // finding from unordered_map is linear in worst case.
    bool add_fibre(Coord xpoint1, Coord xpoint2, Cost cost);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Looping through all fibres from a xpoint.
    std::vector<std::pair<Coord, Cost>> get_fibres_from(Coord xpoint);

    // Estimate of performance: O(n)
    // Short rationale for estimate: Looping through all fibres.
    std::vector<std::pair<Coord, Coord>> all_fibres();

    // Estimate of performance: O(n log n)
    // Short rationale for estimate: Removing from a set is n log n.
    bool remove_fibre(Coord xpoint1, Coord xpoint2);

    // Estimate of performance: O(n)
    // Short rationale for estimate: map.clear() and set.clear() are both linear
    void clear_fibres();

    // Estimate of performance: O(V+E)
    // Short rationale for estimate: Uses a depth-first-search algorithm and loops
    // through the found route
    std::vector<std::pair<Coord, Cost>> route_any(Coord fromxpoint, Coord toxpoint);

    // Non-compulsory operations

    // Estimate of performance: O(V+E)
    // Short rationale for estimate: Uses a breath-first-seach algorithm and loops
    // through the found route
    std::vector<std::pair<Coord, Cost>> route_least_xpoints(Coord fromxpoint, Coord toxpoint);

    // Estimate of performance: O((V+E) log V)
    // Short rationale for estimate: Uses Dijkstra's algorithm and loops
    // through the found route
    std::vector<std::pair<Coord, Cost>> route_fastest(Coord fromxpoint, Coord toxpoint);

    // Estimate of performance: O(V+E)
    // Short rationale for estimate: Uses a depth-first-algorithm and loops
    // through the found route
    std::vector<Coord> route_fibre_cycle(Coord startxpoint);

    // Estimate of performance: Not implemented
    // Short rationale for estimate:
    Cost trim_fibre_network();

private:
    // Add stuff needed for your class implementation here

    // prg1 stuff
    // Calculates the brightness of a color
    int get_brightness(Color color);

    // Recursive implementation for path_inbeam_longest
    void path_inbeam_recursive(BeaconID id, std::deque<BeaconID> atm, std::deque<BeaconID> &longest);

    std::unordered_map<BeaconID, Beacon> beacons_;
    std::multimap<std::string, BeaconID> alphabetical_order_;
    std::multimap<int, BeaconID> brightness_order_;

    // prg2 stuff

    // Resets all xpoints
    void reset_xpoints();

    // Depth-first-search algorithm, which returns true if target node or a
    // loop is found, depending on which is needed.
    bool DFS(std::shared_ptr<Xpoint> from, std::shared_ptr<Xpoint> to,
             bool find_cycle, std::pair<std::shared_ptr<Xpoint>, std::shared_ptr<Xpoint>>& cycle_begin);

    // Breath-first-search algorithm
    void BFS(std::shared_ptr<Xpoint> from);

    // Dijkstra's algorithm
    void dijkstra(std::shared_ptr<Xpoint> from);

    // Relax -function used in Dijkstra's.
    void relax(std::shared_ptr<Xpoint>, std::shared_ptr<Xpoint>, Cost);

    // Route-collecting algorithm that collects a route stored in xpoints
    // pi-pointers.
    void collect_route(std::vector<std::pair<Coord, Cost>>& route, std::shared_ptr<Xpoint> to);

    std::unordered_map<Coord, std::shared_ptr<Xpoint>, CoordHash> xpoints_;
    std::set<std::pair<Coord, Coord>> fibres_;

};

#endif // DATASTRUCTURES_HH
