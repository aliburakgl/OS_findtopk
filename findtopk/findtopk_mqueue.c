#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <wait.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>

#include "common.h"

#define QUEUE_NAME "/mq_maxk"
#define MAXK_boyut 1000
#define ARRAY_boyut (sizeof(int) * MAXK_boyut)

struct MaxK {
    int child_id;
    int maxk[MAXK_boyut];
};

void findtopk(int k, int N, char* read_dosya, int id);
mqd_t mqOlustur();

int main(int argc, char* argv[])
{
    int k = atoi(argv[1]);
    int N = atoi(argv[2]);
    int i,j;

    if (argc != N + 4) {
        printf("Yanlış parametre.\nKullanım: findtopk <k> <N> <infile1>...<infileN> <outfile>\n");
        exit(1);
    }
    printf("k: %d, N: %d\n", k, N);

    char* out_dosya = argv[3 + N];
    printf("out_dosya: %s\n", out_dosya);

    // Msg queue açma
    // mqd_t mqd = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0600, 0);
    mqd_t mqd = mqOlustur();
    for (i=0;i<N;++i) {
        char* read_dosya = argv[i + 3];
        // printf("Read dosyasi: %s\n", read_dosya);

        findtopk(k, N, read_dosya, i);
    }

    for (i=0;i<N;++i)
        wait(0);

    // mq_close(mqd);

    int out_fd = open(out_dosya, O_CREAT | O_RDWR | O_TRUNC, 0644);
    unsigned int prio;

    int* arrays[MAX_N_boyut];
    for (i=0;i<N;++i) {
        arrays[i] = malloc(sizeof(int) * k);
        for (j=0;j<k;++j)
            arrays[i][j] = 0;
    }

    for (i=0;i<N;++i) {
    
        struct MaxK maxKs[2];
        if (mq_receive(mqd, (char*) &maxKs[0], sizeof(struct MaxK) * 2, &prio) != -1) {
            // printf("Mesaj alindi\n");
            char buff[20];
            for (j=0;j<k;++j) {
                // printf("Numara: %d\n", maxKs[0].maxk[j]);
                sprintf(buff, "%d\n", maxKs[0].maxk[j]);
                // write(out_fd, buff, strlen(buff));
                arrays[i][j] = maxKs[0].maxk[j];
            }
        } else {
            perror("mq_receive");
            exit(1);
        }   
    }

    int* arr = malloc(sizeof(int) * k * N);
    for (i=0;i<k*N;++i)
        arr[i] = 0;
    merge(arrays, N, k, arr);

    // int out_fd = open(out_dosya, O_CREAT | O_RDWR | O_TRUNC, 0644);
    for (i=0;i<k*N;++i) {
        char buf[20];
        sprintf(buf, "%d\n", arr[i]);
        write(out_fd, buf, strlen(buf));
    }
    close(out_fd);
    mq_close(mqd);
    mq_unlink(QUEUE_NAME);
}

void findtopk(int k, int N, char* read_dosya, int id)
{
    int i;
    pid_t p = fork();
    
    if (p == 0) { // Child process kontrolu
        // Dosyanin acilmasi
        int read_fd = open(read_dosya, O_RDONLY);
        mqd_t mqd = mq_open(QUEUE_NAME, O_WRONLY);
        if (mqd == -1) {
            perror("mq_open");
            exit(1);
        }

        struct MaxK maxk;
        maxk.child_id = id;
        for (i=0;i<k;++i)
            maxk.maxk[i] = -1;

        //  max k bulma
        
        findmaxk(read_fd, maxk.maxk, k);
        
        if(mq_send(mqd, (const char*) &maxk, sizeof(struct MaxK), 1) == -1) {
            perror("mq_send");
            exit(1);
        } else {
            // printf("Basarili\n");
        }
        
        // Dosyalarin kapatimasi
        close(read_fd);
        close(mqd);

        exit(0);
    }
}

mqd_t mqOlustur() {
    struct mq_attr attr;  
    attr.mq_flags = 0;  
    attr.mq_maxmsg = 10;  
    attr.mq_msgsize = sizeof(struct MaxK) + 1;  
    attr.mq_curmsgs = 0;  

    mqd_t queue = mq_open(QUEUE_NAME, O_CREAT|O_RDWR, 0644, &attr);
    if (queue == -1) {
        perror("mq_open");
        exit(1);
    }
    
    mq_getattr(queue, &attr);
    

    return queue;
}
