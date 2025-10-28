#ifndef SENSOR_READING_SIMULATION_H
#define SENSOR_READING_SIMULATION_H

#include <cmath>
#include <iostream>

class SensorReadingSimulation {
public:
    // --- Stałe fizyczne ---
    static constexpr double R = 8.31446261815324;   // stała gazowa
    static constexpr double mi = 0.0289644;         // masa molowa powietrza
    static constexpr double g = 9.8;                // przyspieszenie ziemskie
    static constexpr double T = 293.00;             // temperatura powietrza [K]

    // --- Parametry drona ---
    static constexpr double dronePower = 100.0;     // [W]
    static constexpr double batteryVoltage = 7.4;   // [V]
    static constexpr double initialBatteryCapacity = 2000.0; // [mAh]

private:
    double batteryCapacity; // aktualna pojemność baterii [mAh]

public:
    SensorReadingSimulation()
        : batteryCapacity(initialBatteryCapacity) {}

    // Metody symulacji 
    uint32_t generatePressureSensorReading(long int time, double baseValue);
    uint16_t generateBatteryLevelReading(long int time);
    uint16_t generateMotorTemperatureReading(long int time);
};

#endif 
