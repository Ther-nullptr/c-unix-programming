#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


void csv_to_binary(const char *csv_filename, const char *binary_filename) {
    FILE *csv_file = fopen(csv_filename, "r");
    FILE *binary_file = fopen(binary_filename, "wb");

    if (!csv_file || !binary_file) {
        perror("文件打开失败");
        exit(EXIT_FAILURE);
    }

    char line[256];
    fgets(line, sizeof(line), csv_file);

    while (fgets(line, sizeof(line), csv_file)) {
        Record record;
        char name[MAX_NAME_LENGTH], department[MAX_DEPARTMENT_LENGTH];
        
        sscanf(line, "%d,%49[^,],%49[^,],%hhd", &record.id, name, department, &record.age);
        
        record.name_length = strlen(name);
        strncpy(record.name, name, MAX_NAME_LENGTH);
        record.name[record.name_length] = '\0';
        record.department_length = strlen(department);
        strncpy(record.department, department, MAX_DEPARTMENT_LENGTH);
        record.department[record.department_length] = '\0';

        fwrite(&record.id, sizeof(record.id), 1, binary_file);
        fwrite(&record.name_length, sizeof(record.name_length), 1, binary_file);
        fwrite(record.name, sizeof(char), record.name_length, binary_file);
        fwrite(&record.department_length, sizeof(record.department_length), 1, binary_file);
        fwrite(record.department, sizeof(char), record.department_length, binary_file);
        fwrite(&record.age, sizeof(record.age), 1, binary_file);
    }

    fclose(csv_file);
    fclose(binary_file);
}

int main() {
    csv_to_binary("data.csv", "data.bin");
    return 0;
}
