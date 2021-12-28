#include <stdio.h>
#include <stdlib.h>

#include "my_tar_archive.h"
#include "../lib/string/string_array.h"
#include "../lib/string/my_string.h"

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        print_error(ERR_NO_TAR_FILE);
        return 1;
    }
    
    char flag = process_flags(argc, argv);
    int access_mode = 0;
    char* archivename = argv[2];

    //get other inputs
    string_array filenames;
    filenames.size = argc-3;
    
    char **filelist = malloc(filenames.size * sizeof(char*));
    filenames.array = get_inputs(argv, filelist, filenames);

    if(flag == 'c')
        access_mode = O_CREAT | O_TRUNC | O_RDWR;

    else if(flag == 'r' || flag == 'u')
        access_mode = O_CREAT | O_RDWR;

    else if(flag == 't' || flag == 'x')
        access_mode = O_RDONLY;

    archive(flag, archivename, access_mode, &filenames);

    for (int i = 0; i < filenames.size; i++)
        free(filelist[i]);
    free(filelist);

    return 0;
}
