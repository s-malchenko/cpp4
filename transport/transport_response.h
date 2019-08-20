#pragma once
#include <map>
#include <ostream>
#include <string>
#include <string_view>
#include <set>

enum ResponseType
{
    BUS,
    STOP,
};

class TransportResponse
{
public:
    TransportResponse(const std::string& name, unsigned int id, bool error) :
        _name(name),
        _error(error),
        _id(id) {}
    virtual ~TransportResponse() = default;
    virtual void Proceed(std::ostream &out) const  = 0;
    virtual void ProceedJSON(std::ostream &out) const = 0;
protected:
    std::string _name;
    bool _error;
    unsigned int _id;
    static const std::string ERROR_MESSAGE;
};

class BusResponse : public TransportResponse
{
public:
    BusResponse(const std::string& name, unsigned int id = 0, bool error = false);
    virtual ~BusResponse()  = default;
    virtual void Proceed(std::ostream &out) const override;
    virtual void ProceedJSON(std::ostream &out) const override;
    BusResponse& Length(unsigned int len);
    BusResponse& Stops(size_t count);
    BusResponse& UniqueStops(size_t count);
    BusResponse& Curvature(long double value);
    unsigned int Length() const;
    size_t Stops() const;
    size_t UniqueStops() const;
    long double Curvature() const;
private:
    unsigned int _routeLength;
    long double _curvature;
    size_t _stopCount;
    size_t _uniqueStopCount;
};

class StopResponse : public TransportResponse
{
public:
    StopResponse(const std::string& name, unsigned int id = 0, bool error = false);
    virtual ~StopResponse() = default;
    virtual void Proceed(std::ostream &out) const override;
    virtual void ProceedJSON(std::ostream &out) const override;
    StopResponse& Buses(std::set<std::string_view> buses);
    const std::set<std::string_view> &Buses() const;
private:
    std::set<std::string_view> _buses;
};

