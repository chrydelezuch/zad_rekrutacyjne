#include "../headers/SerialSimulator.h"
#include "../headers/SensorReadingSimulation.h"
#include <iostream>
#include <string.h>

SerialSimulator::SerialSimulator(): connected(false) {
}

bool SerialSimulator::open(const char* port, int baudrate) {
    std::cout << "Opening serial port: " << port << " at " << baudrate << " baud" << std::endl;
    connected = true;
    return true;
}

void SerialSimulator::close() {
    std::cout << "Closing serial port" << std::endl;
    connected = false;
}

bool SerialSimulator::isOpen() const {
    return connected;
}

size_t SerialSimulator::available() const {
    return connected ? 1024 : 0;
}

size_t SerialSimulator::read(uint8_t* buffer, size_t size, unsigned int time) {
    if (!connected || size == 0) {
        return 0;
    }

    SensorReadingSimulation sim;

    uint8_t sensorReadings[8];
    uint32_t pressure = sim.generatePressureSensorReading(time, 100000.0);
    uint16_t batteryLevel = sim.generateBatteryLevelReading(time);
    uint16_t temp = sim.generateMotorTemperatureReading(time);


    memcpy(sensorReadings, &pressure, sizeof(pressure));
    memcpy(sensorReadings+4, &batteryLevel, sizeof(batteryLevel));
    memcpy(sensorReadings+6, &temp, sizeof(temp));
    
    for (size_t i = 0; i < size; ++i) {
        if(i<8) buffer[i] = sensorReadings[i];
        else buffer[i] = 0;
    }
    return size;
}

size_t SerialSimulator::write(const uint8_t* data, size_t size) {
    if (!connected) {
        return 0;
    }
    
    std::cout << "Written " << size << " bytes to serial" << std::endl;
    return size;
}