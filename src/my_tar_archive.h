#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>


#include "tar.h"
#include "../lib/string/string_array.h"


#ifndef MY_TAR_ARCHIVE_H
#define MY_TAR_ARCHIVE_H


char** get_inputs(char* _argv[], char** _filelist, string_array _filenames);
char process_flags(int _argc, char *_argv[]);


int archive(char flag, char* archive_name, int access_mode, string_array* filenames);

int list_archive_filenames(int fd);

int extract_archive(int archive_fd);
int extract_file_from_archive(int archive_fd, struct posix_header* header);
int create_folder(struct posix_header* header);
int copy_content_to_file(int archive_fd, int file_fd, struct posix_header* header);

int process_to_archive_files(int fd, char flag, string_array* filenames);
int append_file_to_archive(int fd, char* filename, char flag);
char* concat_dir_file_names(char* dir, char* file);
int write_dir_contents_to_archive(int archive_fd, char* path_to_dir);
int add_file_to_archive(int archive_fd, char* path_to_file);


int cmp_filetime_mtime(char* path_to_file, char* mtime);
int calc_filesize_offset_blocks(char* strsize);


char* posix_header_to_str(char* dest, struct posix_header* header);
int str_to_posix_header(char* str_header, struct posix_header* header);
int str_contains_valid_posix_header(char* str);


int build_posix_header(char* filename, struct posix_header* header);
int write_posix_header_to_archive(int archive_fd, struct posix_header* header);

int write_content_to_archive(int archive_fd, char* filename);
int fd_copy_content_blocksize(int dest, int src);
void write_end_of_file_block_to_arch(int fd);


unsigned int calculate_checksum(char* str, size_t n);
char* calc_chksum_to_oct_str(char* str, char* dest, size_t n);


void get_st_mode(mode_t* st_mode, char* dest);
void get_size(char typeflag, off_t* st_size, char* dest);
char* get_typeflag(mode_t mode);

void get_dev_id(char typeflag, dev_t* st_dev, char* dest, unsigned int (*func)(dev_t));
void get_devmajor(char typeflag, dev_t* st_dev, char* dest);
void get_devminor(char typeflag, dev_t* st_dev, char* dest);

char* get_pw_name(uid_t uid);
char* get_gr_name(gid_t gid);


void print_header(struct posix_header* header);
void print_error(int _type);

#endif