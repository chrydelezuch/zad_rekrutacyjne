// serial_simulator.h
#ifndef SERIAL_SIMULATOR_H
#define SERIAL_SIMULATOR_H

#include <cstddef>
#include <cstdint>

class SerialSimulator {
public:
    SerialSimulator();
    ~SerialSimulator() = default;
    
    // Podstawowe metody portu szeregowego
    bool open(const char* port = "COM1", int baudrate = 9600);
    void close();
    bool isOpen() const;
    
    // Odczyt danych - do implementacji
    size_t available() const;
    size_t read(uint8_t* buffer, size_t size, unsigned int time);
    
    // Zapis danych - opcjonalnie
    size_t write(const uint8_t* data, size_t size);

private:
    bool connected;
};

#endif