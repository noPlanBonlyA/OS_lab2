#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#define BUFFER_SIZE (1024 * 1024)  

long get_time_microseconds() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}


void io_task(const char *filename, int size_mb) {
    char *buffer = (char *)malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Ошибка выделения памяти");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 'A', BUFFER_SIZE);


    int fd_out = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_out < 0) {
        perror("Ошибка открытия файла на запись");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    printf("Запись файла: %s размером %d MB...\n", filename, size_mb);
    long write_start = get_time_microseconds();
    for (int i = 0; i < size_mb; i++) {
        if (write(fd_out, buffer, BUFFER_SIZE) != BUFFER_SIZE) {
            perror("Ошибка записи в файл");
            close(fd_out);
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }
    fsync(fd_out);  
    close(fd_out);
    long write_end = get_time_microseconds();
    printf("Время записи: %.3f секунд\n", (write_end - write_start) / 1e6);

   
    int fd_in = open(filename, O_RDONLY);
    if (fd_in < 0) {
        perror("Ошибка открытия файла на чтение");
        free(buffer);
        exit(EXIT_FAILURE);
    }

    printf("Чтение файла: %s...\n", filename);
    long read_start = get_time_microseconds();
    while (read(fd_in, buffer, BUFFER_SIZE) > 0) {}
    close(fd_in);
    long read_end = get_time_microseconds();
    printf("Время чтения: %.3f секунд\n", (read_end - read_start) / 1e6);

    free(buffer);
    printf("IO Task завершена\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <имя файла> <размер файла в МБ>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    int size_mb = atoi(argv[2]);
    if (size_mb <= 0) {
        fprintf(stderr, "Ошибка: размер файла должен быть положительным числом.\n");
        return EXIT_FAILURE;
    }

    long total_start = get_time_microseconds();
    io_task(filename, size_mb);
    long total_end = get_time_microseconds();

    printf("Общее время выполнения: %.3f секунд\n", (total_end - total_start) / 1e6);

    return EXIT_SUCCESS;
}
