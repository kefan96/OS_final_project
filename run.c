#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define SIZE 8192

unsigned int xorbuf(unsigned int *buffer, int size) {
    unsigned int result = 0;
    for (int i = 0; i < size; i++) {
        result ^= buffer[i];
    }
    return result;
}

int read_file(char* fname, int block_size) {
    int input_fd;
    int file_size = 0;
    unsigned int *buffer;
    ssize_t ret_in;
    int xor_result = 0;

    input_fd = open(fname, O_RDONLY);
    if (input_fd == -1) {
        perror("open file error");
        exit(EXIT_FAILURE);
    }
    
    buffer = (unsigned int*) malloc((size_t) block_size); 

    while((ret_in = read (input_fd, buffer, block_size)) > 0){
        file_size += ret_in;
        xor_result ^= xorbuf(buffer, ret_in);
    }

    printf("The result of the xor calculation: %x\n", xor_result);

    close(input_fd);

    return file_size;
}

int write_file(char* fname, int block_size, int block_count) {
    int output_fd;
    unsigned int *buffer;
    int i, j;
    ssize_t ret_out, file_size;

    output_fd = open(fname, O_WRONLY | O_CREAT, 0644);
    if (output_fd == -1) {
        perror("open file error");
        exit(EXIT_FAILURE);
    }

    buffer = (unsigned int*) malloc((size_t) block_size); 
    file_size = 0;

    srand(time(NULL));
    for (i=0; i<block_count; i++) {
        for (j=0; j<block_size/4; j++) {
            buffer[j] = (unsigned int) rand();
        }
        ret_out = write (output_fd, buffer, (ssize_t) block_size);
        if (ret_out != block_size) {
            perror("write error");
            exit(EXIT_FAILURE);
        }
        file_size += ret_out;
    }

    close(output_fd);

    return file_size;
}

int main(int argc, char* argv[]) {
    char* fname = argv[1];
    char* option = argv[2];
    unsigned int block_size = strtol(argv[3], NULL, 10);
    unsigned int block_count;
    unsigned int file_size;
    clock_t begin, end;
    double time_spent;
    
    if (strcmp(option, "-w") == 0) {
        block_count = strtol(argv[4], NULL, 10);
        begin = clock();
        file_size = write_file(fname, block_size, block_count);
        end = clock();
        time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
        printf("Done writing. The file size is: %d bytes, total time spent: %f seconds\n", file_size, time_spent);
    } else if (strcmp(option, "-r") == 0) {
        begin = clock();
        file_size = read_file(fname, block_size);
        end = clock();
        time_spent = (double) (end - begin) / CLOCKS_PER_SEC;
        printf("Done reading. The file size is: %d bytes, total time spent: %f seconds\n", file_size, time_spent);
    }

    return (EXIT_SUCCESS);
}