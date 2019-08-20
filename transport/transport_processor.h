#pragma once
#include "bus_route.h"
#include "bus_stop.h"

#include <istream>
#include <ostream>
#include <iostream>
#include <string>
#include <unordered_map>

class TransportProcessor
{
public:
    TransportProcessor(std::ostream &out);
    void ProcessDatabaseRequest(const std::string &request);
    void PrepareDatabase();
    void ProcessReadingRequest(const std::string &request);
private:
    std::ostream &_out;
    std::unordered_map<std::string, BusStop> _stopsBase;
    std::unordered_map<std::string, BusRoute<std::unordered_map<std::string, BusStop>>> _busesBase;
};

void RunTransportProcessor(std::istream &in = std::cin, std::ostream &out = std::cout);
