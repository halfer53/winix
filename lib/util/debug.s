.global get_pc
get_pc:
	addui $1, $ra, 0
	jr $ra

.global get_sp
get_sp:
	addui $1, $sp, 0
	jr $ra

.global _break
_break:
	break
	jr $ra

.global fcn_to_ptr
fcn_to_ptr:
	lw $1, 0($sp)
	jr $ra
