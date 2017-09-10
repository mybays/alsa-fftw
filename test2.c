#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#define N 4800
int main(int argc, char *argv[])
{
    FILE *fptr;
    fptr=fopen(argv[1],"r");
    if(NULL == fptr)
    {
        printf("open error.\r\n");
        return -1;
    }
    fseek(fptr,0,SEEK_END);
    int nFileLen=ftell(fptr);
    char *buffer=(char*)malloc(nFileLen);
    fseek(fptr,0,SEEK_SET);
    fread(buffer,1,nFileLen,fptr);
    fclose(fptr);

    int is;
    for(is=0;is<100;is++)
    {
        //printf("%d:0x%x ",is,buffer[is]);
    }
    printf("\r\n");

    int16_t (*ip)[1024]=(void *)buffer+44;
    for(is=0;is<30;is++)
    {
        //printf("%d:0x%x ",is,(*ip)[is]);
    }
    printf("\r\n");

    //return 0;



    fftw_complex in[N], out[N], in2[N]; /* double [2] */
    fftw_plan p, q;
    int i;

    /* prepare a cosine wave */
    for (i = 0; i < N; i++)
    {
        //in[i][0] = cos(3 * 2*M_PI*i/N);
        in[i][0] = (double) (*ip)[i];
        in[i][1] = 0;
        //in[i] * = windowHanning(i,N/2);
        //printf("in: %3d %+9.5f %+9.5f I\n", i, in[i][0], in[i][1]);
    }
    printf("\n\n");
    /* forward Fourier transform, save the result in 'out' */
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);
    for (i = 0; i < N; i++)
    {
        //printf("freq: %3d %+9.5f %+9.5f I\n", i, out[i][0], out[i][1]);
    }
    fftw_destroy_plan(p);

    double v;
    for(i=0;i<N/2;i++)
    {
        v=sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]);
        printf("freq:%3d %+9.5f\n",i,v);
    }
    printf("\r\n");

    return 0;

    /* backward Fourier transform, save the result in 'in2' */
    printf("\nInverse transform:\n");
    q = fftw_plan_dft_1d(N, out, in2, FFTW_BACKWARD, FFTW_ESTIMATE);
    fftw_execute(q);
    /* normalize */
    for (i = 0; i < N; i++) {
        in2[i][0] *= 1./N;
        in2[i][1] *= 1./N;
    }
    for (i = 0; i < N; i++)
    //    printf("recover: %3d %+9.5f %+9.5f I vs. %+9.5f %+9.5f I\n",
    //           i, in[i][0], in[i][1], in2[i][0], in2[i][1]);
    fftw_destroy_plan(q);

    fftw_cleanup();
    return 0;
}

