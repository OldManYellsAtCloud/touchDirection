#include <cstdio>
#include <thread>
#include "logger.h"

#include <sdbus-c++/sdbus-c++.h>

#include "touchhandler.h"


using namespace std;

int main()
{
    const char* serviceName = "org.gspine.gesture";
    auto connection = sdbus::createSystemBusConnection(serviceName);

    const char* objectPath = "/org/gspine/gesture";
    auto sdbusObject = sdbus::createObject(*connection, objectPath);

    const char* interfaceName = "org.gspine.Gesture";
    sdbusObject->registerSignal(interfaceName, "touchEvent", "s");
    sdbusObject->finishRegistration();

    TouchHandler th {"/dev/input/event3", sdbusObject.get(), interfaceName};
    std::thread t {&TouchHandler::run, th};

    connection->enterEventLoop();

    return 0;
}


