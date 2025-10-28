## Opis projektu
Projekt składa się z dwóch części:
1. **Program w C++** symulujący odczyty czujników drona:
   - wysokości,
   - temperatury silników,
   - poziomu baterii.
   
   Dane są zapisywane w pamięci współdzielonej (SHM), aby mogły być odczytane przez inne procesy.

2. **Program w Pythonie** odczytujący dane z pamięci współdzielonej i wizualizujący zmiany parametrów w czasie na wykresach.

3. Kompilacja programu C++: g++ main.cpp SerialSimulator.cpp SensorReadingSymulation.cpp -o sim

4. Wykożystane biblioteki python:
   matplotlib, numpy, posix_ipc, mmap
