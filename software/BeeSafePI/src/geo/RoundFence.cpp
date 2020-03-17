#include "RoundFence.h"

#include <cmath>

// Explicit round fence constructor.
RoundFence::RoundFence(bool safe, const std::map<int, std::vector<std::pair<std::tm, std::tm>>> &week, double latitude,
                       double longitude, double radius)
        : Fence(safe, week)
{
    this->latitude = latitude;
    this->longitude = longitude;
    this->radius = radius;
}

// Basic RoundFence constructor.
RoundFence::RoundFence(bool safe, double latitude, double longitude, double radius)
        : Fence(safe)
{
    this->latitude = latitude;
    this->longitude = longitude;
    this->radius = radius;
}

// Copy constructor for the round fence object.
RoundFence::RoundFence(const RoundFence &roundFence)
        : Fence(roundFence)
{
    this->latitude = roundFence.latitude;
    this->longitude = roundFence.longitude;
    this->radius = roundFence.radius;
}

// Get the latitude of the fence.
double RoundFence::getLatitude()
{
    return latitude;
}

// Get the longitude of the fence.
double RoundFence::getLongitude()
{
    return longitude;
}

// Get the radius of the fence.
double RoundFence::getRadius()
{
    return radius;
}

// Calculates whether or not latitude and longitude inside fence.
bool RoundFence::isInside(std::pair<double, double> &latLng)
{
    double distance = std::sqrt((latLng.first - this->longitude) * (latLng.first - this->latitude)
                                + (latLng.second - this->longitude) * (latLng.second - this->longitude));
    return distance <= radius;
}

// Serialise the fence into a JSON element.
web::json::value RoundFence::serialiseFence()
{

    // Serialise the super-class fence.
    web::json::value jsonFence = Fence::serialiseFence();

    // Serialise RoundFence specific attributes.
    jsonFence[U(JSON_KEY_FENCE_FENCE)][U(JSON_KEY_ROUND_FENCE_LATITUDE)]
            = web::json::value::number(latitude);
    jsonFence[U(JSON_KEY_FENCE_FENCE)][U(JSON_KEY_ROUND_FENCE_LONGITUDE)]
            = web::json::value::number(longitude);;
    jsonFence[U(JSON_KEY_FENCE_FENCE)][U(JSON_KEY_ROUND_FENCE_RADIUS)]
            = web::json::value::number(radius);

    // Return the serialised fence.
    return jsonFence;
}