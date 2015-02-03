#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

int main() {
    struct rlimit lim;
    
    int i = getrlimit(RLIMIT_STACK, &lim);
    printf("stack size: %d\n",  (int)lim.rlim_cur );
    int k = getrlimit(RLIMIT_NPROC, &lim);
    printf("process limit: %d\n", (int)lim.rlim_cur);
    int j = getrlimit(RLIMIT_NOFILE, &lim);
    printf("max file descriptors: %d\n", (int)lim.rlim_cur);
    return 0;
}
