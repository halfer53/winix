
.global open
open:
    subui $sp, $sp, 1
    addui $1, $0, 22
    sw $1, 0($sp)
    j wramp_syscall

.global ioctl
ioctl:
    subui $sp, $sp, 1
    addui $1, $0, 45
    sw $1, 0($sp)
    j wramp_syscall

.global fcntl
fcntl:
    subui $sp, $sp, 1
    addui $1, $0, 44
    sw $1, 0($sp)
    j wramp_syscall


