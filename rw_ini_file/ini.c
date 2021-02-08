/*ini.c*/
#include <stdio.h>  
#include <string.h>        
/* 
 * Function name: GetIniKeyString 
 * Entry parameters: title 
 * Identification of a set of data in the configuration file 
 *                  key 
 * The identifier of the value to be read in this set of data 
 *                  filename 
 * File path to be read 
 * Return value: Find the value you need to check and return the correct result 0 
 * Otherwise -1 
 */  
int GetIniKeyString(const char *title, const char *key, const char *filename,char *buf)  
{  
    FILE *fp;  
    int  flag = 0;  
    char sTitle[64], *wTmp;
    char sLine[1024];        
    sprintf(sTitle, "[%s]", title);

    if(NULL == (fp = fopen(filename, "r"))) {  
        perror("fopen");  
        return -1;
    }
    while (NULL != fgets(sLine, 1024, fp)) {  
        // This is the comment line  
        if (0 == strncmp("//", sLine, 2)) continue;  
        if ('#' == sLine[0])              continue;        
        wTmp = strchr(sLine, '=');  
        if ((NULL != wTmp) && (1 == flag)) {  
            if (0 == strncmp(key, sLine, strlen(key))) { // The length depends on the file read  
                sLine[strlen(sLine) - 1] = '\0';  
                fclose(fp);
                while(*(wTmp + 1) == ' '){
                    wTmp++;
                }
                strcpy(buf,wTmp + 1);
                return 0;  
            }  
        } else {  
            if (0 == strncmp(sTitle, sLine, strlen(sTitle))) { // The length depends on the file read  
                flag = 1; // find the title location  
            }  
        }  
    }  
    fclose(fp);  
    return -1;  
}        

/* 
 * Function name: PutIniKeyString 
 * Entry parameters: title 
 * Identification of a set of data in the configuration file 
 *                  key 
 * The identifier of the value to be read in this set of data 
 *                  val 
 * Changed value 
 *                  filename 
 * File path to be read 
 * Return value: Successful return 0 
 * Otherwise return -1 
 */  
int PutIniKeyString(const char *title, const char *key, const char *val, const char *filename)  
{  
    FILE *fpr, *fpw;  
    int  flag = 0;  
    char sLine[1024], sTitle[32], *wTmp;        
    sprintf(sTitle, "[%s]", title);  
    if (NULL == (fpr = fopen(filename, "r")))  
        return -1;// read the original file  
    sprintf(sLine, "%s.tmp", filename);  
    if (NULL == (fpw = fopen(sLine,    "w")))  
        return -1;// write temporary files        
    while (NULL != fgets(sLine, 1024, fpr)) {  
        if (2 != flag) { // if the line to be modified is found, no internal operations will be performed  
            wTmp = strchr(sLine, '=');  
            if ((NULL != wTmp) && (1 == flag)) {  
                if (0 == strncmp(key, sLine, strlen(key))) { // The length depends on the file read 
                    flag = 2;// Change the value for easy writing to the file  
                    sprintf(wTmp + 1, " %s\n", val);
                }  
            } else {
                if (0 == strncmp(sTitle, sLine, strlen(sTitle))) { // The length depends on the file read
                    flag = 1; // find the title location  
                }  
            }  
        }        
        fputs(sLine, fpw); // Write temporary files 
    }  
    fclose(fpr);  
    fclose(fpw);        
    sprintf(sLine, "%s.tmp", filename);  
    return rename(sLine, filename);// Update the temporary file to the original file  
}    
