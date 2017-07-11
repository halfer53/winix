.global get_curr_addr
get_curr_addr:
	subui $sp, $sp, 1
	sw $1, 0($sp)
	lw $1, 1($sp)
	sw $ra, 0($1)
	lw $1, 0($sp)
	addui $sp, $sp, 1
	jr $ra