#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "electraAcRemoteEncoder.h"

// Function to initialize the air conditioner structure

void initializeStruct(struct airCon *newAcPointer)
{
    int i = 0;

    // Set default values for air conditioner properties
    newAcPointer->fullState = FULL_STATE_MASK;
    newAcPointer->fan = FAN_MASK;
    newAcPointer->mode = MODE_MASK;
    newAcPointer->temp = TEMP_MASK;
    newAcPointer->swing = SWING_MASK;

    // Initialize manchesterTimings array to 0
    for (i = 0; i < TIMINGS_LENGTH; i++)
    {
        (newAcPointer->manchesterTimings)[i] = 0;
    }
}

// Function to update fan property of the air conditioner
void updateFan(int value, struct airCon *newAcPointer)
{
    // Update fan property using bitwise operations
    newAcPointer->fan &= (value - 1);
    newAcPointer->fan = (newAcPointer->fan) << 28;
    newAcPointer->fullState |= newAcPointer->fan;
}

// Function to update mode property of the air conditioner
void updateMode(int value, struct airCon *newAcPointer)
{
    // Update mode property using bitwise operations
    newAcPointer->mode &= (value);
    newAcPointer->mode = (newAcPointer->mode) << 30;
    newAcPointer->fullState |= newAcPointer->mode;
}

// Function to update temperature property of the air conditioner
void updateTemperature(int value, struct airCon *newAcPointer)
{
    // Update temperature property using bitwise operations
    newAcPointer->temp &= (value - 15);
    newAcPointer->temp = (newAcPointer->temp) << 19;
    newAcPointer->fullState |= newAcPointer->temp;
}

// Function to update swing property of the air conditioner
void updateSwing(int value, struct airCon *newAcPointer)
{
    // Update swing property using bitwise operations
    newAcPointer->swing &= (value);
    newAcPointer->swing = (newAcPointer->swing) << 25;
    newAcPointer->fullState |= newAcPointer->swing;
}

// Function to update a parameter (fan, mode, temperature, or swing) of the air conditioner
void updateParameter(acParameter parameter, int value, struct airCon *newAcPointer)
{
    switch (parameter)
    {
    case fan:
        updateFan(value, newAcPointer);
        break;
    case mode:
        updateMode(value, newAcPointer);
        break;
    case temp:
        updateTemperature(value, newAcPointer);
        break;
    case swing:
        updateSwing(value, newAcPointer);
        break;
    }
}

// Function to convert the air conditioner state to an array of bit strings
void convertStateToBitStrings(struct airCon *newAcPointer)
{
    int i = 0;
    uint64_t toggleMask = 1, fullStateCopy;

    // Make a copy of the full state for manipulation
    fullStateCopy = newAcPointer->fullState;

    // Loop through the full state and convert each bit to a string
    for (i = DATA_BITS_LENGTH - 1; i >= 0; i--)
    {
        sprintf((newAcPointer->bitStrings)[i], "%d", fullStateCopy & toggleMask);
        fullStateCopy = fullStateCopy >> 1;
    }
}

// Function to convert bit strings to Manchester encoded string
void convertBitStringsToManchesterString(struct airCon *newAcPointer)
{
    int i = 0;
    for (i = 0; i < DATA_BITS_LENGTH; i++)
    {
        if ((newAcPointer->bitStrings)[i][0] == '0')
        {
            strcpy((newAcPointer->manchesterString) + 2 * i, "01");
        }
        else
        {
            strcpy((newAcPointer->manchesterString) + 2 * i, "10");
        }
    }
}

// Function to convert Manchester encoded string to Manchester timings
void convertManchesterStringToManchesterTimings(struct airCon *newAcPointer)
{
    int stringPointer = 0;
    int timingPointer = 0;
    int counter = 0;
    int i = 0;

    // Set initial timing value
    (newAcPointer->manchesterTimings)[timingPointer] = 3000;
    timingPointer++;

    // Check first bit of Manchester string
    if ((newAcPointer->manchesterString)[0] == '1')
    {
        (newAcPointer->manchesterTimings)[timingPointer] = 4000;
        stringPointer++;
    }
    else
    {
        (newAcPointer->manchesterTimings)[timingPointer] = 3000;
    }
    timingPointer++;

    // Loop through the rest of the Manchester string
    for (stringPointer; stringPointer < DATA_BITS_LENGTH * 2; stringPointer++)
    {

        // Count consecutive '0's
        while ((newAcPointer->manchesterString)[stringPointer] == '0')
        {
            counter++;
            stringPointer++;
        }
        (newAcPointer->manchesterTimings)[timingPointer] = 1000 * counter;
        counter = 0;
        timingPointer++;

        // Count consecutive '1's
        while ((newAcPointer->manchesterString)[stringPointer] == '1')
        {
            counter++;
            stringPointer++;
        }
        (newAcPointer->manchesterTimings)[timingPointer] = 1000 * counter;
        counter = 0;
        timingPointer++;
        stringPointer--;
    }

    // Set the last timing value
    (newAcPointer->manchesterTimings)[3 * timingPointer] = 4000;

    // Duplicate timings for transmission
    for (i = 0; i < timingPointer; i++)
    {
        (newAcPointer->manchesterTimings)[i + timingPointer] = (newAcPointer->manchesterTimings)[i];
        (newAcPointer->manchesterTimings)[i + timingPointer * 2] = (newAcPointer->manchesterTimings)[i];
    }
}

// Function to get the Manchester timings for sending a command
int *getCodes(struct airCon *newAc, int fanV, int modeV, int tempV, int state, int swingV)
{

    initializeStruct(newAc);
    updateParameter(fan, fanV, newAc);
    updateParameter(temp, tempV, newAc);
    updateParameter(mode, modeV, newAc);
    updateParameter(swing, swingV, newAc);

    convertStateToBitStrings(newAc);
    if (state == ON)
    {
        (newAc->bitStrings)[0][0] = '0';
    }
    convertBitStringsToManchesterString(newAc);
    convertManchesterStringToManchesterTimings(newAc);

    return newAc->manchesterTimings;
}
