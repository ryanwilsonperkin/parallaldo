#ifndef PARALLALDO_WP_H
#define PARALLALDO_WP_H

#define TRUE 1
#define FALSE 0

typedef int boolean;
enum WorkerInstruction {WORKER_QUIT, WORKER_TASK};

int worker(int id, void *p);

#endif // PARALLALDO_WP_H
