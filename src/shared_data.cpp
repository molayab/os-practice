#include "shared_data.h"

mem_id create_shm(config_init_t * cfg) {
    if (cfg == NULL) {
        perror("Error al crear la memoria dada la configuracion.");
        exit(1);
    }

    int mem = shm_open(cfg->_id.c_str(), O_CREAT | O_RDWR, 0644);

    if (mem < 0) {
        perror("Error al crear o abrir la memoria.");
        exit(1);
    }

    size_t l = sizeof(mem_shared_t) + (cfg->entries * sizeof(queue_t *));
    ftruncate(mem, l);

    mem_shared_t * context = (mem_shared_t *)mmap(NULL, l, PROT_READ|PROT_WRITE, MAP_SHARED, mem, 0);
    if (context == MAP_FAILED) {
      perror("Error al mapear la memoria.");
      exit(1);
	  }

    // Cola de salida.
    queue_create(&context->output_queue, cfg->queue_output_length);

    // Colas de entrada.
    context->input_queue = new queue_t[cfg->entries];

    for (unsigned int i = 0; i < cfg->entries; ++i) {
        queue_create(&i[context->input_queue], cfg->queue_input_length);
    }

    context->queues = (size_t)cfg->entries;
    // unsigned int queue_sample_length;

    queue_create(&context->queue_b, cfg->queue_sample_length);
    queue_create(&context->queue_d, cfg->queue_sample_length);
    queue_create(&context->queue_s, cfg->queue_sample_length);

    return cfg->_id.c_str();
}

mem_shared_t * read_shm(mem_id id, config_init_t * cfg) {
    int fd = shm_open(id, O_RDONLY, 0666);

    size_t l = sizeof(mem_shared_t) + (cfg->entries * sizeof(queue_t *));
    mem_shared_t * base = (mem_shared_t *)mmap(0, l, PROT_READ, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
      perror("Error al mapear (read_shm) la memoria.");
      exit(1);
    }

    return base;
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
