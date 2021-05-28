#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "common.h"

#define MAXK_boyut 1000

void* runner(void* args);
int* findtopk(int* arr, int k, int N, char* read_dosya);

struct Args {
    int k;
    int N;
    int id;
    char* read_dosya;
    int* arr;
};

int main(int argc, char* argv[]) {
    int k = atoi(argv[1]);
    int N = atoi(argv[2]);
    int i,j;

    if (argc != N+4) {
        printf("Yanlış parametre.\nKullanım: findtopk <k> <N> <infile1>...<infileN> <outfile>\n");
        exit(1);
    }
    printf("k: %d, N: %d\n", k, N);

    char* out_dosya = argv[N+3];
    printf("out_dosya: %s\n", out_dosya);

    int* arrays[MAX_N_boyut];
    for (i=0;i<N;++i) {
        arrays[i] = malloc(sizeof(int) * k);
        for (j=0;j<k;++j)
            arrays[i][j] = 0;
    }
    pthread_t tids[MAX_N_boyut];
    pthread_attr_t attr[MAX_N_boyut];
    for (i=0;i<N;++i)
        pthread_attr_init(&attr[i]);

    for(i=0;i<N;++i) {
        char* read_dosya = argv[i + 3];

        struct Args* args = malloc(sizeof(struct Args));
        args->k = k;
        args->N = N;
        args->id = i;
        args->read_dosya = read_dosya;
        args->arr = arrays[i];

        pthread_create(&tids[i], &attr[i], runner, args);
    }

    void* retval;
    for (i=0;i<N;++i) {
        pthread_join(tids[i], &retval);
        arrays[i] = (int*) retval;
    }

    int* outArray = malloc(sizeof(int) * k * N);
    for (i=0;i<k*N;++i)
        outArray[i] = 0;
    merge(arrays, N, k, outArray);

    int out_fd = open(out_dosya, O_CREAT | O_RDWR | O_TRUNC, 0644);
    for (i=0;i<k*N;++i) {
        char buf[20];
        sprintf(buf, "%d\n", outArray[i]);
        write(out_fd, buf, strlen(buf));
    }
    close(out_fd);
    // printf("Basarili\n");
}

void* runner(void* a) {
    struct Args* args = (struct Args*) a;


    void* maxk = findtopk(args->arr, args->k, args->N, args->read_dosya);

    pthread_exit(maxk);
    return maxk;
}

int* findtopk(int* maxk, int k, int N, char* read_dosya) {
    // printf("maxk %s:\n", read_dosya);
    // int* maxk = malloc(sizeof(int) * k);
    int i;
    int read_fd = open(read_dosya, O_RDONLY);
    for (i=0;i<k;++i)
        maxk[i] = 0;
    // printf("\n");
    findmaxk(read_fd, maxk, k);

    close(read_fd);
    return maxk;
}
