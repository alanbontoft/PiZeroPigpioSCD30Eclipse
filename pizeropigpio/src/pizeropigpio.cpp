//============================================================================
// Name        : pizeropigpio.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <unistd.h>
#include <pigpio.h>

bool startSCD30(int handle);
bool getSCD30Status(int handle);
bool getSCD30Readings(int handle);

#define PIN 6

#define MILLISEC 1000
#define PAUSE 5000

float fCO2, fTemperature, fRH;

using namespace std;

int main()
{
	cout << "SCD30 Test Program" << endl;

    int hI2C;

    if (gpioInitialise() < 0)
    {
       cout << "GPIO init failed" << endl;
    }
    else
    {
        cout << "GPIO init OK" << endl;

        hI2C = i2cOpen(1, 0x61, 0);

        if (hI2C < 0)
        {
            cout << "I2C bad handle: " << hI2C << endl;
        }
        else
        {
            // start sensor
            startSCD30(hI2C);
            usleep(100 * MILLISEC);

            while (true)
            {
                if (getSCD30Status(hI2C))
                {
                    usleep(PAUSE);

                    if (getSCD30Readings(hI2C))
                    {
                        cout << fCO2 << "," << fTemperature << "," << fRH << endl;
                    }
                }

                usleep(500 * MILLISEC);
            }
        }
    }
	return 0;
}



bool startSCD30(int handle)
{
    char startCmd[] = {0x00, 0x10, 0x00, 0x00};

    return (i2cWriteDevice(handle, startCmd, 4) == 0);
}

bool getSCD30Status(int handle)
{
    char statusCmd[] = {0x02, 0x02};
    char datain[3] = { 0 };

    if (i2cWriteDevice(handle, statusCmd, 2) != 0) return false;

    usleep(PAUSE);

    if (i2cReadDevice(handle, datain, 3) != 3) return false;

    return (datain[1] == 1);
}

bool getSCD30Readings(int handle)
{
    char readCmd[] = {0x03, 0x00};
    char dataBuffer[18] = { 0 };
    uint8_t co2Bytes[4] = { 0 };
    uint8_t tempBytes[4] = { 0 };
    uint8_t rhBytes[4] = { 0 };
    int expected = 18;

    // send reading request
    if (i2cWriteDevice(handle,  readCmd, 2) != 0) return false;

    // wait 5ms
    usleep(PAUSE);

    // read back data
    if (i2cReadDevice(handle, dataBuffer, expected) != expected) return false;

    // convert to floats
    co2Bytes[0] = dataBuffer[4];
    co2Bytes[1] = dataBuffer[3];
    co2Bytes[2] = dataBuffer[1];
    co2Bytes[3] = dataBuffer[0];

    tempBytes[0] = dataBuffer[10];
    tempBytes[1] = dataBuffer[9];
    tempBytes[2] = dataBuffer[7];
    tempBytes[3] = dataBuffer[6];

    rhBytes[0] = dataBuffer[16];
    rhBytes[1] = dataBuffer[15];
    rhBytes[2] = dataBuffer[13];
    rhBytes[3] = dataBuffer[12];

    fCO2 = *(float*)co2Bytes;

    fTemperature = *(float*)tempBytes;

    fRH = *(float*)rhBytes;

    return true;
}
