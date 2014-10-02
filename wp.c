// TODO: Add license comment at top of each header file.
#include <pilot.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filenames.h"
#include "parallaldo.h"
#include "wp.h"

#define PRINT_FORMAT "%c%s %s (%d,%d,%d)\n"
#define PRINT_PREFIX '$'

PI_CHANNEL **g_results;         // global list of result channels (worker -> PI_MAIN)
PI_CHANNEL **g_instructions;    // global list of instruction channels (PI_MAIN -> worker)
PI_BUNDLE *g_results_bundle;    // global bundle of instruction channels (PI_MAIN -> worker)
char **g_parallaldo_files;      // global list of parallaldo file names
char **g_image_files;           // global list of image file names

/*
 * worker
 *  Repeatedly read instructions from channel until WORKER_QUIT is received.
 *  When WORKER_TASK is received, perform find_parallaldo search using supplied parallaldo_id and image_id.
 *  Return parallaldo_id, image_id, and resulting Position to PI_MAIN.
 */
// TODO: Move Parallaldo, Image, Position declaration within case statement.
// TODO: Use pragma define to ignore unused value p.
int worker(int id, void *p)
{
    enum WorkerInstruction instruction;
    Parallaldo parallaldo;
    Image image;
    Position position;
    int parallaldo_id, image_id;

    while (1) {
        PI_Read(g_instructions[id], "%d%d%d", (int *)&instruction, &parallaldo_id, &image_id);
        switch (instruction) {
            case WORKER_TASK: {
                parallaldo = load_parallaldo(g_parallaldo_files[parallaldo_id]);
                image = load_image(g_image_files[image_id]);
                position = find_parallaldo(parallaldo, image);

                PI_Write(g_results[id], "%d%d%d%d%d", parallaldo_id, image_id, position.y, position.x, position.r);

                free_parallaldo(parallaldo);
                free_image(image);
                break;
            }
            case WORKER_QUIT:
                return 0;
        } 
    }
}

/*
 * serial
 *  Iterate through list of Parallaldo and Image files.
 *  Load each Parallaldo and Image and perform find_parallaldo algorithm.
 *  Print out results.
 */ 
// TODO: Test position.y && position.x not position.r
void serial(int n_parallaldos, int n_images)
{
    for (int i = 0; i < n_parallaldos; i++) {
        for (int j = 0; j < n_images; j++) {
            Parallaldo parallaldo = load_parallaldo(g_parallaldo_files[i]);
            Image image = load_image(g_image_files[j]);
            Position position = find_parallaldo(parallaldo, image);

            if (position.y >= 0 && position.x >= 0 && position.r >= 0) {
                printf(PRINT_FORMAT,
                        PRINT_PREFIX,
                        g_parallaldo_files[i],
                        g_image_files[j],
                        position.y,
                        position.x,
                        position.r);
            }

            free_parallaldo(parallaldo);
            free_image(image);
        }
    }
}

/*
 * round_robin
 *  Initially write all instruction to processes, evenly distributed.
 *  Loop until all results have been returned.
 *  While looping, block until a process returns a result.
 *  Print out any returned results.
 */
// TODO: Determine whether PI_Select still constitutes round robin method.
void round_robin(int n_procs, int n_parallaldos, int n_images)
{
    int recv_parallaldo_id, recv_image_id, y, x, r;
    int num_results = 0, process_id = 0;

    for (int parallaldo_id = 0; parallaldo_id < n_parallaldos; parallaldo_id++) {
        for (int image_id = 0; image_id < n_images; image_id++) {
            PI_Write(g_instructions[process_id], "%d%d%d", (int)WORKER_TASK, parallaldo_id, image_id);
            process_id = (process_id + 1) % n_procs;
        }
    }

    while (num_results < (n_parallaldos * n_images)) {
        process_id = PI_Select(g_results_bundle);
        PI_Read(g_results[process_id], "%d%d%d%d%d", &recv_parallaldo_id, &recv_image_id, &y, &x, &r);
        printf(PRINT_FORMAT, PRINT_PREFIX,
               g_parallaldo_files[recv_parallaldo_id],
               g_image_files[recv_image_id],
               y, x, r);
        num_results++;
    }
}

/*
 * load_balanced
 *  Initially write an instruction out to each available process.
 *  Loop until all results have been returned.
 *  While looping, block until a process returns a result.
 *  While looping, send out new instructions to processes that return results.
 *  Print out any returned results.
 */
