#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_NAME_LENGTH 50
#define MAX_DEPARTMENT_LENGTH 50

typedef struct {
    int32_t id;
    uint8_t name_length;
    char name[MAX_NAME_LENGTH];
    uint8_t department_length;
    char department[MAX_DEPARTMENT_LENGTH];
    uint8_t age;
} Record;

void read_binary_and_print(const char *binary_filename) {
    FILE *binary_file = fopen(binary_filename, "rb");

    if (!binary_file) {
        perror("File open failed");
        exit(EXIT_FAILURE);
    }

    Record record;

    printf("Id,Name,Department,Age\n");

    while (fread(&record.id, sizeof(record.id), 1, binary_file) == 1) {
        record.id = __builtin_bswap32(record.id);

        fread(&record.name_length, sizeof(record.name_length), 1, binary_file);
        fread(record.name, sizeof(char), record.name_length, binary_file);
        record.name[record.name_length] = '\0';

        fread(&record.department_length, sizeof(record.department_length), 1, binary_file);
        fread(record.department, sizeof(char), record.department_length, binary_file);
        record.department[record.department_length] = '\0';

        fread(&record.age, sizeof(record.age), 1, binary_file);

        printf("%d,%s,%s,%d\n", record.id, record.name, record.department, record.age);
    }

    fclose(binary_file);
}

int main() {
    read_binary_and_print("data.bin");
    return 0;
}
