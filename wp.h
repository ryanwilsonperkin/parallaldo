#ifndef PARALLALDO_WP_H
#define PARALLALDO_WP_H

enum WorkerInstruction {WORKER_QUIT, WORKER_TASK};

int worker(int id, void *p);

#endif // PARALLALDO_WP_H
