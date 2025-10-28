#include <iostream>
#include "../headers/SerialSimulator.h"
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>


#define SEM_NAME "/sem_sensor"
#define SHM_SENSOR_NAME "/shm_sensor_data"
#define SHM_POINTERS_NAME "/shm_sensor_pointers"

#define SENSOR_BUFFER_SIZE 2560
#define POINTERS_SIZE 3

long int symulationTime =0;
const int symulationDuration = 50000;
int readingPeriod = 100;


int main()
{
    SerialSimulator serial;
    serial.open("/dev/ttyUSB0", 115200);

    // tworzenie semafora binarnego
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1); // 1 = dostępny
    if (sem == SEM_FAILED) { perror("sem_open"); exit(1); }





    /*#####################################################################################

    tworzenie w  pamieci współdzielonej Circular Queue do kolejkowania danych z czujników

    #######################################################################################*/ 


    // deskryptor danych/bufora Circular Queue
    int shm_fd_sensor = shm_open(SHM_SENSOR_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd_sensor == -1) {
        perror("shm_open sensor");
        exit(1);
    }

    // rozmiar segmentu
    if (ftruncate(shm_fd_sensor, SENSOR_BUFFER_SIZE) == -1) {
        perror("ftruncate sensor");
        exit(1);
    }

    // mapowanie do przestrzeni adresowej
    uint8_t *sensorData = (uint8_t *)mmap(NULL, SENSOR_BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_sensor, 0);
    if (sensorData == MAP_FAILED) {
        perror("mmap sensorData");
        exit(1);
    }
    close(shm_fd_sensor);

    // deskryptor zmiennych pomocniczych Circular Queue -> wskaźnika początku kolejki, końsca kolejki oraz licza elementów
    int shm_fd_pointers = shm_open(SHM_POINTERS_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd_pointers == -1) {
        perror("shm_open pointers");
        exit(1);
    }
    // rozmiar segmentu
    if (ftruncate(shm_fd_pointers, POINTERS_SIZE) == -1) {
        perror("ftruncate pointers");
        exit(1);
    }
    // mapowanie do przestrzeni adresowej
    uint8_t *frontRearPointers = (uint8_t *)mmap(NULL, POINTERS_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_pointers, 0);
    if (frontRearPointers == MAP_FAILED) {
        perror("mmap frontRearPointers");
        exit(1);
    }
    close(shm_fd_pointers);
   
    sem_wait(sem);
    frontRearPointers[0] = 0; // front
    frontRearPointers[1] = 0; // rear
    frontRearPointers[2] = 0; // size 
    sem_post(sem);
    

     /*#####################################################################################

                                główna pętla symulacji

    #######################################################################################*/ 


    
    while(symulationTime < symulationDuration){
        if (serial.isOpen()) {


            uint8_t buffer[256];
            // odczyt danych do buffora
            size_t bytes_read = serial.read(buffer, sizeof(buffer), symulationTime);
        
            

            // zapis danych do Circular Queue w pamięci współdzielonej
            sem_wait(sem);
            if(frontRearPointers[2]<255){  //sprawdzenie czy kolejka nie jest przepełniona
                memcpy(sensorData + (frontRearPointers[0]*10), buffer, 10); // dla uproszczenia kopiujemy 10 pierwszych bajtów, w zeczywistości należu dekodowac dane za pomocą protokołów jak np. MAVLink
                frontRearPointers[2] ++; 
                if(frontRearPointers[0]<255)frontRearPointers[0] ++;
                else frontRearPointers[0] =0;
                
            }
            else std::cout << "Przpełnienie bufora, dane pootencjalnie utracone: " << buffer << std::endl;
            sem_post(sem);
        }

        symulationTime +=readingPeriod;
        usleep(readingPeriod*1000);
    }

    std::cout << "Nacisnij Enter, aby kontynuowac..." << std::endl;
    std::cin.get(); 



    // czyszczenie
    munmap(sensorData, SENSOR_BUFFER_SIZE);
    munmap(frontRearPointers, POINTERS_SIZE);
    shm_unlink(SHM_SENSOR_NAME);
    shm_unlink(SHM_POINTERS_NAME);
    sem_unlink(SEM_NAME);

    sem_close(sem);
    sem_unlink(SEM_NAME);
    
    serial.close();
    return 0;
}