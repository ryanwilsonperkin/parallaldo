#include <pilot.h>
#include <stdio.h>
#include <stdlib.h>

#include "filenames.h"
#include "parallaldo.h"
#include "wp.h"

#define PRINT_FORMAT "%c%s %s (%d,%d,%d)"
#define PRINT_PREFIX '$'

PI_CHANNEL **g_instructions;
PI_CHANNEL **g_results;
char **g_parallaldos, **g_images;

int worker(int id, void *p)
{
    enum WorkerInstruction instruction;
    int parallaldo_id, image_id;
    while (1) {
        PI_Read(g_instructions[id], "%d%d%d", (int *)&instruction, &parallaldo_id, &image_id);
        switch (instruction) {
            case WORKER_TASK: {
                Parallaldo parallaldo = load_parallaldo(g_parallaldos[parallaldo_id]);
                Image image = load_image(g_images[image_id]);
                Position p = find_parallaldo(parallaldo, image);

                PI_Write(g_results[id], "%d%d%d", p.y, p.x, p.r);

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
    int i, j;
    int n_procs, n_parallaldos, n_images;
    int assigned_process, x, y, r;
    PI_PROCESS **processes;

    // Initialize Pilot environment.
    n_procs = PI_Configure(&argc, &argv);

    // Get filenames from directories.
    n_parallaldos = listFilenames(argv[1], &g_parallaldos);
    n_images = listFilenames(argv[2], &g_images);

    // Initialize processes and channels.
    if (n_procs > 1) {
        processes = malloc((n_procs - 1) * sizeof(PI_PROCESS *));
        g_instructions = malloc((n_procs - 1) * sizeof(PI_CHANNEL *));
        g_results = malloc((n_procs - 1) * sizeof(PI_CHANNEL *));
        for (i = 0; i < n_procs - 1; i++) {
            processes[i] = PI_CreateProcess(worker, i, NULL);
            g_instructions[i] = PI_CreateChannel(PI_MAIN, processes[i]);
            g_results[i] = PI_CreateChannel(processes[i], PI_MAIN);
        }
    }

    PI_StartAll();
    
    // Analyze parallaldos and image files.
    assigned_process = 0;
    for (i = 0; i < n_parallaldos; i++) {
        for (j = 0; j < n_images; j++) {
            if (n_procs == 1) {
                Parallaldo parallaldo = load_parallaldo(g_parallaldos[i]);
                Image image = load_image(g_images[j]);
                Position p = find_parallaldo(parallaldo, image);
                y = p.y;
                x = p.x;
                r = p.r;
            } else {
                assigned_process = (assigned_process + 1) % (n_procs - 1);
                PI_Write(g_instructions[assigned_process], "%d%d%d", (int)WORKER_TASK, i, j);
                PI_Read(g_results[assigned_process], "%d%d%d", &y, &x, &r);
            }
            if (y >= 0 && x >= 0 && r >= 0) {
                printf(PRINT_FORMAT, PRINT_PREFIX, g_parallaldos[i], g_images[j], y, x, r);
            }
        }
    }

    // Send signal to workers to stop.
    for (i = 0; i < n_procs - 1; i++) {
        PI_Write(g_instructions[i], "%d%d%d", (int)WORKER_QUIT, -1, -1);
    }

    PI_StopMain(0);

    freeFilenames(n_parallaldos, g_parallaldos);
    freeFilenames(n_images, g_images);
    free(processes);
    free(g_instructions);
    free(g_results);
    return 0;
}
