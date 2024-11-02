#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdint.h>

#define MAX_RECORDS_PER_FILE 10
#define MAX_NAME_LENGTH 20
#define MAX_DEPARTMENT_LENGTH 10

typedef struct {
    int32_t id;
    uint8_t name_length;
    char name[MAX_NAME_LENGTH];
    uint8_t department_length;
    char department[MAX_DEPARTMENT_LENGTH];
    uint8_t age;
} Record;

void createSmallFiles(const char *csvFile) {
    FILE *in = fopen(csvFile, "r");
    if (!in) {
        perror("Failed to open CSV file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    int fileIndex = 0;
    int recordCount = 0;

    FILE *out = NULL;

    while (fgets(line, sizeof(line), in)) {
        if (recordCount == 0) {
            char smallFileName[32];
            snprintf(smallFileName, sizeof(smallFileName), "temp%d.csv", fileIndex);
            out = fopen(smallFileName, "w");
            fprintf(out, "%s", line); // Write header
        } else {
            fprintf(out, "%s", line); // Write data line
        }

        recordCount++;
        if (recordCount >= MAX_RECORDS_PER_FILE) {
            fclose(out);
            recordCount = 0;
            fileIndex++;
        }
    }

    if (out) {
        fclose(out);
    }

    fclose(in);
}

void convertToBinary(const char *smallFile) {
    FILE *in = fopen(smallFile, "r");
    if (!in) {
        perror("Failed to open small CSV file");
        exit(EXIT_FAILURE);
    }

    char outputFileName[32];
    snprintf(outputFileName, sizeof(outputFileName), "%s.bin", smallFile);
    FILE *out = fopen(outputFileName, "wb");

    char line[256];
    fgets(line, sizeof(line), in); // Read header, ignore

    while (fgets(line, sizeof(line), in)) {
        Record record;
        char name[MAX_NAME_LENGTH], department[MAX_DEPARTMENT_LENGTH];
        char format[20 + MAX_NAME_LENGTH + MAX_DEPARTMENT_LENGTH];
        snprintf(format, sizeof(format), "%%d,%%%d[^,],%%%d[^,],%%hhu", MAX_NAME_LENGTH, MAX_DEPARTMENT_LENGTH);

        sscanf(line, format, &record.id, name, department, &record.age);
        
        record.name_length = strlen(name);
        strncpy(record.name, name, MAX_NAME_LENGTH);
        record.name[record.name_length] = '\0';
        record.department_length = strlen(department);
        strncpy(record.department, department, MAX_DEPARTMENT_LENGTH);
        record.department[record.department_length] = '\0';

        // Convert to Big Endian
        record.id = __builtin_bswap32(record.id);

        fwrite(&record.id, sizeof(record.id), 1, out);
        fwrite(&record.name_length, sizeof(record.name_length), 1, out);
        fwrite(record.name, sizeof(char), record.name_length, out);
        fwrite(&record.department_length, sizeof(record.department_length), 1, out);
        fwrite(record.department, sizeof(char), record.department_length, out);
        fwrite(&record.age, sizeof(record.age), 1, out);
    }

    fclose(in);
    fclose(out);
}

int main() {
    const char *csvFile = "data.csv";
    createSmallFiles(csvFile);

    pid_t pids[MAX_RECORDS_PER_FILE];
    int fileIndex = 0;

    while (1) {
        char smallFileName[32];
        snprintf(smallFileName, sizeof(smallFileName), "temp%d.csv", fileIndex);
        if (access(smallFileName, F_OK) == -1) break; // File does not exist

        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            convertToBinary(smallFileName);
            exit(0);
        } else if (pid > 0) {
            // Parent process
            pids[fileIndex] = pid;
            fileIndex++;
        } else {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all child processes to complete
    for (int i = 0; i < fileIndex; i++) {
        waitpid(pids[i], NULL, 0);
    }

    // Merge binary files
    FILE *out = fopen("data.bin", "wb");
    for (int i = 0; i < fileIndex; i++) {
        char binFileName[32];
        snprintf(binFileName, sizeof(binFileName), "temp%d.csv.bin", i);
        FILE *in = fopen(binFileName, "rb");
        if (in) {
            fseek(in, 0, SEEK_END);
            long size = ftell(in);
            fseek(in, 0, SEEK_SET);
            char *buffer = malloc(size);
            fread(buffer, 1, size, in);
            fwrite(buffer, 1, size, out);
            free(buffer);
            fclose(in);
        }
    }

    fclose(out);

    // delete temp files
    for (int i = 0; i < fileIndex; i++) {
        char smallFileName[32];
        snprintf(smallFileName, sizeof(smallFileName), "temp%d.csv", i);
        remove(smallFileName);

        char binFileName[32];
        snprintf(binFileName, sizeof(binFileName), "temp%d.csv.bin", i);
        remove(binFileName);
    }

    return 0;
}
