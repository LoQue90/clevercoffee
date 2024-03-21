/**
 * @file switchHandler.h
 *
 * @brief Handler for all digital switches
 */

/**
 * @brief Handler for digital power switch
 */

uint8_t currStatePowerSwitch; // the current reading from the input pin

void checkPowerSwitch() {
    if (FEATURE_POWERSWITCH) {
        uint8_t powerSwitchReading = powerSwitch->isPressed();

        if (POWERSWITCH_TYPE == Switch::TOGGLE) {
            // Set pidON to 1 when powerswitch is HIGH
            if ((powerSwitchReading == HIGH && machineState != kStandby) || (powerSwitchReading == LOW && machineState == kStandby)) {
                pidON = 1;
            }
            else {
                // Set pidON to 0 when powerswitch is not HIGH
                pidON = 0;
            }
        }
        else if (POWERSWITCH_TYPE == Switch::MOMENTARY) {
            // if the button state has changed:
            if (powerSwitchReading != currStatePowerSwitch) {
                currStatePowerSwitch = powerSwitchReading;

                // only toggle heating power if the new button state is HIGH
                if (currStatePowerSwitch == HIGH) {
                    if (pidON == 0) {
                        pidON = 1;
                    }
                    else {
                        pidON = 0;
                    }
                }
            }
        }
    }
}

/**
 * @brief Handler for digital water switch
 */

uint8_t currStateWaterSwitch;

void checkWaterSwitch() {
    if (FEATURE_WATERSWITCH) {
        uint8_t waterSwitchReading = waterSwitch->isPressed();

        if (WATERSWITCH_TYPE == Switch::TOGGLE) {

            if (waterSwitchReading == HIGH) {
                LOG(INFO, "Start pump for hot water");
                pumpRelay.on();
            }
            else {
                LOG(INFO, "Stop pump");
                pumpRelay.off();
            }
        }
        else if (WATERSWITCH_TYPE == Switch::MOMENTARY) {

            if (waterSwitchReading != currStateWaterSwitch) {
                currStateWaterSwitch = waterSwitchReading;

                if (currStateWaterSwitch == HIGH) {
                    LOG(INFO, "Start pump for hot water");
                    pumpRelay.on();
                }
                else {
                    LOG(INFO, "Stop pump")
                    pumpRelay.off();
                }
            }
        }
    }
}

/**
 * @brief Handler for digital steam switch
 */

uint8_t currStateSteamSwitch;

void checkSteamSwitch() {
    if (FEATURE_STEAMSWITCH) {
        uint8_t steamSwitchReading = steamSwitch->isPressed();

        if (STEAMSWITCH_TYPE == Switch::TOGGLE) {
            // Set steamON to 1 when steamswitch is HIGH
            if (steamSwitchReading == HIGH) {
                steamON = 1;
            }

            // if activated via web interface then steamFirstON == 1, prevent override
            if (steamSwitchReading == LOW && !steamFirstON) {
                steamON = 0;
            }

            // monitor QuickMill thermoblock steam-mode
            if (machine == QuickMill) {
                if (steamQM_active == true) {
                    if (checkSteamOffQM() == true) { // if true: steam-mode can be turned off
                        steamON = 0;
                        steamQM_active = false;
                        lastTimeOptocouplerOn = 0;
                    }
                    else {
                        steamON = 1;
                    }
                }
            }
        }
        else if (STEAMSWITCH_TYPE == Switch::MOMENTARY) {
            if (steamSwitchReading != currStateSteamSwitch) {
                currStateSteamSwitch = steamSwitchReading;

                // only toggle heating power if the new button state is HIGH
                if (currStateSteamSwitch == HIGH) {
                    if (steamON == 0) {
                        steamON = 1;
                    }
                    else {
                        steamON = 0;
                    }
                }
            }
        }
    }
}
