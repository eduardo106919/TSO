#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "systracer.h"

struct
{
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 8 * 1024 * 1024);
} events SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __type(key, u32);
    __type(value, u64);
    __uint(max_entries, 1);
} lost_events SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, u32);
    __type(value, u32);
    __uint(max_entries, 100);
} pids SEC(".maps");

struct block_rq_submission
{
    uint16_t common_type;
    uint8_t common_flags;
    uint8_t common_preempt_count;
    int32_t common_pid;
    uint32_t dev;
    uint64_t sector;
    uint32_t nr_sectors;
    uint32_t nr_bytes;
    uint16_t ioprio;
    int8_t rwbs[10];
    int8_t comm[16];
    int8_t cmd[4];
};

int to_discard(u32 pid)
{
    u32 *value = bpf_map_lookup_elem(&pids, &pid);
    if (value == NULL)
    {
        return 1;
    }
    return *value != pid;
}

int create_and_submit_event(u32 pid, u64 id, u32 type)
{
    struct data_t *data = bpf_ringbuf_reserve(&events, sizeof(struct data_t), 0);
    if (!data)
    {
        u32 key = 0;
        u64 *lost_counter = bpf_map_lookup_elem(&lost_events, &key);
        if (lost_counter)
        {
            __sync_fetch_and_add(lost_counter, 1);
            return 0;
        }
        return 1;
    }

    data->pid = pid;
    data->syscall_id = id;
    data->type = type;
    bpf_get_current_comm(&data->command, sizeof(data->command));

    bpf_ringbuf_submit(data, 0);

    return 0;
}

SEC("tp/syscalls/sys_enter_openat")
int BPF_PROG(sys_enter_openat,
             struct pt_regs *regs,
             long syscall_id)
{
    char comm[16];
    bpf_get_current_comm(comm, sizeof(comm));
    if (__builtin_memcmp(comm, "systracer", 9) == 0)
    {
        return 0;
    }

    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, syscall_id, SYSCALL);
}

SEC("tp/syscalls/sys_enter_read")
int BPF_PROG(sys_enter_read,
             struct pt_regs *regs,
             long syscall_id)
{
    char comm[16];
    bpf_get_current_comm(comm, sizeof(comm));
    if (__builtin_memcmp(comm, "systracer", 9) == 0)
    {
        return 0;
    }

    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, syscall_id, SYSCALL);
}

SEC("tp/syscalls/sys_enter_write")
int BPF_PROG(sys_enter_write,
             struct pt_regs *regs,
             long syscall_id)
{
    char comm[16];
    bpf_get_current_comm(comm, sizeof(comm));
    if (__builtin_memcmp(comm, "systracer", 9) == 0)
    {
        return 0;
    }

    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, syscall_id, SYSCALL);
}

SEC("tp/syscalls/sys_enter_close")
int BPF_PROG(sys_enter_close,
             struct pt_regs *regs,
             long syscall_id)
{
    char comm[16];
    bpf_get_current_comm(comm, sizeof(comm));
    if (__builtin_memcmp(comm, "systracer", 9) == 0)
    {
        return 0;
    }

    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, syscall_id, SYSCALL);
}

SEC("kprobe/vfs_open")
int BPF_KPROBE(vfs_enter_open,
               const struct path *path,
               struct file *file)
{
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, MYOP_OPENAT, VFS);
}

SEC("kprobe/vfs_read")
int BPF_KPROBE(vfs_enter_read,
               const struct path *path,
               struct file *file)
{
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, MYOP_READ, VFS);
}

SEC("kprobe/vfs_write")
int BPF_KPROBE(vfs_enter_write,
               const struct path *path,
               struct file *file)
{
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, MYOP_WRITE, VFS);
}

SEC("kprobe/ext4_file_open")
int BPF_KPROBE(ext4_enter_open,
               struct inode *inode,
               struct file *filp)
{
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, MYOP_OPENAT, EXT4);
}

SEC("kprobe/ext4_file_read_iter")
int BPF_KPROBE(ext4_enter_read,
               struct kiocb *iocb, struct iov_iter *to)
{
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, MYOP_READ, EXT4);
}

SEC("kprobe/ext4_file_write_iter")
int BPF_KPROBE(ext4_enter_write,
               struct kiocb *iocb, struct iov_iter *from)
{
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;
    return create_and_submit_event(pid, MYOP_WRITE, EXT4);
}

SEC("tp/block/block_rq_issue")
int trace_block_rq(struct block_rq_submission *ctx)
{
    u32 pid = bpf_get_current_pid_tgid() >> 32;
    if (to_discard(pid))
        return 0;

    if (ctx->rwbs[0] == 'R')
    {
        return create_and_submit_event(pid, MYOP_READ, BLOCK);
    }
    else if (ctx->rwbs[0] == 'W')
    {
        return create_and_submit_event(pid, MYOP_WRITE, BLOCK);
    }
    return 0;
}

SEC("tp/sched/sched_process_fork")
int handle_sched_fork(struct trace_event_raw_sched_process_fork *ctx)
{
    u32 parent_pid = ctx->parent_pid;
    u32 child_pid = ctx->child_pid;

    // check if parent id is in the traced processes
    if (to_discard(parent_pid))
        return 0;

    // add process id to map
    if (bpf_map_update_elem(&pids, &child_pid, &child_pid, BPF_ANY) != 0)
    {
        return 1;
    }

    return create_and_submit_event(parent_pid, MYOP_FORK, SYSCALL);
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
