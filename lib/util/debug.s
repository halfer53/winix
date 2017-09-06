.global get_pc
get_pc:
	addui $1, $ra, 0
	jr $ra

.global get_sp
get_sp:
	addui $1, $sp, 0
	jr $ra
