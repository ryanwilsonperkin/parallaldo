#ifndef PARALLALDO_WP_H
#define PARALLALDO_WP_H

#define TRUE 1
#define FALSE 0

typedef int boolean;

/*
 * enum WorkerInstruction
 *  An instruction protocol to be sent to a worker process.
 *
 *  WORKER_QUIT: instruction to quit
 *  WORKER_TASK: instruction to run find_parallaldo algorithm using the supplied data
 */
enum WorkerInstruction {WORKER_QUIT, WORKER_TASK};

/*
 * worker
 *  A worker process that satisfies the Pilot API and executes in parallel.
 *  
 *  id: the identification number of the process
 *  p: required by Pilot API, unused
 *
 *  returns: exit code used by Pilot API
 */
int worker(int id, void *p);

/*
 * serial
 *  Run the search for Parallaldos within Images on a single process (PI_MAIN).
 *
 *  n_parallaldos: number of Parallaldo files
 *  n_images: number of Image files
 */
void serial(int n_parallaldos, int n_images);

/*
 * round_robin
 *  Run the search for Parallaldos within Images on multiple processes.
 *  Rotate through available worker processes assigning new tasks as previous ones finish.
 *
 *  n_procs: number of available processes (not including PI_MAIN)
 *  n_parallaldos: number of Parallaldo files
 *  n_images: number of Image files
 */
void round_robin(int n_procs, int n_parallaldos, int n_images);

/*
 * load_balanced
 *  Run the search for Parallaldos within Images on multiple processes.
 *  Assign new tasks to whichever worker processes have most recently finished. 
 *
 *  n_procs: number of available processes (not including PI_MAIN)
 *  n_parallaldos: number of Parallaldo files
 *  n_images: number of Image files
 */
void load_balanced(int n_procs, int n_parallaldos, int n_images);

#endif // PARALLALDO_WP_H
