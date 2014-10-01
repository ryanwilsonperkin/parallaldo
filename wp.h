#ifndef PARALLALDO_WP_H
#define PARALLALDO_WP_H

#define TRUE 1
#define FALSE 0

typedef int boolean;
enum WorkerInstruction {WORKER_QUIT, WORKER_TASK};

int worker(int id, void *p);
void serial(int n_parallaldos, int n_images);
void round_robin(int n_procs, int n_parallaldos, int n_images);
void load_balanced(int n_procs, int n_parallaldos, int n_images);

#endif // PARALLALDO_WP_H
