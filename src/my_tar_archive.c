#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <errno.h>


#include <string.h>


#include "tar.h"
#include "my_tar_archive.h"
#include "helper_file.h"
#include "../lib/string/my_string.h"
#include "../lib/string/string_array.h"
#include "../lib/string/string_listnode.h"


#define OCT_BASE 8


int archive(char flag, char* archive_name, int access_mode, string_array* filenames)
{
    int fd = open(archive_name, access_mode, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(fd != -1)
    {
        switch(flag)
        {
            case 'c':
            case 'r':
            case 'u':
                process_to_archive_files(fd, flag, filenames);
                break;
            case 't':
                list_archive_filenames(fd);
                break;
            case 'x':
                extract_archive(fd);
                break;
        }        
    }
    else
    {
        print_error(ERR_OPENING_ARCH_FILE);
        return -1;
    }
    return close(fd);    
}


int process_to_archive_files(int fd, char flag, string_array* filenames)
{
    for(int i = 0; i < filenames->size; i++)
        switch(flag)
        {
            case 'r':
            case 'u':
                append_file_to_archive(fd, filenames->array[i], flag);
                break;
            case 'c':
                add_file_to_archive(fd, filenames->array[i]);
                break;
        }

    write_end_of_file_block_to_arch(fd);
    return 0;
}


int list_archive_filenames(int fd)
{
    int bytes = 0;
    char str_header[HEADERSIZE];
    struct posix_header* header = malloc(sizeof(struct posix_header));
    string_listnode* head = NULL;

    while( (bytes = read(fd, str_header, HEADERSIZE)) == HEADERSIZE)
    {
        if(str_contains_valid_posix_header(str_header) == 1)
        {
            str_to_posix_header(str_header, header);
            
            head = string_listnode_push_element(head, header->name);
            
            int _offset = calc_filesize_offset_blocks(header->size);
            lseek(fd, _offset, SEEK_CUR);
        }
        // else
            // ERR_SKIP_HEADER
    }

    if(head == NULL)
    {
        print_error(ERR_NOT_TAR_FORMAT);
    }
    else
    {
        string_listnode_print(head, "\n", 1);
        string_listnode_free(head);
    }

    free(header);
    return 0;
}


int extract_archive(int archive_fd)
{
    int offset = 0;
    int bytes = 0;
    char str_header[HEADERSIZE];
    struct posix_header* header = malloc(sizeof(struct posix_header));

    while( (bytes = read(archive_fd, str_header, HEADERSIZE)) == HEADERSIZE)
    {
        if(str_contains_valid_posix_header(str_header) == 1)
        {
            str_to_posix_header(str_header, header);

            // directory
            if(header->typeflag[0] == '5')
                create_folder(header);
            else
            {
                offset = extract_file_from_archive(archive_fd, header);
                lseek(archive_fd, offset, SEEK_CUR);
            }
        }
        else
            print_error(ERR_SKIP_HEADER);
    }

    free(header);
    return 0;
}


int extract_file_from_archive(int archive_fd, struct posix_header* header)
{    
    int offset = 0;
    
    // using 10 as base means char to num - no conversion
    mode_t mode = my_usign_atoi(header->mode, OCT_BASE);

    int file_fd = open(header->name, O_CREAT | O_TRUNC | O_WRONLY, mode);
    
    // resolved some bugs? unknown issues
    chmod(header->name, mode);

    if(file_fd != 1)
        offset = copy_content_to_file(archive_fd, file_fd, header);
    else
    {
        printf("Error while extracting file %s!\n", header->name);
        return -1;
    }

    if (close(file_fd) < 0)
        return -1;

    return offset;
}


int create_folder(struct posix_header* header)
{
    DIR* _dir = opendir(header->name);

    if(_dir)
        return closedir(_dir);
    else if(ENOENT == errno)
        return mkdir(header->name, my_usign_atoi(header->mode, OCT_BASE));
    else
        return -1;
}


int copy_content_to_file(int archive_fd, int file_fd, struct posix_header* header)
{
    unsigned int size = 0;
    int offset = 0;
    char* temp;

    // Allocate memory and skip in archive file
    size = my_usign_atoi(header->size, OCT_BASE);
    offset = size / BLOCKSIZE;
    offset += (size % BLOCKSIZE) == 0 ? 0 : BLOCKSIZE;

    // size extremly large -> RAM !!!
    temp = malloc(sizeof(char) * size);

    read(archive_fd, temp, size);
    write(file_fd, temp, size);
    free(temp);

    return offset - size;
}


int append_file_to_archive(int fd, char* filename, char flag)
{
    int u_append = 0;
    int lseek_offset = 0;
    int count_empty_end_blocks = 0;

    int bytes = 0;
    char str_header[HEADERSIZE];
    struct posix_header* header = malloc(sizeof(struct posix_header));

    while( (bytes = read(fd, str_header, HEADERSIZE) ) == HEADERSIZE)
    {
        if(count_empty_end_blocks == 0 && str_contains_valid_posix_header(str_header) == 1)
        {
            str_to_posix_header(str_header, header);

            if(flag == 'u' && my_strcmp(filename, header->name) == 0)
                u_append = cmp_filetime_mtime(filename, header->mtime);

            int _offset = calc_filesize_offset_blocks(header->size);
            lseek_offset = lseek(fd, _offset, SEEK_CUR);
        }
        else
        {
            unsigned int i = 0;
            while( (i < HEADERSIZE) && (str_header[i] != '\0') )
                i++;

            if(i > 0 && str_header[i-1] != '\0')
            {
                printf("\033[1;31m");
                printf("no valid header found!\n");
                printf("\033[0m");
            }
            else
            {
                // printf("only zeros found!\n");
                count_empty_end_blocks++;         
            }
        }
    }

    if(count_empty_end_blocks < 2 || (bytes > 0 && bytes < HEADERSIZE) )
    {
        free(header);
        return  ERR_NOT_TAR_FORMAT;
    }

    lseek(fd, lseek_offset, SEEK_SET);
    if(flag == 'r' || (flag == 'u' && u_append))
        add_file_to_archive(fd, filename);

    free(header);
    return 0;
}


char* concat_dir_file_names(char* dir, char* file)
{
    int dirlen = my_strlen(dir);
    int filelen = my_strlen(file);
    int length = dirlen + filelen + 3;

    char* ret = malloc(sizeof(char) * length);
    my_strncpy(ret, "\0", length);

    my_strcat(ret, dir);
    if(dir[dirlen - 1] != '/')
        my_strcat(ret, "/");

    my_strcat(ret, file);

    return ret;
}


int write_dir_contents_to_archive(int archive_fd, char* path_to_dir)
{
    int bytes = 0;
    char* path_to_file = NULL;
    
    struct dirent* de;
    DIR* _dir = opendir(path_to_dir);
    
    if(_dir != NULL)
    {
        while((de = readdir(_dir)) != NULL)
        {
            if(my_strcmp(".", de->d_name) != 0 && my_strcmp("..", de->d_name) != 0)
            {
                path_to_file = concat_dir_file_names(path_to_dir, de->d_name);
                bytes += add_file_to_archive(archive_fd, path_to_file);
                free(path_to_file);
            }
        }
    }
    else
        return -1;
    
    closedir(_dir);
    return bytes;
}


int add_file_to_archive(int archive_fd, char* path_to_file)
{
    int res, bytes = 0;
    struct posix_header* header = malloc(sizeof(struct posix_header));

    res = build_posix_header(path_to_file, header);
    if(res == 0)
    {
        bytes += write_posix_header_to_archive(archive_fd, header);

        // If directory, copy directory header and contents. Don't copy contents of subdirs
        if(header->typeflag[0] == '5')  
            write_dir_contents_to_archive(archive_fd, header->name);
        else
            bytes += write_content_to_archive(archive_fd, path_to_file);
    }
    else
        bytes = res;

    free(header);
    return bytes;
}


int cmp_filetime_mtime(char* path_to_file, char* mtime)
{
    int time = my_usign_atoi(mtime, OCT_BASE);
    int stat_ret;
    struct stat sb;

    stat_ret = stat(path_to_file, &sb);
    if(stat_ret == 0)
    {
        if(sb.st_mtime > time)
            return 1;
        else if(sb.st_mtime > time)
            return 0;
        else
            return -1;
    }

    return -2;
}


int calc_filesize_offset_blocks(char* strsize)
{
    int offset = 0;
    int size = my_usign_atoi(strsize, OCT_BASE);

    // How often does HEADERSIZE fit in size?
    offset = size / HEADERSIZE;

    // Is there a remainder? If yes, increment by one (empty filled space)
    if(size % HEADERSIZE != 0)
        offset++;
    
    // Calculated offset of bytes to skip content and eventually empty filled space 
    offset *= HEADERSIZE; 
    
    return offset;
}

int build_posix_header(char* filename, struct posix_header* header)
{
    int stat_ret;
    struct stat sb;

    stat_ret = stat(filename, &sb);
    if(stat_ret == 0) 
    {
        my_strncpy(header->name, filename, SIZE_NAME);                                                  // NAME
        get_st_mode(&sb.st_mode, header->mode);                                                         // SIZE
        my_usign_itoa_right_shift_fixed_length(sb.st_uid, header->uid, OCT_BASE, SIZE_UID);             // UID
        my_usign_itoa_right_shift_fixed_length(sb.st_gid, header->gid, OCT_BASE, SIZE_GID);             // GID
        my_strncpy(header->typeflag, get_typeflag(sb.st_mode), SIZE_TYPEFLAG);                          // TYPEFLAG
        get_size(header->typeflag[0], &sb.st_size, header->size);                                       // SIZE
        my_usign_itoa_right_shift_fixed_length(sb.st_mtime, header->mtime, OCT_BASE, SIZE_MTIME);       // MTIME
        my_strncpy(header->chksum, STR_CHKSUM, SIZE_CHKSUM);                                            // CHECKSUM
        my_strncpy(header->linkname, "\0", SIZE_LINKNAME);                                              // LINKNAME
        my_strncpy(header->magic, STR_MAGIC, SIZE_MAGIC);                                               // MAGIC
        my_strncpy(header->version, STR_VERSION, SIZE_VERSION);                                         // VERSION
        my_strncpy(header->uname, get_pw_name(sb.st_uid), SIZE_UNAME);                                  // UNAME
        my_strncpy(header->gname, get_gr_name(sb.st_gid), SIZE_GNAME);                                  // GNAME 
        // TO-DO: ask Gaetan function pointer
        // get_dev_id(header->typeflag[0], &sb.st_dev, header->devmajor, major); // DEVMAJOR
        get_devmajor(header->typeflag[0], &sb.st_dev, header->devmajor);                                // DEVMAJOR
        get_devminor(header->typeflag[0], &sb.st_dev, header->devminor);                                // DEVMINOR      
        my_strncpy(header->prefix, "\0", SIZE_PREFIX);                                                  // PREFIX
    }
    else
    {
        printf("Error while getting file status! File %s not found!\n", filename);
        return -1;
    }

    return 0;
}


char* posix_header_to_str(char* dest, struct posix_header* header)
{
    return my_strncpy_no_zero_filling(dest, (char*)header, sizeof(struct posix_header));
}


int str_to_posix_header(char* str_header, struct posix_header* header)
{
    my_strmemcpy((char*)header, str_header, sizeof(struct posix_header));
    return 0;
}


int str_contains_valid_posix_header(char* str)
{
    char str_archive_chksum[SIZE_CHKSUM] = "\0";
    char str_calculated_chksum[SIZE_CHKSUM] = STR_CHKSUM;
    char copy[HEADERSIZE] = "\0";

    // copy of original string - no modification of source 
    my_strmemcpy(copy, str, HEADERSIZE);

    // copy checksum, because for calculation the chksum field must contain all CHR_CHKSUM_CALC (' ' -> space) characters
    my_strmemcpy(str_archive_chksum, &copy[OFFSET_CHKSUM], SIZE_CHKSUM);
    
    calc_chksum_to_oct_str(copy, str_calculated_chksum, HEADERSIZE);
    
    // CHECK IF CHECKSUMS ARE EQUAL
    for(int i = 0; i < SIZE_CHKSUM; i++)
        if(str_archive_chksum[i] != str_calculated_chksum[i])
            return 0;

    my_strncpy(&copy[OFFSET_CHKSUM], str_archive_chksum, SIZE_CHKSUM);
    return 1;
}


int write_posix_header_to_archive(int archive_fd, struct posix_header* header)
{
    char str[HEADERSIZE] = "\0";

    // Each record of n blocks is written with a single ‘write()’ operation
    posix_header_to_str(str, header);
    calc_chksum_to_oct_str(str, header->chksum, HEADERSIZE);
  
    my_strncpy(&str[OFFSET_CHKSUM], header->chksum, SIZE_CHKSUM - 1);

    return write(archive_fd, str, HEADERSIZE);
}


int write_content_to_archive(int archive_fd, char* path_to_file)
{
    int ret = 0;

    int file_fd = open(path_to_file, O_RDONLY);
    if(file_fd != -1)
        ret = fd_copy_content_blocksize(archive_fd, file_fd);
    else
    {
        printf("%s could not be opened!\n", path_to_file);
        ret = -1;
    }

    close(file_fd);
    return ret;
}


int fd_copy_content_blocksize(int dest, int src)
{
    int bytes = 0, total_bytes = 0;
    char* str;
    char* content = malloc(sizeof(char) * BLOCKSIZE);

    while((bytes = read(src, content, BLOCKSIZE)) == BLOCKSIZE)
    {
        write(dest, content, BLOCKSIZE);
        total_bytes += bytes;
    }
    if(bytes > 0)
    {
        // fill blocksize
        str = malloc(sizeof(char) * BLOCKSIZE);
        for(int i = 0; i < BLOCKSIZE; i++)
            str[i] = (i < bytes) ? content[i] : '\0';

        write(dest, str, BLOCKSIZE);
        total_bytes += BLOCKSIZE;
        free(str);
    }

    free(content);
    return total_bytes;
}


void print_header(struct posix_header* header)
{
    helper_log_posix_header("name: ", header->name, SIZE_NAME);
    helper_log_posix_header("mode: ", header->mode, SIZE_MODE);
    helper_log_posix_header("uid: ", header->uid, SIZE_UID);
    helper_log_posix_header("gid: ", header->gid, SIZE_GID);
    helper_log_posix_header("size: ", header->size, SIZE_SIZE);
    helper_log_posix_header("mtime: ", header->mtime, SIZE_MTIME);
    helper_log_posix_header("chksum: ", header->chksum, SIZE_CHKSUM);
    helper_log_posix_header("typeflag: ", header->typeflag, SIZE_TYPEFLAG);
    helper_log_posix_header("linkname: ", header->linkname, SIZE_LINKNAME);
    helper_log_posix_header("magic: ", header->magic, SIZE_MAGIC);
    helper_log_posix_header("version: ", header->version, SIZE_VERSION);
    helper_log_posix_header("uname: ", header->uname, SIZE_UNAME);
    helper_log_posix_header("gname: ", header->gname, SIZE_GNAME);
    helper_log_posix_header("devmajor: ", header->devmajor, SIZE_DEVMAJOR);
    helper_log_posix_header("devminor: ", header->devminor, SIZE_DEVMINOR);
    helper_log_posix_header("prefix: ", header->prefix, SIZE_PREFIX);
}


unsigned int calculate_checksum(char* str, size_t n)
{
    /*
        The chksum field represents the simple sum of all bytes in the header block.
        Each 8-bit byte in the header is added to an unsigned integer, initialized to
        zero, the precision of which shall be no less than seventeen bits. When
        calculating the checksum, the chksum field is treated as if it were all blanks.
    */    
    unsigned int count = 0, chksum = 0;

    while(count < n)
        chksum += str[count++];

    return chksum;
}


char* calc_chksum_to_oct_str(char* str, char* dest, size_t n)
{
    unsigned int value = 0;

    for(int i = OFFSET_CHKSUM; i < (OFFSET_CHKSUM + SIZE_CHKSUM - 1); i++)
        str[i] = CHR_CHKSUM_CALC;

    value = calculate_checksum(str, n);
    my_usign_itoa_right_shift_fixed_length(value, dest, OCT_BASE, SIZE_CHKSUM - 1);

    return dest;
}


void write_end_of_file_block_to_arch(int fd)
{
    /*
        -b, --blocking-factor=BLOCKS
        BLOCKS x 512 bytes per record
        DEFAULT: -b20
        20 x 512 = 10240 bytes
    */

    int bytes = 0;
    int invalid_header = 0;
    int count_empty_end_blocks = 0;
    int lseek_offset = -1;
    char str_header[HEADERSIZE];

    lseek(fd, 0, SEEK_SET);
    while( (bytes = read(fd, str_header, HEADERSIZE) ) == HEADERSIZE)
    {
        if(count_empty_end_blocks == 0 && str_contains_valid_posix_header(str_header) == 1)
        {
            int _offset = calc_filesize_offset_blocks(&str_header[OFFSET_SIZE]);
            lseek_offset = lseek(fd, _offset, SEEK_CUR);
        }
        else
        {
            unsigned int i = 0;
            while( (i < HEADERSIZE) && (str_header[i] != '\0') )
                i++;

            if(i > 0 && str_header[i-1] != '\0')
                invalid_header++;
            else
                count_empty_end_blocks++;         
        }
    }

    if(lseek_offset > 0 && count_empty_end_blocks < 2)
    {
        int tot_bytes = lseek_offset + 2 * BLOCKSIZE;
        int block_bytes = BLOCKS * BLOCKSIZE;
        int remaining = block_bytes - (tot_bytes % block_bytes);

        char eof_block[BLOCKSIZE] = "\0";

        lseek_offset = lseek(fd, lseek_offset, SEEK_SET);
        while( (remaining + 2 * BLOCKSIZE) > 0)
            remaining -= write(fd, eof_block, BLOCKSIZE);
    }
}


void get_st_mode(mode_t* st_mode, char* dest)
{
    int helper = *st_mode & (S_IRWXU | S_IRWXG | S_IRWXO);
    my_usign_itoa_right_shift_fixed_length(helper, dest, OCT_BASE, SIZE_MODE);
}


void get_size(char typeflag, off_t* st_size, char* dest)
{
    off_t size = 0;
    // if directory -> size = 0;
    if(typeflag != '5') 
        size = *st_size;

    my_usign_itoa_right_shift_fixed_length(size, dest, OCT_BASE, SIZE_SIZE);
}


char* get_typeflag(mode_t mode)
{
    /*    
        The following POSIX macros are defined to check the file
        type using the st_mode field:
        S_ISREG(m)      is it a regular file?
        S_ISDIR(m)      directory?
        S_ISCHR(m)      character device?
        S_ISBLK(m)      block device?
        S_ISFIFO(m)     FIFO (named pipe)?
        S_ISLNK(m)      symbolic link? (Not in POSIX.1-1996.)
        S_ISSOCK(m)     socket? (Not in POSIX.1-1996.)
    */
    if(S_ISREG(mode) != 0)     // is it a regular file?
        return "0";
    if(S_ISLNK(mode) != 0)     // symbolic link? (Not in POSIX.1-1996.)
        return "1";
    if(S_ISCHR(mode) != 0)     // character device?
        return "3";
    if(S_ISBLK(mode) != 0)     // block device?
        return "4";
    if(S_ISDIR(mode) != 0)     // directory?
        return "5";
    if(S_ISFIFO(mode) != 0)    //FIFO (named pipe)?
        return "6";        
    // if(S_ISSOCK(mode) != 0)  //socket? (Not in POSIX.1-1996.)
    //   return "?";
    return "\0";
}


void get_dev_id(char typeflag, dev_t* st_dev, char* dest, unsigned int (*func)(dev_t))
{
    // TO-DO: use SIZE_DEVMAJOR as parameter -> function has 5 parameters -> Statement Gaetan?
    if(typeflag == '4') 
        my_usign_itoa_right_shift_fixed_length((*func)(*st_dev), dest, OCT_BASE, SIZE_DEVMAJOR);
    else
        my_strncpy(dest, "\0", SIZE_DEVMAJOR);
}


void get_devmajor(char typeflag, dev_t* st_dev, char* dest)
{    
    if(typeflag == '4') 
        my_usign_itoa_right_shift_fixed_length(major(*st_dev), dest, OCT_BASE, SIZE_DEVMAJOR);
    else
        my_strncpy(dest, "\0", SIZE_DEVMAJOR);
}


void get_devminor(char typeflag, dev_t* st_dev, char* dest)
{
    if(typeflag == '4')
        my_usign_itoa_right_shift_fixed_length(minor(*st_dev), dest, OCT_BASE, SIZE_DEVMINOR);
    else
        my_strncpy(dest, "\0", SIZE_DEVMINOR);
}


char* get_pw_name(uid_t uid)
{
    struct passwd* pwd = getpwuid(uid);
    return pwd->pw_name;
}


char* get_gr_name(gid_t gid)
{
    struct group* grp = getgrgid(gid);
    return grp->gr_name;
}


char process_flags(int _argc, char *_argv[])
{
    int fcheck = 0;
    char flag = '\0';
    for(int i = 1; i < _argc; i++)
    {
        if(*_argv[i] == '-')
        {
            int walk = 0;
            while (*(_argv[i]+walk) != '\0')
            {
                if (*(_argv[i]+walk) == 'f')
                    fcheck = 1;
                else 
                    flag = *(_argv[i]+walk);
                walk++;
            }
        }
    }
    if (fcheck)
        return flag;
    else
        return '\0';
}


void print_error(int _type)
{
    switch(_type)
    {
        case ERR_FLAGS:
            printf("Please use flags -c, -r, -t, -u, or -x, along with f\n");
            break;
        case ERR_NOT_TAR_FORMAT:
            printf("File is not a tar formatted file!\n");
            break;
        case ERR_NO_TAR_FILE:
            printf("No tar file provided!\n");
            break; 
        case ERR_OPENING_ARCH_FILE:
            printf("Error opening archive file!\n");
            break;
        case ERR_CREATING_ARCHIVE:
            printf("Error creating archive file!\n");
            break;
        case ERR_SKIP_HEADER:
            printf("Error: Skipping Header.\n");
            break;
    }
}


char** get_inputs(char* _argv[], char** _filelist, string_array _filenames){
    
    for (int i = 0; i < _filenames.size; i++){
      _filelist[i] = malloc(sizeof(**_filelist) * 100);
      my_strcpy(_filelist[i], "./");
      my_strcat(_filelist[i], _argv[i+3]);
    }   

    return _filelist;
}