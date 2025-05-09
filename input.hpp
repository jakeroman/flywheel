#ifndef FLYWHEEL_INPUT_HPP
#define FLYWHEEL_INPUT_HPP

class FlywheelInput {
public:
    // Configure input pins
    void begin() {
        pinMode(4, INPUT_PULLUP); // Configure pin as input with pull-up enabled
        pinMode(5, INPUT_PULLUP); // Configure pin as input with pull-up enabled
        pinMode(6, INPUT_PULLUP); // Configure pin as input with pull-up enabled
        pinMode(7, INPUT_PULLUP); // Configure pin as input with pull-up enabled
        pinMode(15, INPUT_PULLUP); // Configure pin as input with pull-up enabled
        pinMode(16, INPUT_PULLUP); // Configure pin as input with pull-up enabled
    }

    bool check_up() {
        return digitalRead(6) == LOW;
    }
    bool check_down() {
        return digitalRead(4) == LOW;
    }
    bool check_left() {
        return digitalRead(5) == LOW;
    }
    bool check_right() {
        return digitalRead(7) == LOW;
    }
    bool check_a() {
        return digitalRead(16) == LOW;
    }
    bool check_b() {
        return digitalRead(15) == LOW;
    }
};

#endif
