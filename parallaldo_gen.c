/*
 * ------------------------------------------------------------------
 *  parallaldo_gen.c - parallaldo generator
 * ------------------------------------------------------------------
 * [1-Oct-10] calloc bugs fixed thx. to Jason Cote
 */

#include <stdlib.h>
#include <stdio.h>

/*
 * MINSIZE - minimum x/y dimension of generated image
 * MAXSIZE - maximum x/y dimension of generated image
 * PROBCHG - probability of flipping current sequence of 1/0
 *           (30 makes decent images - 0 will end up displaying only the
 *            parallaldo, 100 will be all 1s)
 * BUFSIZE - input buffer for reading first line with dimensions
 *
 * caution: if MAXSIZE - MINSIZE is smaller than one of the dimensions of
 *          the parallaldo you can get floating point exceptions from
 *          the modulus operations --- basically, the image is out of
 *          bounds and this is a quick and dirty so I'm not doing a lot
 *          of V&V.
 */
#define MINSIZE   1000
#define MAXSIZE   5000
#define PROBCHG   30
#define BUFSIZE   129

int main(int argc, char *argv[])
{
    int i,j;
    int parx, pary, targetx, targety;
    int range, rotation, insx, insy;
    int last, rotval;
    int **par;
    int **target;
    FILE *fpin, *fpout;
    char inbuf[BUFSIZE];

    if (argc != 3)
    {
        fprintf(stderr,"usage: parallaldo_gen <parfile> <outfile>\n");
        exit(1);
    }
    
    srandom(time(NULL));

    /*
     * load parallaldo
     */
    if (!(fpin = fopen(argv[1],"r")))
    {
        fprintf(stderr,"! unable to open file: %s\n",argv[1]);
        exit(2);
    }
    if (!fgets(inbuf,BUFSIZE,fpin))
    {
        fprintf(stderr,"! error reading from file: %s\n",argv[1]);
        exit(2);
    }
    if (sscanf(inbuf,"%d %d",&pary,&parx) != 2)
    {
        fprintf(stderr,"! error reading size from file: %s\n",argv[1]);
        exit(2);
    }
    if (!(par = calloc(pary,sizeof(int*))))
    {
        fprintf(stderr,"! memory allocation failure (fatal)\n");
        exit(3);
    }
    for (i = 0; i < pary; i++)
    {
        if (!(par[i] = calloc(parx,sizeof(int))))
        {
            fprintf(stderr,"! memory allocation failure (fatal)\n");
            exit(3);
        }
    }
    for (i = 0; i < pary; i++)
    {
        for (j = 0; j < parx; j++)
        {
            if (fscanf(fpin,"%1d",&par[i][j]) != 1)
            {
                fprintf(stderr,"! error reading data from file: %s\n",argv[1]);
                exit(2);
            }
        }
    }

    /*
     * random size target image
     */
    range = MAXSIZE - MINSIZE;
    targety = (random() % range) + MINSIZE;
    targetx = (random() % range) + MINSIZE;
    
    if (!(target = calloc(targety,sizeof(int*))))
    {
        fprintf(stderr,"! memory allocation failure (fatal)\n");
        exit(3);
    }
    for (i = 0; i < targety; i++)
    {
        if (!(target[i] = calloc(targetx,sizeof(int))))
        {
            fprintf(stderr,"! memory allocation failure (fatal)\n");
            exit(3);
        }
    }

    /* randomize target image (note: calloc initializes to 0) */
    last = 0;
    for (i = 0; i < targety; i++)
        for (j = 0; j < targetx; j++)
        {
            if ((random() % 100) < PROBCHG)
                last = (last == 1) ? 0 : 1;
            target[i][j] = last;
        }


    /* insert parallaldo into target image */
    rotation = random() % 4;
    switch(rotation)
    {
        case 0: /* 0 degrees */
            insy = random() % (targety - pary);
            insx = random() % (targetx - parx);
            for (i = 0; i < pary; i++)
                for (j = 0; j < parx; j++)
                    target[insy+i][insx+j] = par[i][j];
            rotval = 0;
            break;
        case 1: /* 90 degrees */
            insy = random() % (targety - parx);
            insx = (random() % (targetx - pary))+(pary-1);
            for (i = 0; i < pary; i++)
                for (j = 0; j < parx; j++)
                    target[insy+j][insx-i] = par[i][j];
            rotval = 90;
            break;
        case 2: /* 180 degrees - calc normally, insert upside down */
            insx = random() % (targetx - parx);
            insy = random() % (targety - pary);
            for (i = 0; i < pary; i++)
                for (j = 0; j < parx; j++)
                    target[insy+i][insx+j] = par[pary-i-1][parx-j-1];
            /* fix insx/insy */
            insx += (parx - 1);
            insy += (pary - 1);
            rotval = 180;
            break;
        case 3: /* 280 degrees - calc as 90, insert upside down */
            insy = random() % (targety - parx);
            insx = (random() % (targetx - pary))+(pary-1);
            for (i = 0; i < pary; i++)
                for (j = 0; j < parx; j++)
                    target[insy+j][insx-i] = par[pary-i-1][parx-j-1];
            rotval = 270;
            /* fix insx/insy */
            insx -= (pary - 1);
            insy += (parx - 1);
            break;
    }

    /*
     * open output file
     */
    if (!(fpout = fopen(argv[2],"w")))
    {
        fprintf(stderr,"! unable to open output file: %s\n",argv[2]);
        exit(2);
    }

    /*
     * printf("-> PAR <-\n");
     * for (i = 0; i < pary; i++)
     * {
     *  for (j = 0; j < parx; j++)
     *  printf("%d",par[i][j]);
     *  printf("\n");
     * }
     * printf("\n\n");
     */

    fprintf(fpout,"%d %d\n",targety,targetx);
    for (i = 0; i < targety; i++)
    {
        for (j = 0; j < targetx; j++)
            fprintf(fpout,"%d",target[i][j]);
        fprintf(fpout,"\n");
    }

    fprintf(stdout,"%s %s (%d,%d,%d)\n",argv[1],argv[2],insy+1,insx+1,rotval);

    return(0);
}
