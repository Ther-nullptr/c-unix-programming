#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_NAME_LENGTH 50
#define MAX_DEPARTMENT_LENGTH 50

// 结构体定义
typedef struct {
    int32_t id;  // 4 字节，保持不变
    uint8_t name_length;  // 1 字节
    char name[MAX_NAME_LENGTH];  // 50 字节
    uint8_t department_length;  // 1 字节
    char department[MAX_DEPARTMENT_LENGTH];  // 50 字节
    uint8_t age;  // 使用 uint8_t 代替 int32_t，节省空间
} Record;

// 从二进制文件读取并打印数据
void read_binary_and_print(const char *binary_filename) {
    FILE *binary_file = fopen(binary_filename, "rb");

    if (!binary_file) {
        perror("文件打开失败");
        exit(EXIT_FAILURE);
    }

    Record record;

    while (fread(&record.id, sizeof(record.id), 1, binary_file) == 1) {
        fread(&record.name_length, sizeof(record.name_length), 1, binary_file);
        fread(record.name, sizeof(char), record.name_length, binary_file);
        record.name[record.name_length] = '\0';  // 确保字符串结束

        fread(&record.department_length, sizeof(record.department_length), 1, binary_file);
        fread(record.department, sizeof(char), record.department_length, binary_file);
        record.department[record.department_length] = '\0';  // 确保字符串结束

        fread(&record.age, sizeof(record.age), 1, binary_file);

        // 打印记录
        printf("Id: %d, Name: %s, Department: %s, Age: %d\n",
               record.id, record.name, record.department, record.age);
    }

    fclose(binary_file);
}

// 主函数
int main() {
    read_binary_and_print("data.bin");
    return 0;
}
