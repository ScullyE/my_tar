
@thomasstampfl


Create Repl

Upgrade
Home
Apps
BETA
Templates
My Repls
Talk
Learn
Teams
Curriculum
Blog
About
Careers
Pricing
Discord


EthanScully
/
my_tar
C


Files




12345678910111213141516171819202122
#ifndef MY_STRING 
#define MY_STRING 

int my_strlen(char* str);
int my_strcmp(char* str1, char* str2);
char* my_strrev(char* str);
char* my_strcat(char* dest, const char* src);
char* my_strdup(char* str);
char* my_strstr(char* search_in, char* search_for);

Preview of README.md
my_tar
Project for Students @ Qwasar Silicon Valley



Team members:
Ethan Scully
Thomas Stampfl
Task
Recreate the Linux command line utility tar

Tar creates a single archive file containing multiple files & folders. The most important historical reason to create a single file archive was to be able to copy fixed sized junks onto a tape archive. By default tar does not compress the files.

Description
my_tar is a command to manipulate tape archive. The first option to tar is a mode indicator from the following list (the -f option is required for all):

-c Create a new archive containing the specified items.

-r Like -c, but new entries are appended to the archive.

-t List archive contents to stdout.

-u Like -r, but new entries are added only if they have a modification date newer than the corresponding entry in the archive.

-x Extract to disk from the archive. If a file with the same name appears more than once in the archive, each copy will be extracted, with later copies overwriting (replacing) earlier copies.

my_tar will not handle file inside subdirectory.

Usage
$ make
$ ./my_tar -options archive_name to_be_archived_files_and_folders
$ make
$ ./my_tar -options archive_name to_be_archived_files_and_folders
Have fun!
Console
Shell
Markdown

my_tar - Replit
