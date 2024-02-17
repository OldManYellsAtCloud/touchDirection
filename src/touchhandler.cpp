#include <string.h>
#include <map>
#include <err.h>
#include <errno.h>
#include <poll.h>
#include <unistd.h>
#include <linux/input-event-codes.h>
#include <loglibrary.h>

#include "touchhandler.h"

void TouchHandler::sendSignal(Direction direction){
    std::string signalType;
    switch (direction){
    case Direction::BOTTOM_TO_TOP:
        signalType = DIRECTION_BOTTOM_TO_TOP;
        break;
    case Direction::TOP_TO_BOTTOM:
        signalType = DIRECTION_TOP_TO_BOTTOM;
        break;
    case Direction::LEFT_TO_RIGHT:
        signalType = DIRECTION_LEFT_TO_RIGHT;
        break;
    default:
        signalType = DIRECTION_RIGHT_TO_LEFT;
        break;
    }

    auto signal = _sdbus->createSignal(_sdbusInterface, signalType);
    _sdbus->emitSignal(signal);
}

TouchHandler::TouchHandler(const std::string& filepath, sdbus::IObject* sdbus, const char* sdbusInterface)
{
    _sdbus = sdbus;
    _sdbusInterface = sdbusInterface;
    eventSource = fopen(filepath.c_str(), "r");
    if (!eventSource) {
        ERROR("Could not open event source: {}, error: {}", filepath, strerror(errno));
        exit(1);
    }
}

void TouchHandler::calculateDirection(){
    DEBUG("Calculating direction x1: {}, x2: {}, y1: {}, y2: {}", std::to_string(first_x),
           std::to_string(last_x), std::to_string(first_y), std::to_string(last_y));

    if (std::abs(first_x - last_x) < 50) { // vertical movement
        if ( last_y - first_y < 0 ) {
            if (first_y > (BOTTOM_Y - MAX_DEVIATION)) {
                DEBUG("BOTTOM_TO_TOP movement");
                sendSignal(Direction::BOTTOM_TO_TOP);
            }
        } else if (first_y < (TOP_Y + MAX_DEVIATION)) {
            DEBUG("TOP_TO_BOTTOM movement");
            sendSignal(Direction::TOP_TO_BOTTOM);
        }
    } else { //horizontal movement
        if (last_x - first_x > 0) {
            if (first_x < MAX_DEVIATION) {
                DEBUG("LEFT_TO_RIGHT movement");
                sendSignal(Direction::LEFT_TO_RIGHT);
            }
        } else if (first_x > (RIGHT_X - MAX_DEVIATION)) {
            DEBUG("RIGHT_TO_LEFT movement");
            sendSignal(Direction::RIGHT_TO_LEFT);
        }
    }

    state = State::WAITING;
}

void TouchHandler::collectData(const input_event& ie){
    if ((event_num_x + event_num_y) < 20 || event_num_x == 0 || event_num_y == 0) {
        switch (ie.code) {
        case ABS_X:
            if (event_num_x++ == 0)
                first_x = ie.value;
            last_x = ie.value;
            break;

        case ABS_Y:
            if (event_num_y++ == 0)
                first_y = ie.value;
            last_y = ie.value;
            break;
        default:
            break;
        }
    } else {
        state = State::DATA_COLLECTED;
    }
}

void TouchHandler::run()
{
    struct pollfd pfd[1];
    pfd[0].fd = fileno(eventSource);
    pfd[0].events = POLLIN;

    int poll_timeout_ms = 2000;
    nfds_t nfds = 1;

    struct input_event event;


    while (!done){
        poll(pfd, nfds, poll_timeout_ms);
        if (pfd[0].revents & POLLIN) {
            fread(&event, sizeof(input_event), 1, eventSource);

            if (event.type == EV_KEY && event.code == BTN_TOUCH){
                if (event.value == 1) {
                    event_num_x = event_num_y = 0;
                    state = State::COLLECTING_DATA;
                } else {
                    state = State::WAITING;
                }
            } else if ( event.type == EV_ABS && (event.code == ABS_X || event.code == ABS_Y)){
                if (state == State::COLLECTING_DATA) {
                    collectData(event);
                } else if (state == State::DATA_COLLECTED){
                    calculateDirection();
                }
            }
        }
    }
}

