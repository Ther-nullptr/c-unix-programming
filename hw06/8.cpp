#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <cstdint>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <algorithm>

#define MAX_NAME_LENGTH 20
#define MAX_DEPARTMENT_LENGTH 10
#define MAX_LINE_LENGTH 256
#define MAX_NUM_CHINDREN 2 


struct Record {
    uint8_t idx;
    int32_t id;
    uint8_t name_length;
    char name[MAX_NAME_LENGTH];
    uint8_t department_length;
    char department[MAX_DEPARTMENT_LENGTH];
    uint8_t age;
};


struct Task {
    std::string line;
    int index;
};


void csv2bin(const std::string& csv_file) {
    std::ifstream input(csv_file);
    if (!input) {
        std::cerr << "Failed to open CSV file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<Task> tasks;
    std::string line;
    int index = 0;
    std::getline(input, line);  // jump the first line

    while (std::getline(input, line)) {
        tasks.push_back(Task{line, index++});
    }

    size_t totalLines = tasks.size();
    size_t chunkSize = totalLines / MAX_NUM_CHINDREN;

    // create pipes
    int pipes[MAX_NUM_CHINDREN][2];  // read_pipe, write_pipe
    for (int i = 0; i < MAX_NUM_CHINDREN; ++i) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    // create child processes
    for (int i = 0; i < MAX_NUM_CHINDREN; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {  // child process
            close(pipes[i][0]);  // close read pipe

            size_t startIdx = i * chunkSize;
            size_t endIdx = (i == MAX_NUM_CHINDREN - 1) ? totalLines : (i + 1) * chunkSize;

            for (size_t j = startIdx; j < endIdx; ++j) {
                const Task& task = tasks[j];
                Record record;

                record.idx = task.index;

                std::istringstream ss(task.line);
                std::string name, department;
                int id;
                ss >> id;
                record.id = static_cast<int32_t>(id);
                ss.ignore(1, ',');  // Skip the comma
                std::getline(ss, name, ',');
                std::getline(ss, department, ',');

                int age;
                ss >> age;
                record.age = static_cast<uint8_t>(age);

                // convert to network byte order
                record.id = htonl(record.id);
                record.name_length = name.length() + 1;
                record.department_length = department.length() + 1;
                
                // write to pipe seperately
                write(pipes[i][1], &record.idx, sizeof(uint8_t));
                write(pipes[i][1], &record.id, sizeof(int32_t));
                write(pipes[i][1], &record.name_length, sizeof(uint8_t));
                write(pipes[i][1], name.c_str(), name.length()+1);
                write(pipes[i][1], &record.department_length, sizeof(uint8_t));
                write(pipes[i][1], department.c_str(), department.length()+1);
                write(pipes[i][1], &record.age, sizeof(uint8_t));

                // if (write(pipes[i][1], &record, sizeof(Record)) == -1) {
                //     perror("write to pipe failed");
                //     exit(EXIT_FAILURE);
                // }
            }
            close(pipes[i][1]);  // close write pipe
            exit(0);
        }
    }

    // parent process
    // close write pipes
    for (int i = 0; i < MAX_NUM_CHINDREN; ++i) {
        close(pipes[i][1]);
    }

    // read from pipes
    std::vector<Record> records;
    size_t recordsReceived = 0;

    // read records from pipes
    while (recordsReceived < totalLines) {
        for (int i = 0; i < MAX_NUM_CHINDREN; ++i) {
            Record record;
            // ssize_t bytesRead = read(pipes[i][0], &record, sizeof(Record));
            // read from pipe seperately
            ssize_t bytesRead = read(pipes[i][0], &record.idx, sizeof(uint8_t));
            bytesRead += read(pipes[i][0], &record.id, sizeof(int32_t));
            bytesRead += read(pipes[i][0], &record.name_length, sizeof(uint8_t));
            bytesRead += read(pipes[i][0], record.name, record.name_length);
            bytesRead += read(pipes[i][0], &record.department_length, sizeof(uint8_t));
            bytesRead += read(pipes[i][0], record.department, record.department_length);
            bytesRead += read(pipes[i][0], &record.age, sizeof(uint8_t));

            if (bytesRead > 0) {
                records.push_back(record);
                // sort records by index
                std::sort(records.begin(), records.end(), [](const Record& a, const Record& b) {
                    return a.idx < b.idx;
                });
                recordsReceived++;
            }
        }
    }

    // close read pipes
    for (int i = 0; i < MAX_NUM_CHINDREN; ++i) {
        wait(NULL);
    }

    std::cout << "CSV to binary conversion completed." << std::endl;

    // write to binary file
    std::ofstream binaryOutput("output.bin", std::ios::binary);
    if (!binaryOutput) {
        std::cerr << "Failed to open output file" << std::endl;
        exit(EXIT_FAILURE);
    }

    // write records to binary file
    for (const auto& record : records) {
        binaryOutput.write(reinterpret_cast<const char*>(&record), sizeof(Record));
    }
}

// use mmap to read binary file
void bincat(const std::string& binaryFile) {
    int fd = open(binaryFile.c_str(), O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat failed");
        exit(EXIT_FAILURE);
    }

    size_t fileSize = sb.st_size;
    Record* records = (Record*)mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (records == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    size_t numRecords = fileSize / sizeof(Record);
    for (size_t i = 0; i < numRecords; ++i) {
        Record record = records[i];
        record.id = ntohl(record.id);
        std::cout << "ID: " << record.id << ", ";
        std::cout << "Name: " << record.name << ", ";
        std::cout << "Department: " << record.department << ", ";
        std::cout << "Age: " << (int)record.age << std::endl;
    }

    munmap(records, fileSize);
    close(fd);
}

int main() {
    csv2bin("data.csv");

    std::cout << "Contents of the binary file:" << std::endl;
    bincat("output.bin");

    return 0;
}
