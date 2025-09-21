#include "proc_reader.h"

int list_process_directories(void) {
    DIR *dir = opendir("/proc");
if (dir == NULL) {
    perror("opendir(/proc)");
    return -1;
}

    struct dirent *entry;
int count = 0;

    printf("Listing processes from /proc directory:\n");
    printf("%-8s %-20s\n", "PID", "Type");
    printf("%-8s %-20s\n", "---", "----");

    while ((entry = readdir(dir)) != NULL) {
    if (is_number(entry->d_name)) {
        printf("%-8s %s\n", entry->d_name, "process");
        count++;
    }
}

    if (closedir(dir) == -1) {
    perror("closedir(/proc)");
    return -1;
}

    printf("Total processes detected: %d\n", count);

    return 0;
}

int read_process_info(const char* pid) {
    char filepath[256];

    snprintf(filepath, sizeof(filepath), "/proc/%s/status", pid);

    printf("\n[Process %s Details]\n", pid);

    if (read_file_with_syscalls(filepath) == -1) {
    fprintf(stderr, "\n[!] Failed to read %s\n", filepath);
    return -1;
}

    snprintf(filepath, sizeof(filepath), "/proc/%s/cmdline", pid);

    printf("\n--- Command Line ---\n");

    if (read_file_with_syscalls(filepath) == -1) {
    fprintf(stderr, "\n[!] Failed to read %s\n", filepath);
    return -1;
}

    printf("\n");

    return 0;
}

int show_system_info(void) {
    int line_count = 0;
    const int MAX_LINES = 10;

    printf("\nCPU info (showing top %d lines):\n", MAX_LINES);

    FILE *f_cpu = fopen("/proc/cpuinfo", "r");
if (!f_cpu) {
    perror("fopen(/proc/cpuinfo)");
    return -1;
}

    char line[1024];
    line_count = 0;
while (line_count < MAX_LINES && fgets(line, sizeof(line), f_cpu) != NULL) {
    fputs(line, stdout);
    line_count++;
}
    if (fclose(f_cpu) == EOF) {
    perror("fclose(/proc/cpuinfo)");
    return -1;
}

    printf("\n--- Memory Information (first %d lines) ---\n", MAX_LINES);

    FILE *f_mem = fopen("/proc/meminfo", "r");
if (!f_mem) {
    perror("fopen(/proc/meminfo)");
    return -1;
}

    line_count = 0;
while (line_count < MAX_LINES && fgets(line, sizeof(line), f_mem) != NULL) {
    fputs(line, stdout);
    line_count++;
}
if (fclose(f_mem) == EOF) {
    perror("fclose(/proc/meminfo)");
    return -1;
}
    return 0;
}

void compare_file_methods(void) {
    const char* test_file = "/proc/version";

    printf("Comparing file reading methods for: %s\n\n", test_file);

    printf("=== Method 1: Using System Calls ===\n");
    read_file_with_syscalls(test_file);

    printf("\n=== Method 2: Using Library Functions ===\n");
    read_file_with_library(test_file);

    printf("\nNOTE: Run this program with strace to see the difference!\n");
    printf("Example: strace -e trace=openat,read,write,close ./lab2\n");
}

int read_file_with_syscalls(const char* filename) {
    int fd;
char buffer[1024];
ssize_t bytes_read;

    fd = open(filename, O_RDONLY);
if (fd == -1) {
    perror("open");
    return -1;
}

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
    for (ssize_t i = 0; i < bytes_read; ++i) {
        if (buffer[i] == '\0') buffer[i] = ' ';
    }
    buffer[bytes_read] = '\0';
    printf("%s", buffer);
}

    if (bytes_read == -1) {
    perror("read");
    close(fd);
    return -1;
}

    if (close(fd) == -1) {
    perror("close");
    return -1;
}
return 0;

}

int read_file_with_library(const char* filename) {
    FILE *fp;
char line[1024];

    fp = fopen(filename, "r");
if (!fp) {
    perror("fopen");
    return -1;
}

   while (fgets(line, sizeof(line), fp) != NULL) {
    fputs(line, stdout);
}
    if (fclose(fp) == EOF) {
    perror("fclose");
    return -1;
}
return 0;

}

int is_number(const char* str) {
    if (str == NULL || *str == '\0') return 0;

    const unsigned char *p = (const unsigned char*)str;
while (*p) {
    if (!isdigit(*p)) return 0;
    p++;
}

    return 1;

}
