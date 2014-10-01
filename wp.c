#include <pilot.h>
#include <stdio.h>
#include <stdlib.h>

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

                PI_Write(g_results[id], "%d%d%d", position.y, position.x, position.r);

                free_parallaldo(parallaldo);
                free_image(image);
                break;
            }
            case WORKER_QUIT:
                return 0;
        } 
    }
}

int main(int argc, char *argv[])
{
    PI_PROCESS **processes;
    Parallaldo parallaldo;
    Image image;
    Position position;
    int n_procs, n_parallaldos, n_images;
    int assigned_process, x, y, r;

    // Initialize Pilot environment.
    n_procs = PI_Configure(&argc, &argv);

    // Get filenames from directories.
    n_parallaldos = listFilenames(argv[1], &g_parallaldo_files);
    n_images = listFilenames(argv[2], &g_image_files);

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

    // Search image files for parallaldos.
    assigned_process = 0;
    for (int i = 0; i < n_parallaldos; i++) {
        for (int j = 0; j < n_images; j++) {

            // If PI_MAIN is the only process, run the find algorithm directly.
            if (n_procs == 1) {
                parallaldo = load_parallaldo(g_parallaldo_files[i]);
                image = load_image(g_image_files[j]);
                position = find_parallaldo(parallaldo, image);
                y = position.y;
                x = position.x;
                r = position.r;

                free_parallaldo(parallaldo);
                free_image(image);

            // If running in parallel, instruct worker processes to run the algorithm.
            } else {
                assigned_process = (assigned_process + 1) % (n_procs - 1);
                PI_Write(g_instructions[assigned_process], "%d%d%d", (int)WORKER_TASK, i, j);
                PI_Read(g_results[assigned_process], "%d%d%d", &y, &x, &r);
            }

            // If y, x, and r are valid, print them according to PRINT_FORMAT specification.
            if (y >= 0 && x >= 0 && r >= 0) {
                printf(PRINT_FORMAT, PRINT_PREFIX, g_parallaldo_files[i], g_image_files[j], y, x, r);
            }
        }
    }

    // Send signal to worker processes (if any) to stop.
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
