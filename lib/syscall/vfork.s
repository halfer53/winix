
.global __vfork
__vfork:
	subui $sp, $sp, 4	# 3 is the function scope stack
	sw $ra, 3($sp)

	addui $1, $0, 3		# Call sbrk
	sw $1, 0($sp)
	jal sbrk 
	addu $4, $1, $0		# $4 points to alternative stack

	lw $ra, 3($sp)
	sw $ra, 0($4)
	sw $3, 1($4)
	sw $4, 2($4)
	la $3, mesg			# $3 points to struct message used for syscall

invoke_vfork:
	addui $1, $0, 4		# 4 is the syscall number for vfork
	sw $1, 1($3)		# mesg->type = 4
	lhi	$1, 0x1337		# 0x13370003 is WINIX_SREC
	ori	$1, $1, 0x3
	sw $1, 0($sp)		# WINIX_SENDREC
	sw $0, 1($sp)		# 0, SYSTEM_TASK
	sw $3, 2($sp)		# address of mesg
	jal wramp_syscall	# invoke vfork() system call
	# lw $1, 1($3)		# load mesg->reply_res

	# la $3, _pid
	# sw $0, 0($3)		# invalidate pid cache

do_return:
	lw $ra, 0($4)
	lw $3, 1($4)
	lw $4, 2($4)
	addui $sp, $sp, 4	# pop the function stack and message
	jr $ra

#private
.bss
mesg:
	.space	8			# note this conforms strickly to sizeof(struct message)
vfork_stack:
	.space 2
.extern	_pid 1
