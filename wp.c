#include <pilot.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "filenames.h"
#include "parallaldo.h"
#include "wp.h"

#define PRINT_FORMAT "%c%s %s (%d,%d,%d)\n"
#define PRINT_PREFIX '$'

PI_CHANNEL **g_instructions;
PI_CHANNEL **g_results;
char **g_parallaldo_files, **g_image_files;

int worker(int id, void *p)
{
    enum WorkerInstruction instruction;
    Parallaldo parallaldo;
    Image image;
    Position position;
    int parallaldo_id, image_id;

    // Repeatedly read and execute parallaldo searches until instructed to exit by PI_MAIN.
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

void round_robin(int n_procs, int n_parallaldos, int n_images)
{
    int recv_parallaldo_id, recv_image_id;
    int current_parallaldo = 0, current_image = 0;
    int x, y, r, process_id = 0;

    for (int i = 0; i < n_parallaldos; i++) {
        for (int j = 0; j < n_images; j++) {
            process_id = (process_id + 1) % (n_procs - 1);
            PI_Write(g_instructions[process_id], "%d%d%d", (int)WORKER_TASK, i, j);
            PI_Read(g_results[process_id], "%d%d%d%d%d", &recv_parallaldo_id, &recv_image_id, &y, &x, &r);
            printf(PRINT_FORMAT, PRINT_PREFIX,
                   g_parallaldo_files[recv_parallaldo_id],
                   g_image_files[recv_image_id],
                   y, x, r);
        }
    }
}

void load_balanced(int n_procs, int n_parallaldos, int n_images)
{
}

int main(int argc, char *argv[])
{
    // TODO: Fix round robin function to properly assign tasks.
    // TODO: Implement load_balanced functionality.
    // TODO: Implement find_parallaldo algorithm.
    // TODO: Properly document functions/headers.
    // TODO: Add license comment at top of each header file.
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
    n_parallaldos = listFilenames(parallaldo_dir, &g_parallaldo_files);
    n_images = listFilenames(image_dir, &g_image_files);

    // Initialize processes and channels.
    if (n_procs > 1) {
        processes = malloc((n_procs - 1) * sizeof(PI_PROCESS *));
        g_instructions = malloc((n_procs - 1) * sizeof(PI_CHANNEL *));
        g_results = malloc((n_procs - 1) * sizeof(PI_CHANNEL *));
        for (int i = 0; i < n_procs - 1; i++) {
            processes[i] = PI_CreateProcess(worker, i, NULL);
            g_instructions[i] = PI_CreateChannel(PI_MAIN, processes[i]);
            g_results[i] = PI_CreateChannel(processes[i], PI_MAIN);
        }
    }

    // Begin processing.
    PI_StartAll();

    if (n_procs == 1) {
        serial(n_parallaldos, n_images);
    } else if (load_balancer) {
        load_balanced(n_procs, n_parallaldos, n_images);
    } else {
        round_robin(n_procs, n_parallaldos, n_images);
    }

    // If running in parallel, instruct worker processes to stop.
    for (int i = 0; i < n_procs - 1; i++) {
        PI_Write(g_instructions[i], "%d%d%d", (int)WORKER_QUIT, -1, -1);
    }

    // End processing.
    PI_StopMain(0);

    // Free all dynamically allocated objects.
    freeFilenames(n_parallaldos, g_parallaldo_files);
    freeFilenames(n_images, g_image_files);
    free(processes);
    free(g_instructions);
    free(g_results);
    return 0;
}
