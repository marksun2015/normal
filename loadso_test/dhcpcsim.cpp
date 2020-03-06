#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string> 

int main(int argc, const char* argv[])
{
    void *fHandle;
    typedef int (*func)(const std::string& device, bool onoff);

	printf("dlopen \n");
    fHandle = dlopen("/usr/lib/libweintek-hal-cmt_rk3288_eglfs.so.1",RTLD_LAZY);
	if (!fHandle) {
        fprintf (stderr, "%s\n", dlerror());
        exit(1);
    }
    dlerror();

	printf("dlsym \n");
	func fptr;
	fptr = (func)dlsym(fHandle,"set_dhcp_status");

	printf("set dhcpc status \n");
    if (fptr) {
        fptr("eth0", 1);
    }

    dlclose(fHandle);
    return 0;
}
