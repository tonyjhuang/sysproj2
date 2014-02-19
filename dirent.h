typedef struct dirent_s {
unsigned int valid;
unsigned int first_block;
unsigned int size;
uid_t user;
gid_t group;
mode_t mode;
struct timespec access_time;
struct timespec modify_time;
struct timespec create_time;
char name[28];
} dirent;
