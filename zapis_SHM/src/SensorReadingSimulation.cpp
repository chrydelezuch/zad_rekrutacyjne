#include "../headers/SensorReadingSimulation.h"

// symulacja czujnika ciśnienie BMP280 - mierzenie wysokości
// założenie jest że dane są przesłane już po kompensacji przeprowadzonej na dronie wg. algorytmu przedstawionego w dokumentacji BMP280
// dron lata w górę i dół cyklicznie, amplituda 40m
uint32_t SensorReadingSimulation::generatePressureSensorReading(long int time, double baseValue) {
    double h = 30.0 + 20 * sin(time / 1000.0);  // wysokość
    double p = baseValue * exp(-1.0 * mi * g * h / (T * R));  // ciśnienie
    std::cout << "Ciśnienie: " << p << std::endl;

    return (uint32_t)(p * 256);
}


//symulacja odczytów z czujnika poziomu bateri
//założenie jest takie że poziom naładowania baterii jest obliczany na dronie na podstwie np. odczytów poziomu napięcia INA219
//poziom naładowania baterii jest przekazywany przez dron za pomocą liczby całkowitej
//gdy dron leci do góry zużycie energi rośnie, gdy w dół to maleje, dla uproszczenia 
//jedna milisekunda w symulacji to 1s w rzeczywistości


uint16_t SensorReadingSimulation::generateBatteryLevelReading(long int time) {
    //double I = (dronePower + (0.05 * dronePower * (2 + sin(time / 1000.0)))) / batteryVoltage;  // prąd [A]
    double I = dronePower / batteryVoltage;

    batteryCapacity = batteryCapacity  - ((I * time) / 3600);  // spadek pojemności (mAh)
    double batteryLevel = batteryCapacity / initialBatteryCapacity;

    if (batteryLevel < 0.0) batteryLevel = 0.0;
    std::cout << "Bateria: " << batteryLevel << std::endl;

    return (uint16_t)(batteryLevel * 0xFFFF);
}

// symulacja odczytu z czujnika temperatury silników NTC 10kOhm
// ADC miktokontrolera 16 bitowy
// gdy dron leci do góry temperatura rośnie, gdy w dół to maleje 

uint16_t SensorReadingSimulation::generateMotorTemperatureReading(long int time) {
    double temp = 305.0 + 2.5 * sin(time / 1000.0); // temperatura [K]
    double NTC_R = 10000 * exp(3950 * (1 / temp - 1 / 298.15)); // rezystancja NTC [Ω]
    std::cout << "Temperatura (rezystancja NTC): " << NTC_R << std::endl;

    return (uint16_t)((NTC_R / (NTC_R + 100)) * 0xFFFF);
}
