#include <cstdio>
#include <thread>
#include <format>
#include <fstream>
#include <string>
#include <sstream>
#include <loglibrary.h>

#include <sdbus-c++/sdbus-c++.h>

#include "touchhandler.h"

#define INPUT_EVENT_PATH "/dev/input/{}"
#define INPUT_DEVICES "/proc/bus/input/devices"
#define PHYS_TYPE "P: Phys"
#define TOUCHSCREEN_TYPE "input/ts"
#define HANDLER_TYPE "H: Handlers"
#define EVENT_HANDLER "event"

using namespace std;

bool isLineTouchscreenName(const std::string& line){
    return line.starts_with(PHYS_TYPE) && line.ends_with(TOUCHSCREEN_TYPE);
}

bool isLineHandlerType(const std::string& line){
    return line.starts_with(HANDLER_TYPE);
}

bool isEventId(const std::string& line){
    return line.starts_with(EVENT_HANDLER);
}

std::string extractEventId(const std::string& line){
    std::stringstream tokens{line};
    std::string eventId;

    do {
        tokens >> eventId;
    } while (!isEventId(eventId) && tokens.good());

    if (!isEventId(eventId))
        return "";

    return eventId;
}

std::string getNextEventId(std::ifstream& stream){
    std::string line;
    std::string eventId = "";
    while (stream.good()){
        std::getline(stream, line);
        if (isLineHandlerType(line)){
            eventId = extractEventId(line);
            break;
        }
    }
    return eventId;
}

std::string findTouchScreenEvent(){
    std::ifstream inputDevices{INPUT_DEVICES, ios_base::in};
    std::string evendId;
    if (!inputDevices.is_open()){
        ERROR("Could not open input devices!");
        exit(1);
    }

    std::string line;
    std::string eventId;
    while (inputDevices.good()){
        std::getline(inputDevices, line);
        if (isLineTouchscreenName(line)){
            eventId = getNextEventId(inputDevices);
            break;
        }
    }

    inputDevices.close();
    return eventId;
}

int main()
{

    std::string touchScreenEvent = findTouchScreenEvent();
    if (touchScreenEvent == "") {
        ERROR("Could not find touchscreen device!");
        exit(1);
    }

    sdbus::ServiceName serviceName{"org.gspine.gesture"};
    auto connection = sdbus::createBusConnection(serviceName);

    sdbus::ObjectPath objectPath{"/org/gspine/gesture"};
    auto sdbusObject = sdbus::createObject(*connection, std::move(objectPath));

    sdbus::InterfaceName interfaceName{"org.gspine.Gesture"};
    sdbusObject->addVTable(sdbus::SignalVTableItem{sdbus::MethodName{DIRECTION_LEFT_TO_RIGHT}, {}, {}}).forInterface(interfaceName);
    sdbusObject->addVTable(sdbus::SignalVTableItem{sdbus::MethodName{DIRECTION_RIGHT_TO_LEFT}, {}, {}}).forInterface(interfaceName);
    sdbusObject->addVTable(sdbus::SignalVTableItem{sdbus::MethodName{DIRECTION_TOP_TO_BOTTOM}, {}, {}}).forInterface(interfaceName);
    sdbusObject->addVTable(sdbus::SignalVTableItem{sdbus::MethodName{DIRECTION_BOTTOM_TO_TOP}, {}, {}}).forInterface(interfaceName);

    TouchHandler th {std::format(INPUT_EVENT_PATH, touchScreenEvent), sdbusObject.get(), interfaceName};
    std::thread t {&TouchHandler::run, th};

    connection->enterEventLoop();

    return 0;
}


