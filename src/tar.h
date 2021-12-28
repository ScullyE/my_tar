#ifndef TAR_H
#define TAR_H

#define HEADERSIZE  512 
#define BLOCKSIZE   512
#define BLOCKS      20          // -b, --blocking-factor=BLOCKS     DEFAULT = 20

#define TSUID    04000          /* set UID on execution */
#define TSGID    02000          /* set GID on execution */
#define TSVTX    01000          /* reserved */
                                /* file permissions */
#define TUREAD   00400          /* read by owner */
#define TUWRITE  00200          /* write by owner */
#define TUEXEC   00100          /* execute/search by owner */
#define TGREAD   00040          /* read by group */
#define TGWRITE  00020          /* write by group */
#define TGEXEC   00010          /* execute/search by group */
#define TOREAD   00004          /* read by other */
#define TOWRITE  00002          /* write by other */
#define TOEXEC   00001          /* execute/search by other */

/*
The chksum field represents the simple sum of all bytes in the header block. Each 8-bit byte in the header is added to an unsigned integer, initialized to zero, the precision of which shall be no less than seventeen bits. When calculating the checksum, the chksum field is treated as if it were all blanks.
*/


#define SIZE_NAME       100
#define SIZE_MODE         8 
#define SIZE_UID          8
#define SIZE_GID          8
#define SIZE_SIZE        12
#define SIZE_MTIME       12
#define SIZE_CHKSUM       8
#define SIZE_TYPEFLAG     1
#define SIZE_LINKNAME   100
#define SIZE_MAGIC        6
#define SIZE_VERSION      2
#define SIZE_UNAME       32
#define SIZE_GNAME       32
#define SIZE_DEVMAJOR     8
#define SIZE_DEVMINOR     8
#define SIZE_PREFIX     155
#define SIZE_SPARSE       12


/*
  DOCS:
  #define TMAGIC   "ustar "        ustar and a null
  #define TVERSION " \0"           00 and no null
*/
/*
  MATCHING THE ACTUAL OUTPUT OF TAR
*/
#define STR_MAGIC "ustar "
#define STR_VERSION " \0"

#define STR_CHKSUM "        "
#define STR_SPARSE "\0\0\0\0\0\0\0\0\0\0\0\0"


#define CHR_CHKSUM_CALC     ' '
#define OFFSET_CHKSUM       SIZE_NAME + SIZE_MODE + SIZE_UID + SIZE_GID + SIZE_SIZE + SIZE_MTIME
#define OFFSET_SIZE         SIZE_NAME + SIZE_MODE + SIZE_UID + SIZE_GID


struct posix_header
{                                   /* byte offset */
    char name[SIZE_NAME];           /*   0 */
    char mode[SIZE_MODE];           /* 100 */
    char uid[SIZE_UID];             /* 108 */
    char gid[SIZE_GID];             /* 116 */
    char size[SIZE_SIZE];           /* 124 */
    char mtime[SIZE_MTIME];         /* 136 */
    char chksum[SIZE_CHKSUM];       /* 148 */
    char typeflag[SIZE_TYPEFLAG];   /* 156 */
    char linkname[SIZE_LINKNAME];   /* 157 */
    char magic[SIZE_MAGIC];         /* 257 */
    char version[SIZE_VERSION];     /* 263 */
    char uname[SIZE_UNAME];         /* 265 */
    char gname[SIZE_GNAME];         /* 297 */
    char devmajor[SIZE_DEVMAJOR];   /* 329 */
    char devminor[SIZE_DEVMINOR];   /* 337 */
    char prefix[SIZE_PREFIX];       /* 345 */
                                    /* 500 */
    // char sparse[SIZE_SPARSE];       /* 512 */
    /* debatable if sparse should be part*/
};


/* ERROR DECLARATIONS */
#define ERR_FLAGS               1
#define ERR_NOT_TAR_FORMAT      2
#define ERR_NO_TAR_FILE         3
#define ERR_OPENING_ARCH_FILE   4 
#define ERR_CREATING_ARCHIVE    5
#define ERR_SKIP_HEADER         6


#endif