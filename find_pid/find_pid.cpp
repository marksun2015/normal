#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include <string.h>
#include <unistd.h>

using namespace std;

int getProcIdByName(string procName)
{
    int pid = -1;

    // Open the /proc directory
    DIR *dp = opendir("/proc");
    if (dp != NULL)
    {
        // Enumerate all entries in directory until process found
        struct dirent *dirp;
        while (pid < 0 && (dirp = readdir(dp)))
        {
            // Skip non-numeric entries
            int id = atoi(dirp->d_name);
            if (id > 0)
            {
                // Read contents of virtual /proc/{pid}/cmdline file
                string cmdPath = string("/proc/") + dirp->d_name + "/cmdline";
                ifstream cmdFile(cmdPath.c_str());
                string cmdLine;
                getline(cmdFile, cmdLine);
                if (!cmdLine.empty())
                {
                    // Keep first cmdline item which contains the program path
                    size_t pos = cmdLine.find('\0');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(0, pos);
                    // Keep program name only, removing the path
                    pos = cmdLine.rfind('/');
                    if (pos != string::npos)
                        cmdLine = cmdLine.substr(pos + 1);
                    // Compare against requested process name
                    if (procName == cmdLine)
                        pid = id;
                }
            }
        }
    }

    closedir(dp);

    return pid;
}

int readarg(FILE *fp, char *buf, int sz)
{
    int c = 0, f = 0;

    while (f < (sz - 1) && (c = fgetc(fp)) != EOF && c)
        buf[f++] = c;
    buf[f] = 0;

    return (c == EOF && f == 0) ? c : f;
}

int find_pid_by_name(const char *ProcName, int *foundpid)
{
    DIR *dir;
    struct dirent *d;
    int pid, i;
    char *s;
    int pnlen, f;
    FILE *fp;
    i = 0;
    foundpid[0] = 0;
    pnlen = strlen(ProcName);

    /* Open the /proc directory. */
    dir = opendir("/proc");
    if (!dir) {
        printf("cannot open /proc");
        return -1;
    }
    /* Walk through the directory. */
    while ((d = readdir(dir)) != NULL) {
        char exe[PATH_MAX + 1];
        char path[PATH_MAX + 1];

        int namelen;
        /* See if this is a process */
        if ((pid = atoi(d->d_name)) == 0)
            continue;

        snprintf(exe, sizeof(exe), "/proc/%s/cmdline", d->d_name);
        if ((fp = fopen(exe, "r")) == NULL)
            continue;

        f = readarg(fp, path, sizeof(path));
        if (f < 0) {
            fclose(fp);
            continue;
        }

        //log_debug("get name %s pid %d\n", path, pid);

        /* Find ProcName */
        s = strrchr(path, '/');
        if (s == NULL)
            s = path;
        else
            s++;
        fclose(fp);

        /* we don't need small name len */
        namelen = strlen(s);
        if (namelen < pnlen)
            continue;

        if (!strncmp(ProcName, s, pnlen)) {
            /* to avoid subname like search proc tao but proc taolinke matched */
            if (s[pnlen] == ' ' || s[pnlen] == '\0') {
                foundpid[i] = pid;
                i++;
            }
        }
    }
    foundpid[i] = 0;
    closedir(dir);
    return 0;
}

int main(int argc, char* argv[])
{
    // Fancy command line processing skipped for brevity

#if 0 
    //int pid = getProcIdByName(argv[1]);
    int id = getProcIdByName("background");
    cout << "pid: " << id << endl;
    //if(pid > 0)
        //kill(pid,SIGTERM);
#endif

    int pid[10] = { 0 };
    int s;
    //pid[0] = 123456;
    //s = kill(pid[0], 0);
    if(find_pid_by_name("background", pid) != -1) {
        cout << "pid: " << pid[0] << endl;
        if(pid[0] != 0) {      
            s = kill(pid[0],SIGKILL);
            sleep(1);
            s = kill(pid[0],0); //check process exist  
            cout << "s: " << s << endl;  // 0 : exist 
        }
    }

    return 0;
}
