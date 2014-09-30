#include <pilot.h>
#include <stdlib.h>

#include "filenames.h"

PI_CHANNEL **instructions;
PI_CHANNEL **results;

int worker(int id, void *p) {
    int parallaldoIndex, imageIndex;
    // Repeatedly read data from PI_MAIN.
    // Quit when <0 values read.
    while (1) {
        PI_Read(instructions[id], "%d%d", &parallaldoIndex, &imageIndex);
        if (parallaldoIndex < 0 || imageIndex < 0) {
            break;
        } else {
            // Run parallaldo searching algorithm.
            PI_Write(results[id], "%d%d%d", -1, -1, -1);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int i, j;
    int nProcs, nParallaldos, nImages;
    int assignedProcess, x, y, r;
    char **parallaldos, **images;
    PI_PROCESS **processes;

    // Initialize Pilot environment.
    nProcs = PI_Configure(&argc, &argv);

    // Get filenames from directories.
    nParallaldos = listFilenames(argv[1], &parallaldos);
    nImages = listFilenames(argv[2], &images);

    // Initialize processes and channels.
    if (nProcs > 1) {
        processes = malloc((nProcs - 1) * sizeof(PI_PROCESS *));
        instructions = malloc((nProcs - 1) * sizeof(PI_CHANNEL *));
        results = malloc((nProcs - 1) * sizeof(PI_CHANNEL *));
        for (i = 0; i < nProcs - 1; i++) {
            processes[i] = PI_CreateProcess(worker, i, NULL);
            instructions[i] = PI_CreateChannel(PI_MAIN, processes[i]);
            results[i] = PI_CreateChannel(processes[i], PI_MAIN);
        }
    }

    PI_StartAll();
    
    // Analyze parallaldos and image files.
    assignedProcess = 0;
    for (i = 0; i < nParallaldos; i++) {
        for (j = 0; j < nImages; j++) {
            if (nProcs == 1) {
                // Run parallaldo searching algorithm.
                // Print out result.
            } else {
                assignedProcess = (assignedProcess + 1) % (nProcs - 1);
                PI_Write(instructions[assignedProcess], "%d%d", i, j);
                PI_Read(results[assignedProcess], "%d%d%d", &y, &x, &r);
                // Print out result.
            }
        }
    }

    // Send signal to workers to stop.
    for (i = 0; i < nProcs - 1; i++) {
        PI_Write(instructions[i], "%d%d", -1, -1);
    }

    PI_StopMain(0);

    freeFilenames(nParallaldos, parallaldos);
    freeFilenames(nImages, images);
    free(processes);
    free(instructions);
    free(results);
    return 0;
}
