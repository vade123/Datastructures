// Datastructures.cc

#include "datastructures.hh"

#include <random>
#include <cmath>
#include <algorithm>
#include <stack>
#include <queue>

// ---------------------------- PROVIDED BY THE COURSE ------------------------

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// ----------------------------------------------------------------------------

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures() :
    beacons_({}),
    alphabetical_order_({}),
    brightness_order_({})
{
}

Datastructures::~Datastructures()
{
}

int Datastructures::beacon_count()
{
    return static_cast<int>(beacons_.size());
}

void Datastructures::clear_beacons()
{
    beacons_.clear();
    alphabetical_order_.clear();
    brightness_order_.clear();
}


std::vector<BeaconID> Datastructures::all_beacons()
{
    std::vector<BeaconID> ids;
    ids.reserve(beacons_.size());
    for (const auto& beacon : beacons_) {
        ids.push_back(beacon.first);
    }
    return ids;
}

bool Datastructures::add_beacon(BeaconID id, const std::string& name, Coord xy, Color color)
{
    const auto result = beacons_.find(id);
    if (result != beacons_.end()) {
        return false;
    }
    int new_beacon_brightness = get_brightness(color);
    Beacon new_beacon = { id, name, xy, color, new_beacon_brightness };
    beacons_[id] = new_beacon;
    alphabetical_order_.insert({name, id});
    brightness_order_.insert({new_beacon_brightness, id});
    return true;
}

std::string Datastructures::get_name(BeaconID id)
{
    const auto result = beacons_.find(id);
    if (result != beacons_.end()) {
        return result->second.name;
    }
    return NO_NAME;
}

Coord Datastructures::get_coordinates(BeaconID id)
{
    const auto result = beacons_.find(id);
    if (result != beacons_.end()) {
        return result->second.coords;
    }
    return NO_COORD;
}

Color Datastructures::get_color(BeaconID id)
{
    const auto result = beacons_.find(id);
    if (result != beacons_.end()) {
        return result->second.color;
    }
    return NO_COLOR;
}


std::vector<BeaconID> Datastructures::beacons_alphabetically()
{
    std::vector<BeaconID> ids = {};
    ids.reserve(alphabetical_order_.size());
    for (const auto& beacon : alphabetical_order_) {
        ids.push_back(beacon.second);
    }
    return ids;
}

std::vector<BeaconID> Datastructures::beacons_brightness_increasing()
{
    std::vector<BeaconID> ids = {};
    ids.reserve(brightness_order_.size());
    for (const auto& beacon : brightness_order_) {
        ids.push_back(beacon.second);
    }
    return ids;
}

BeaconID Datastructures::min_brightness()
{
    if (brightness_order_.size() == 0){
        return NO_ID;
    }
    const auto it = brightness_order_.begin();
    return it->second;
}

BeaconID Datastructures::max_brightness()
{
    if (brightness_order_.size() == 0){
        return NO_ID;
    }
    const auto it = brightness_order_.rbegin();
    return it->second;
}

std::vector<BeaconID> Datastructures::find_beacons(std::string const& name)
{
    std::vector<BeaconID> found_ids;
    for (const auto& beacon : beacons_) {
        if (beacon.second.name == name){
            found_ids.push_back(beacon.second.id);
        }
    }
    std::sort(found_ids.begin(),found_ids.end());
    return found_ids;
}

bool Datastructures::change_beacon_name(BeaconID id, const std::string& newname)
{
    auto result = beacons_.find(id);
    if (result != beacons_.end()) {
        std::string oldname = result->second.name;
        result->second.name = newname;
        auto range = alphabetical_order_.equal_range(oldname);
        for (auto i = range.first; i != range.second; ++i){
            if (i->second == id) {
                auto node_handler = alphabetical_order_.extract(i->first);
                node_handler.key() = newname;
                alphabetical_order_.insert(std::move(node_handler));
                return true;
            }
        }
    }
    return false;
}

bool Datastructures::change_beacon_color(BeaconID id, Color newcolor)
{
    auto result = beacons_.find(id);
    if (result != beacons_.end()) {
        int new_brightness = get_brightness(newcolor);
        int old_brightness = result->second.brightness;
        result->second.color = newcolor;
        result->second.brightness = new_brightness;
        auto range = brightness_order_.equal_range(old_brightness);
        for (auto& i = range.first; i != range.second; ++i) {
            if (i->second == id) {
                auto node_handler = brightness_order_.extract(i->first);
                node_handler.key() = new_brightness;
                brightness_order_.insert(std::move(node_handler));
                return true;
            }
        }
    }
    return false;
}


