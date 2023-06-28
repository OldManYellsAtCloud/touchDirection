#ifndef TOUCHHANDLER_H
#define TOUCHHANDLER_H

#include <string>
#include <linux/input.h>
#include <sdbus-c++/sdbus-c++.h>

enum class Direction {
    LEFT_TO_RIGHT,
    RIGHT_TO_LEFT,
    TOP_TO_BOTTOM,
    BOTTOM_TO_TOP,
    INVALID
};

enum class State {
    WAITING,
    COLLECTING_DATA,
    DATA_COLLECTED,
    DIRECTION_DETERMINED
};

class TouchHandler
{
private:
    FILE* eventSource;
    bool done = false;

    sdbus::IObject* _sdbus;
    const char* _sdbusInterface;

    unsigned int event_num_x = 0;
    unsigned int event_num_y = 0;

    int first_x = 0;
    int first_y = 0;
    int last_x = 0;
    int last_y = 0;

    void calculateDirection();
    void collectData(const input_event& ie);
    void sendSignal(Direction direction);

    State state = State::WAITING;

    const unsigned int MAX_DEVIATION = 30;
    const unsigned int TOP_Y = 0;
    const unsigned int BOTTOM_Y = 1439;

    const unsigned int LEFT_X = 0;
    const unsigned int RIGHT_X = 719;

    std::map<Direction, std::string> directionDict {{Direction::BOTTOM_TO_TOP, "BOTTOM_TO_TOP"},
                                                    {Direction::TOP_TO_BOTTOM, "TOP_TO_BOTTOM"},
                                                    {Direction::LEFT_TO_RIGHT, "LEFT_TO_RIGHT"},
                                                    {Direction::RIGHT_TO_LEFT, "RIGHT_TO_LEFT"}};

protected:
    void setCoordinates(unsigned int x1, unsigned int x2, unsigned int y1, unsigned int y2) {
        first_x = x1;
        last_x = x2;
        first_y = y1;
        last_y = y2;
    }

public:

    TouchHandler(const std::string& filepath, sdbus::IObject* sdbus, const char* sdbusInterface);
    ~TouchHandler(){}

    void run();
};

#endif // TOUCHHANDLER_H
