#include <pilot.h>
#include <stdlib.h>

#include "filenames.h"

PI_CHANNEL **instructions;
PI_CHANNEL **results;

int worker(int id, void *p) {
    int parallaldoIndex, imageIndex;
    PI_Read(instructions[id], "%d%d", &parallaldoIndex, &imageIndex);
    // Run parallaldo searching algorithm.
    PI_Write(results[id], "%d%d%d", -1, -1, -1);
    return 0;
}

int main(int argc, char *argv[]) {
    int i, nProcs, nParallaldos, nImages;
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
    // Run algorithm.
    // Print results.
    PI_StopMain(0);

    freeFilenames(nParallaldos, parallaldos);
    freeFilenames(nImages, images);
    free(processes);
    free(instructions);
    free(results);
    return 0;
}
