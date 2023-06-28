#include <cstdio>
#include <thread>
#include "logger.h"

#include <sdbus-c++/sdbus-c++.h>

#include "touchhandler.h"


using namespace std;

int main()
{



    const char* serviceName = "org.sdbuscpp.concatenator";
    auto connection = sdbus::createSystemBusConnection(serviceName);

    const char* objectPath = "/org/sdbuscpp/concatenator";
    auto sdbusObject = sdbus::createObject(*connection, objectPath);

    const char* interfaceName = "org.sdbuscpp.Concatenator";
    sdbusObject->registerSignal(interfaceName, "touchEvent", "s");
    sdbusObject->finishRegistration();

    TouchHandler th {"/dev/input/event3", sdbusObject.get(), interfaceName};
    std::thread t{&TouchHandler::run, th};

    //th.run();


    connection->enterEventLoop();

    return 0;
}


