#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <wait.h>
#include <unistd.h>
#include <string.h>

#define TEMP(n) { 't', 'm', 'p', (n) + '0', '\0' } 
#define MAX_N 5

#include "common.h"

void findtopk(int k, int N, char* read_dosya, char* out_dosya);

int main(int argc, char* argv[])
{
    int k = atoi(argv[1]);
    int N = atoi(argv[2]);
    int i, j;

    if (argc != N+4) {
        printf("Yanlış parametre.\nKullanım: findtopk <k> <N> <infile1>...<infileN> <outfile>\n");
        exit(1);
    }
    printf("k: %d, N: %d\n", k, N);

    char* out_dosya = argv[N+3];
    printf("out_dosya: %s\n", out_dosya);

    // N adet child-process olusturma
    for (i=0;i<N;++i) {
        char* read_dosya = argv[i + 3];
        char out_dosya[] = TEMP(i);
        
        findtopk(k, N, read_dosya, out_dosya);
    }

    // Tum child processler icin wait()
    for (i=0;i<N;++i)
        wait(0);

    // Her dosya icin alan ayrimi
    int** maxSayilar = malloc(sizeof(int*) * N);
    for (i=0;i<N;++i)
        maxSayilar[i] = malloc(sizeof(int) * k);
    // Out dosyasi
    int out_fd = open(out_dosya, O_CREAT | O_RDWR | O_TRUNC, 0644);

    for (i=0;i<N;++i) {
        char temp_isim[] = TEMP(i);
        
        int temp_fd = open(temp_isim, O_RDONLY);

        for (j=0;j<k;++j) {
            int current;
            // Dosyayi oku
            while (1) {
                current = readint(temp_fd);
                if (current > 0)
                    break;
            }
            
            maxSayilar[i][j] = current;
        }
        close(temp_fd);
        // Gereksiz dosyalarin temizlenmesi
        remove(temp_isim);
    }
    for (i=0;i<N;++i) {
    
        for (j=0;j<k;++j) {
        
        }
        // printf("\n");
    }
    //outArray icin alan ayrimi
    int* outArray = malloc(sizeof(int) * k * N);
    for (i=0;i<k*N;++i)
        outArray[i] = 0;
    merge(maxSayilar, N, k, outArray);

    for (i=0;i<k*N;++i) {
        char buf[20];
        sprintf(buf, "%d\n", outArray[i]);
        write(out_fd, buf, strlen(buf));
    }
    close(out_fd);
    printf("Başarılı\n");
}

void findtopk(int k, int N, char* read_dosya, char* out_dosya)
{
    int i;
    pid_t p = fork();
    
    if (p == 0) { //Child process olup olmama durumunu kontrol etmesi
    
        int read_fd = open(read_dosya, O_RDONLY);
        int write_dosya = open(out_dosya, O_CREAT | O_WRONLY, 0644);

        int* maxk = malloc(sizeof(int) * k);
        for (i=0;i<k;++i)
            maxk[i] = - 1;
            
        findmaxk(read_fd, maxk, k);
        
        for (i=0;i<k;++i) {
            char buffer[20];
            // Dosyaya yazdirma
            sprintf(buffer, "%d\n", maxk[i]);
            
            write(write_dosya, buffer, strlen(buffer));
            
        }
        //Dosyalarin kapatilmasi
        close(read_fd);
        close(write_dosya);

        exit(0);
    }
}
