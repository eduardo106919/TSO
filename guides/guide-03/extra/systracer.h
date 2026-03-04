#ifndef SYSTRACER_H
#define SYSTRACER_H

struct data_t
{
   int pid;
   char command[16];
   long syscall_id;
   int type;
};

enum type_e
{
   SYSCALL,
   VFS,
   EXT4,
   BLOCK,
};

enum my_op
{
   MYOP_OPENAT = 2,
   MYOP_READ = 0,
   MYOP_WRITE = 1,
   MYOP_FORK = 57,
};

#endif
