#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include "shared_data.h"

int _maint() {
    int mem = shm_open("mateo", O_CREAT | O_RDWR, 0644);

    if (mem < 0) {
        perror("Error al crear o abrir la memoria");
        exit(1);
    }



    return 0;
}
