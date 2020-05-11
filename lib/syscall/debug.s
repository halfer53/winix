
.global get_sp
get_sp:
	addui $1, $sp, 0
	jr $ra

.global get_ra
get_ra:
	addui $1, $ra, 0
	jr $ra