bool Datastructures::add_lightbeam(BeaconID sourceid, BeaconID targetid)
{
    auto source_it = beacons_.find(sourceid);
    auto target_it = beacons_.find(targetid);
    if ( source_it == beacons_.end() or target_it == beacons_.end()) {
        return false;
    } else if (source_it->second.target != NO_ID) {
        return false;
    }
    source_it->second.target = targetid;
    target_it->second.sources.push_back(sourceid);
    return true;
}

std::vector<BeaconID> Datastructures::get_lightsources(BeaconID id)
{
    auto result = beacons_.find(id);
    if (result == beacons_.end()) {
        return {{NO_ID}};
    }
    auto sources = result->second.sources;
    std::sort(sources.begin(), sources.end());
    return sources;
}

std::vector<BeaconID> Datastructures::path_outbeam(BeaconID id)
{
    const auto result = beacons_.find(id);
    if (result == beacons_.end()) {
        return {{NO_ID}};
    }
    std::vector<BeaconID> ids;
    ids.push_back(id);
    auto target = result->second.target;
    while (target != NO_ID) {
        ids.push_back(target);
        target = beacons_.at(target).target;
    }
    return ids;
}

bool Datastructures::remove_beacon(BeaconID id)
{
    const auto result = beacons_.find(id);
    if (result == beacons_.end()) {
        return false;
    }
    const auto& target = result->second.target;
    if (target != NO_ID) {
        auto& targets_sources = beacons_.at(target).sources;
        targets_sources.erase(std::remove_if(targets_sources.begin(), targets_sources.end(),
                                             [=](auto& source){ return source == id; }));
    }
    for (const auto& source : result->second.sources){
        beacons_.at(source).target = NO_ID;
    }
    for (auto it = alphabetical_order_.begin(); it != alphabetical_order_.end(); ++it){
        if (it->second == id) {
            alphabetical_order_.erase(it);
            break;
        }
    }
    for (auto it = brightness_order_.begin(); it != brightness_order_.end(); ++it){
        if (it->second == id) {
            brightness_order_.erase(it);
            break;
        }
    }
    beacons_.erase(result);
    return true;
}

std::vector<BeaconID> Datastructures::path_inbeam_longest(BeaconID id)
{
    auto result = beacons_.find(id);
    if (result == beacons_.end()) {
        return {{NO_ID}};
    }
    std::deque<BeaconID> atm;
    std::deque<BeaconID> longest;
    path_inbeam_recursive(id, atm, longest);
    std::vector<BeaconID> longest_vector(longest.begin(), longest.end());
    return longest_vector;

}

void Datastructures::path_inbeam_recursive(BeaconID id, std::deque<BeaconID> atm, std::deque<BeaconID>& longest)
{
    auto result = beacons_.find(id);
    atm.push_front(id);
    if (atm.size() > longest.size()){
        longest = atm;
    }
    for (const auto& source : result->second.sources) {
        path_inbeam_recursive(source, atm, longest);
    }

}

Color Datastructures::total_color(BeaconID id)
{
    auto result = beacons_.find(id);
    if (result == beacons_.end()) {
        return NO_COLOR;
    }
    Color color = beacons_.at(id).color;
    const auto& sources = beacons_.at(id).sources;
    if (sources.size() > 0) {
        int r = color.r;
        int g = color.g;
        int b = color.b;
        for (auto& source : sources) {
            Color source_color = total_color(source);
            r += source_color.r;
            g += source_color.g;
            b += source_color.b;
        }
        int number_of_beacons = static_cast<int>(sources.size()) + 1;
        color = { r / number_of_beacons,
                  g / number_of_beacons,
                  b / number_of_beacons };
    }
    return color;
}

int Datastructures::get_brightness(Color color)
{
    return 3 * color.r + 6 * color.g + color.b;
}

std::vector<Coord> Datastructures::all_xpoints()
{
    std::vector<Coord> all_xpoints = {};
    for (const auto& xpoint : xpoints_) {
        all_xpoints.push_back(xpoint.first);
    }
    std::sort(all_xpoints.begin(), all_xpoints.end());
    return all_xpoints;
}

