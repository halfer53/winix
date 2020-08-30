
.global open
open:
    subui $sp, $sp, 1
    addui $1, $0, 22
    sw $1, 0($sp)
    j wramp_syscall


