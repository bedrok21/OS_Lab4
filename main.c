#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUF_SIZE 1024

struct MemoryBlob {
    unsigned long addr;
    unsigned long value;
};

struct MemoryRegion {
    unsigned long start_addr;
    unsigned long end_addr;
    char perm[4];
};

struct MemoryRegion* read_maps_info(pid_t pid, int* count) {
    char filepath[BUF_SIZE];
    snprintf(filepath, sizeof(filepath), "/proc/%d/maps", pid);

    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Failed to open maps file");
        exit(EXIT_FAILURE);
    }

    struct MemoryRegion* regions = malloc(BUF_SIZE * sizeof(struct MemoryRegion));
    if (regions == NULL) {
        perror("Failed to allocate memory for regions");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char line[BUF_SIZE];
    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%lx-%lx %3s", &regions[i].start_addr, &regions[i].end_addr, regions[i].perm);
        i++;
    }

    fclose(file);
    *count = i;
    return regions;
}

void read_memory(pid_t pid, struct MemoryRegion* regions, int count) {

    struct MemoryBlob mem_blobs[100000];
    struct MemoryBlob mem_blob;

    char mempath[BUF_SIZE];
    snprintf(mempath, sizeof(mempath), "/proc/%d/mem", pid);

    int init = 1;
    while (1) {
        
      ptrace(PTRACE_ATTACH, pid, NULL, NULL);

      int mem_fd = open(mempath, O_RDONLY);
      if (mem_fd == -1) {
          perror("Failed to open mem file");
          exit(EXIT_FAILURE);
      }

      int j = 0;
      for (int i = 0; i < count; i++) {
          if (strstr(regions[i].perm, "rw") != NULL) {
              for (unsigned long addr = regions[i].start_addr; addr < regions[i].end_addr; addr += sizeof(unsigned long)) {
                  if (pread(mem_fd, &mem_blob.value, sizeof(unsigned long), addr) == -1) {
                      perror("Failed to read memory");
                      exit(EXIT_FAILURE);
                  }
                  if (init){
                    mem_blobs[j] = mem_blob;
                  } else if (mem_blobs[j].value != mem_blob.value) {
                      printf(" %ld  ->  %ld\n", mem_blobs[j].value, mem_blob.value);
                      mem_blobs[j] = mem_blob;
                  }
                  j++;
              }
          }
      }
      init = 0;

      ptrace(PTRACE_DETACH, pid, NULL, NULL);

      close(mem_fd);

      sleep(1);  
    }

}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t pid = atoi(argv[1]);

    int region_count;
    struct MemoryRegion* regions = read_maps_info(pid, &region_count);
    
    read_memory(pid, regions, region_count);
    
    return 0;
}
