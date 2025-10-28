import mmap
import posix_ipc
import time

import matplotlib.pyplot as plt

import numpy as np
from collections import deque

import sensor_decode as sd


LOG_FILE = "sensor_log.txt"
QUEUE_MAXLEN = 512

SHM_NAME_SENSOR = "/shm_sensor_data"   # nazwa segmentu z sensorData
SHM_NAME_POINTERS = "/shm_sensor_pointers" # nazwa segmentu z frontRearPointers
SEM_NAME = "/sem_sensor"
ELEMENT_SIZE = 10  # każdy odczyt ma 10 bajtów
QUEUE_SIZE = 256   # maksymalna liczba elementów w Circular Queue



def add(y1q, y2q, y3q, new_y1, new_y2, new_y3):
    
    #Dodaje nowe dane do kolejek. 
    #Jeśli kolejka jest pełna, usuwa najstarszy element i zapisuje go do pliku.
    if len(y1q) == QUEUE_MAXLEN:
        # zapis najstarszego elementu do pliku
        with open(LOG_FILE, "a") as f:
            f.write(f"{y1q[0]}, {y2q[0]}, {y3q[0]}\n")
        # usunięcie najstarszego elementu
        y1q.popleft()
        y2q.popleft()
        y3q.popleft()
    
    # dodanie nowych danych
    y1q.append(new_y1)
    y2q.append(new_y2)
    y3q.append(new_y3)


# Dane
x_data = []
y1_fig_data = deque(maxlen=QUEUE_MAXLEN)
y2_fig_data = deque(maxlen=QUEUE_MAXLEN)
y3_fig_data = deque(maxlen=QUEUE_MAXLEN)



def main():

    # otwarcie semafora
    sem = posix_ipc.Semaphore(SEM_NAME)

    # otwieramy segmenty pamięci współdzielonej
    shm_sensor = posix_ipc.SharedMemory(SHM_NAME_SENSOR)
    shm_pointers = posix_ipc.SharedMemory(SHM_NAME_POINTERS)


    sensor_map = mmap.mmap(shm_sensor.fd, shm_sensor.size)
    pointers_map = mmap.mmap(shm_pointers.fd, shm_pointers.size)

    # Zamykamy deskryptory, bo mmap już mapuje pamięć
    shm_sensor.close_fd()
    shm_pointers.close_fd()



    # pętla główna


    try:
        i=0
        while True:
        

            sem.acquire()

            data = None
            # odczytujemy front i size
            front = pointers_map[0]
            rear = pointers_map[1]
            size = pointers_map[2]
    
            if size > 0:

                # kopiujemy dane z bufora
                start = rear * ELEMENT_SIZE
                data = sensor_map[start:start+ELEMENT_SIZE]

                # aktualizacja wskaźników Circular Queue
                if rear < 255:
                    rear +=1
                else:
                    rear =0

                size -= 1
    
                pointers_map[1] = rear
                pointers_map[2] = size

            sem.release()

            if data is not None:
                i=0
           
                pressure =  sd.decode_pressure_sensor(int.from_bytes(data[0:4], byteorder='little'), 100000.0)
                battery =sd.decode_battery_level(int.from_bytes(data[4:6], byteorder='little')) 
                temp =sd.decode_motor_temperature(int.from_bytes(data[6:8], byteorder='little'))


                add(y1_fig_data, y2_fig_data, y3_fig_data, pressure, battery, temp)

            

                print(f"Pressure={pressure}, Battery={battery}, Temp={temp}")
            else:
                # brak danych, czekamy chwilę
                time.sleep(0.01)
                i+=1
                #jeśli po 200 cykla nie ma nowch danych kończymy program
                if i >200:
                    break

        #tworzenie wykresu
        fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(8, 8))

        size = len(list(y1_fig_data))
        x_show = []
        if size < 256:
            x_data = np.arange(size)
            ax1.plot(x_data, list(y1_fig_data)[-size:], color='blue', label='wysokość [m]')
            ax2.plot(x_data, list(y2_fig_data)[-size:], color='green', label='poziom naładowania [%]')
            ax3.plot(x_data, list(y3_fig_data)[-size:], color='red', label='temp. w [K]')
        else:
            x_data = np.arange(256)
            ax1.plot(x_data, list(y1_fig_data)[-256:], color='blue', label='wysokość [m]')
            ax2.plot(x_data, list(y2_fig_data)[-256:], color='green', label='poziom naładowania [%]')
            ax3.plot(x_data, list(y3_fig_data)[-256:], color='red', label='temp. w [K]')


        ax1.legend(loc='upper right')
        ax2.legend(loc='upper right')
        ax3.legend(loc='upper right')

        plt.tight_layout()
        plt.show()


        # sprzątanie
        sensor_map.close()
        pointers_map.close()

    except KeyboardInterrupt:
        print("Zakończone")


if __name__ == "__main__":
    main()


