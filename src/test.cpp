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

    queue_t * q;
    q = new queue_t;

    queue_create(q, 255);

    sample_t a;
    a.queue = 1;
    a.kind = 'B';
    a.quantity = 66;

    queue_append(q, a);

    printf("%c", dequeue(q)->kind);
    queue_delete(q);

    return 0;
}
