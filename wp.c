#include <pilot.h>

#include "filenames.h"

int main(int argc, char *argv[]) {
    int nProcs, nParallaldos, nImages;
    char **parallaldos, **images;

    nProcs = PI_Configure(&argc, &argv);
    nParallaldos = listFilenames(argv[1], &parallaldos);
    nImages = listFilenames(argv[2], &images);

    PI_StartAll();

    freeFilenames(nParallaldos, parallaldos);
    freeFilenames(nImages, images);
    PI_StopMain(0);
    return 0;
}
