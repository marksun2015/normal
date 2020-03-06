#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int *argc, char **argv)
{
    void *fHandle;
    void (*func)();

    fHandle = dlopen("/home/libfoo.so",RTLD_LAZY);

    if (!fHandle) {
        fprintf (stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror();

    func = (void(*)())dlsym(fHandle,"foo");

    if (func) {
        func("eth0", 0);
    }

    dlclose(fHandle);
    return 0;
}
