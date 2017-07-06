_set_errno:
    subui $sp, $sp, 1
    sw $1, 0($sp)
    lw $1, 1($sp) #load errno from stack
    sw $1, __errno($0)
    addui $sp, $sp, 2
    jr $ra