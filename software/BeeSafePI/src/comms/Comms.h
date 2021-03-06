/**
 * \file Comms.h
 * \class Comms
 *
 * \defgroup Comms Comms
 * \brief The package handling the communication aspects of the project
 * \ingroup Comms
 *
 * \brief A high-level wrapper for the communication features
 *
 * The Comms class acts as a high-level wrapper over the UBlox class, enabling the underlying device objects to be invoked in a thread-safe
 * manner, concurrently. By calling the UBlox commands describing higher level communications functions such as getting a location and
 * sending a text message, and "wrapping it" in a mutex lock it ensures the safe interactions with and operations of the
 * hardware connected to the Raspberry Pi. Unless specified otherwise, functions within the class will block access to prevent
 * errors / side-effects from propagating in lower levels; this is difficult to manage given the interoperability of functions.
 *
 * \author BeeSafe Team, Team 13
 *
 * \version v1.0
 *
 * \date 2020/04/20
 *
 * Contact: beesafe.uofg@gmail.com
 *
 * Licence: MIT
 */

#ifndef BEESAFEPI_COMMUNICATION_H
#define BEESAFEPI_COMMUNICATION_H

#include "UBlox.h"

#include "../contact/Contact.h"

// Include system libraries.
#include <string>
#include <utility>
#include <mutex>


class Comms
{

public:

    // Constructor and destructor.
    explicit Comms();
    ~Comms();

public:

    // Initialise the comms interface.
    bool init();

    // Get the uBlox device that's being communicated to.
    const UBlox &getUBlox();

    // Connection specific functions.
    bool hasRegistered(bool &registered);
    bool hasGPRS(bool &attached);
    bool hasPSD(bool &connected);

    // Establish connections.
    bool startAutoRegistration(bool &registered);
    bool connectPSD(bool &connected, std::string &urc);

    // Get information about the device.
    bool getModelNumber(std::string &modelNumber);
    bool getIMEI(std::string &imeiNumber);

    // Getting and setting the message mode.
    bool getSendMessageMode(char &sendMsgMode);
    bool setSendMessageMode(char sendMsgMode);

    // Getting and setting the location scan mode.
    bool getLocationScanMode(char &locScanMode);
    bool setLocationScanMode(char locScanMode);

    // Getting the CellLocate location from the device.
    bool getLocation(std::pair<double, double> &latLng);
    bool getLocation(double &lat, double &lng);

    // Sending a text message
    bool sendMessage(Contact &contact, const std::string &message);
    bool sendMessage(const std::string &phoneNumber, const std::string &message);


private:

    // The u-Blox device that is being communicated with.
    UBlox uBlox;

    // The lock utilised to prevent concurrent access.
    std::mutex mtx;

};

#endif //BEESAFEPI_COMMUNICATION_H