bool Datastructures::add_fibre(Coord xpoint1, Coord xpoint2, Cost cost)
{
    if (xpoint1 == xpoint2) {
        return false;
    }
    auto result1 = xpoints_.find(xpoint1);
    auto result2 = xpoints_.find(xpoint2);

    if (result1 == xpoints_.end()) {
        Xpoint new_xpoint = {xpoint1};
        auto new_ptr = std::make_shared<Xpoint>(new_xpoint);
        xpoints_[xpoint1] = new_ptr;
    }
    if (result2 == xpoints_.end()) {
        Xpoint new_xpoint = {xpoint2};
        auto new_ptr = std::make_shared<Xpoint>(new_xpoint);
        xpoints_[xpoint2] = new_ptr;
    }
    auto& xp1 = xpoints_.at(xpoint1);
    auto& xp2 = xpoints_.at(xpoint2);

    if (xp1->fibres.find(xpoint2) != xp1->fibres.end()){
        return false;
    }
    xp1->fibres[xpoint2] = { xp2, cost };
    xp2->fibres[xpoint1] = { xp1, cost };

    if(xpoint1 < xpoint2){
        fibres_.insert({xpoint1, xpoint2});
    } else {
        fibres_.insert({xpoint2, xpoint1});
    }
    return true;
}

std::vector<std::pair<Coord, Cost> > Datastructures::get_fibres_from(Coord xpoint)
{
    if (xpoints_.find(xpoint) == xpoints_.end()) {
        return {};
    }
    auto& xp = xpoints_.at(xpoint);
    if (xp->fibres.size() == 0) {
        return {};
    }
    std::vector<std::pair<Coord, Cost>> fibres_from;
    for(const auto& fibre : xp->fibres) {
        fibres_from.push_back({fibre.first, fibre.second.second});
    }
    std::sort(fibres_from.begin(), fibres_from.end());
    return fibres_from;
}

std::vector<std::pair<Coord, Coord> > Datastructures::all_fibres()
{
    std::vector<std::pair<Coord, Coord>> fibres;
    for(const auto& fibre : fibres_) {
        fibres.push_back({fibre.first, fibre.second});
    }
    return fibres;
}

bool Datastructures::remove_fibre(Coord xpoint1, Coord xpoint2)
{
    auto result1 = xpoints_.find(xpoint1);
    auto result2 = xpoints_.find(xpoint2);
    if ((result1 == xpoints_.end()) or (result2 == xpoints_.end())){
        return false;
    }
    auto fibre1 = result1->second->fibres.find(xpoint2);
    auto fibre2 = result2->second->fibres.find(xpoint1);
    if ((fibre1 == result1->second->fibres.end()) or
            (fibre2 == result2->second->fibres.end())){
        return false;
    } else {
        result1->second->fibres.erase(fibre1);
        result2->second->fibres.erase(fibre2);
    }
    if (result1->second->fibres.empty()){
        xpoints_.erase(result1);
    }
    if (result2->second->fibres.empty()){
        xpoints_.erase(result2);
    }
    std::pair<Coord, Coord> to_be_removed;
    if (xpoint1 < xpoint2) {
        to_be_removed = {xpoint1, xpoint2};
    } else {
        to_be_removed = {xpoint2, xpoint1};
    }
    auto result = fibres_.find(to_be_removed);
    fibres_.erase(result);
    return true;
}

void Datastructures::clear_fibres()
{
    xpoints_.clear();
    fibres_.clear();
}

void Datastructures::reset_xpoints() {
    for(auto& xpoint : xpoints_){
        xpoint.second->state = WHITE;
        xpoint.second->pi = nullptr;
        xpoint.second->d = INT_MAX;
        xpoint.second->route_cost = 0;
    }
}

std::vector<std::pair<Coord, Cost> > Datastructures::route_any(Coord fromxpoint, Coord toxpoint)
{
    auto result1 = xpoints_.find(fromxpoint);
    auto result2 = xpoints_.find(toxpoint);
    if ((result1 == xpoints_.end()) or (result2 == xpoints_.end())){
        return {};
    }
    std::pair<std::shared_ptr<Xpoint>, std::shared_ptr<Xpoint>> cycle_pair = { nullptr, nullptr };
    if (!DFS(xpoints_.at(fromxpoint), xpoints_.at(toxpoint), false, cycle_pair)){
        return {};
    }
    std::vector<std::pair<Coord, Cost>> route;
    collect_route(route, xpoints_.at(toxpoint));
    return route;
}

bool Datastructures::DFS(std::shared_ptr<Xpoint> from, std::shared_ptr<Xpoint> to, bool find_cycle, std::pair<std::shared_ptr<Xpoint>, std::shared_ptr<Xpoint>>& cycle_begin)
{
    reset_xpoints();
    std::stack<std::shared_ptr<Xpoint>> stack;
    stack.push(from);

    while (!stack.empty()) {
        std::shared_ptr<Xpoint> u = stack.top();
        stack.pop();
        if (u->state == WHITE) {
            u->state = GRAY;
            stack.push(u);
            for(auto& fibre : u->fibres) {
                auto& v = fibre.second.first;
                if (v->state == WHITE){
                    v->pi = u;
                    v->route_cost = u->route_cost + fibre.second.second;
                    if (v == to){
                        return true;
                    }
                    stack.push(v);
                } else if (v->state == GRAY){
                    if (find_cycle and v != u->pi){
                        cycle_begin.first = u;
                        cycle_begin.second = v;
                        return true;
                    }
                }
            }
        } else {
            u->state = BLACK;
        }
    }
    return false;
}