// TODO: Handle edge case where n_procs > (n_parallaldos * n_images)
void load_balanced(int n_procs, int n_parallaldos, int n_images)
{
    int recv_parallaldo_id, recv_image_id, x, y, r;
    int parallaldo_id = 0, image_id = 0;
    int num_results = 0, num_sent = 0, process_id = 0;

    for ( ; process_id < n_procs; process_id++) {
        PI_Write(g_instructions[process_id], "%d%d%d", (int)WORKER_TASK, parallaldo_id, image_id);
        num_sent++;

        parallaldo_id += (image_id + 1) / n_images;
        image_id = (image_id + 1) % n_images; 
    }

    while (num_results < (n_parallaldos * n_images)) {
        process_id = PI_Select(g_results_bundle);
        PI_Read(g_results[process_id], "%d%d%d%d%d", &recv_parallaldo_id, &recv_image_id, &y, &x, &r);
        printf(PRINT_FORMAT, PRINT_PREFIX,
               g_parallaldo_files[recv_parallaldo_id],
               g_image_files[recv_image_id],
               y, x, r);
        num_results++;

        if (num_sent < (n_parallaldos * n_images)) {
            PI_Write(g_instructions[process_id], "%d%d%d", (int)WORKER_TASK, parallaldo_id, image_id);
            parallaldo_id += (image_id + 1) / n_images;
            image_id = (image_id + 1) % n_images; 
            num_sent++;
        }
    }
}

/*
 * main
 *  usage: wp [-b] parallaldodir imagedir
 *  Searches for a set of Parallaldos in a set of Images.
 *  Prints the location and orientation of any Parallaldos found within any Images.
 *  Capable of running search serially on a single process, or in parallel across many.
 *
 *  -b: flag specifying search should be run using load balancing
 *  parallaldodir: the directory to load Parallaldo files from
 *  imagedir: the directory to load Image files from
 */
// TODO: Don't free processes if running in serial.
int main(int argc, char *argv[])
{
    PI_PROCESS **processes;
    int n_procs, n_parallaldos, n_images;
    char *parallaldo_dir, *image_dir;
    boolean load_balancer;

    // Initialize Pilot environment.
    n_procs = PI_Configure(&argc, &argv);

    // Parse command line arguments.
    if (argc == 4 && strcmp(argv[1], "-b") == 0) {
        load_balancer = TRUE;
        parallaldo_dir = argv[2];
        image_dir = argv[3];
    } else if (argc == 3) {
        load_balancer = FALSE;
        parallaldo_dir = argv[1];
        image_dir = argv[2];
    } else {
        fprintf(stderr, "usage: wp [-b] parallaldodir imagedir\n");
        return 1;
    }

    // Get filenames from directories.
    n_parallaldos = list_filenames(parallaldo_dir, &g_parallaldo_files);
    n_images = list_filenames(image_dir, &g_image_files);

    // Initialize processes and channels.
    if (n_procs > 1) {
        processes = malloc((n_procs - 1) * sizeof(PI_PROCESS *));
        if (processes == NULL) {
            fprintf(stderr, "error: main: can't initialize processes, out of memory.\n");
            exit(EXIT_FAILURE);
        }

        g_instructions = malloc((n_procs - 1) * sizeof(PI_CHANNEL *));
        if (g_instructions == NULL) {
            fprintf(stderr, "error: main: can't initialize g_instructions, out of memory.\n");
            exit(EXIT_FAILURE);
        }

        g_results = malloc((n_procs - 1) * sizeof(PI_CHANNEL *));
        if (g_results == NULL) {
            fprintf(stderr, "error: main: can't initialize g_results, out of memory.\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n_procs - 1; i++) {
            processes[i] = PI_CreateProcess(worker, i, NULL);
            g_instructions[i] = PI_CreateChannel(PI_MAIN, processes[i]);
            g_results[i] = PI_CreateChannel(processes[i], PI_MAIN);
        }
        g_results_bundle = PI_CreateBundle(PI_SELECT, g_results, (n_procs - 1));
    }

    // Begin processing.
    PI_StartAll();

    // Select search method from serial, load_balanced, or round_robin.
    if (n_procs == 1) {
        serial(n_parallaldos, n_images);
    } else if (load_balancer) {
        load_balanced(n_procs - 1, n_parallaldos, n_images);
    } else {
        round_robin(n_procs - 1, n_parallaldos, n_images);
    }

    // Instruct worker processes (if any) to stop.
    for (int i = 0; i < n_procs - 1; i++) {
        PI_Write(g_instructions[i], "%d%d%d", (int)WORKER_QUIT, -1, -1);
    }

    // End processing.
    PI_StopMain(0);

    // Free all dynamically allocated memory.
    free_filenames(n_parallaldos, g_parallaldo_files);
    free_filenames(n_images, g_image_files);
    free(processes);
    free(g_instructions);
    free(g_results);
    return 0;
}
