#include "shared_data.h"

mem_id create_shm(config_init_t * cfg) {
    if (cfg == NULL) {
        perror("Error al crear la memoria dada la configuracion.");
        exit(1);
    }
    
    mem_shared_t * context = new mem_shared_t;
    
    // Cola de salida.
    queue_create(&context->output_queue, cfg->queue_output_length);

    // Colas de entrada.
    context->input_queue = new queue_t[cfg->entries];
    
    for (unsigned int i = 0; i < cfg->entries; ++i) {
        queue_create(&i[context->input_queue], cfg->queue_input_length);
    }

    context->queues = (size_t)cfg->entries;

}

mem_shared_t * read_shm(mem_id id) {
    // TODO - Terminar
    return NULL;
}

int queue_create(queue_t * q, unsigned int size) {
    if (q != NULL) {
        q->size = size;
        q->pointer = 0;
        q->head = 0;

        q->buffer = new sample_t[q->size];
        return sizeof(q);
    }

    return -1;
}

int queue_append(queue_t * q, sample_t sample) {
    if (q != NULL) {
        if (q->pointer < q->size) {
            q->buffer[q->pointer++] = sample;
            
            return q->pointer;
        }
    }

    return -1;
}

const sample_t * dequeue(queue_t * q) {
    if (q != NULL) {
        if (q->head < q->pointer) {
            return &q->buffer[q->head++]; 
        }
    }

    return NULL;
}

void queue_delete(queue_t * q) {
    if (q != NULL) {
        delete[] q->buffer;
        delete q;
    }
}