void Datastructures::BFS(std::shared_ptr<Xpoint> from)
{
    reset_xpoints();
    std::queue<std::shared_ptr<Xpoint>> queue;

    from->state = GRAY;
    from->d = 0;
    queue.push(from);

    while (!queue.empty()) {
        std::shared_ptr<Xpoint> u = queue.front();
        queue.pop();
        for(auto& fibre : u->fibres) {
            auto& v = fibre.second.first;
            if (v->state == WHITE){
                v->state = GRAY;
                v->pi = u;
                v->route_cost = u->route_cost + fibre.second.second;
                queue.push(v);
            }
        }
        u->state = BLACK;
    }
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_least_xpoints(Coord fromxpoint, Coord toxpoint)
{
    auto result1 = xpoints_.find(fromxpoint);
    auto result2 = xpoints_.find(toxpoint);
    if ((result1 == xpoints_.end()) or (result2 == xpoints_.end())){
        return {};
    }

    BFS(xpoints_.at(fromxpoint));

    if (xpoints_.at(toxpoint)->pi == nullptr){
        return {};
    }
    std::vector<std::pair<Coord, Cost>> route;
    collect_route(route, xpoints_.at(toxpoint));
    return route;
}

void Datastructures::collect_route(std::vector<std::pair<Coord, Cost>>& route, std::shared_ptr<Xpoint> to)
{
    std::shared_ptr<Xpoint> ptr = to;
    while (ptr != nullptr) {
        route.push_back({ptr->coords, ptr->route_cost});
        ptr = ptr->pi;
    }
    std::reverse(route.begin(), route.end());
}

std::vector<std::pair<Coord, Cost>> Datastructures::route_fastest(Coord fromxpoint, Coord toxpoint)
{
    auto result1 = xpoints_.find(fromxpoint);
    auto result2 = xpoints_.find(toxpoint);
    if ((result1 == xpoints_.end()) or (result2 == xpoints_.end())){
        return {};
    }
    dijkstra(xpoints_.at(fromxpoint));

    if (xpoints_.at(toxpoint)->pi == nullptr){
        return {};
    }

    std::vector<std::pair<Coord, Cost>> route;
    collect_route(route, xpoints_.at(toxpoint));
    return route;
}

void Datastructures::dijkstra(std::shared_ptr<Xpoint> from)
{
    reset_xpoints();
    std::priority_queue<std::shared_ptr<Xpoint>, std::vector<std::shared_ptr<Xpoint>>, Prio_que_op> min_queue;

    from->state = GRAY;
    from->d = 0;
    min_queue.push(from);

    while (!min_queue.empty()){
        std::shared_ptr<Xpoint> u = min_queue.top();
        min_queue.pop();
        for(auto& fibre : u->fibres) {
            auto& v = fibre.second.first;
            relax(u, v, fibre.second.second);
            if (v->state == WHITE){
                v->state = GRAY;
                min_queue.push(v);
            }  
        }
        u->state = BLACK;
    }
}

void Datastructures::relax(std::shared_ptr<Xpoint> u, std::shared_ptr<Xpoint> v, Cost w)
{
    if (v->d > u->d + w) {
        v->d = u->d + w;
        v->pi = u;
        v->route_cost = u->route_cost + w;
    }
}

std::vector<Coord> Datastructures::route_fibre_cycle(Coord startxpoint)
{
    auto result = xpoints_.find(startxpoint);
    if (result == xpoints_.end()){
        return {};
    }

    std::pair<std::shared_ptr<Xpoint>, std::shared_ptr<Xpoint>> cycle_pair = { nullptr, nullptr };
    if (!DFS(xpoints_.at(startxpoint), nullptr, true, cycle_pair)){
        return {};
    }

    std::vector<Coord> route;
    route.push_back(cycle_pair.second->coords);
    std::shared_ptr<Xpoint> ptr = cycle_pair.first;
    while (ptr != nullptr) {
        route.push_back(ptr->coords);
        if (ptr->coords == route.at(0)) {
            break;
        }
        ptr = ptr->pi;
    }
    return route;
}

Cost Datastructures::trim_fibre_network()
{
    // Replace this with your implementation
    return NO_COST;
}
