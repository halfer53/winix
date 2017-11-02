
.global vfork
vfork:
	subui $sp, $sp, 3	# 3 is the function scope stack
	la $1, vfork_stack		# store ra and $3
	sw $ra, 0($1)
	sw $3, 1($1)	
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
	lw $1, 1($3)		# load mesg->reply_res

	la $3, _pid
	sw $0, 0($3)		# invalidate pid cache

do_return:
	la $3, vfork_stack		# restore ra and $3
	lw $ra, 0($3)
	lw $3, 1($3)
	addui $sp, $sp, 3	# pop the function stack and message
	jr $ra

#private
.bss
mesg:
	.space	8			# note this conforms strickly to sizeof(struct message)
vfork_stack:
	.space 2
.extern	_pid 1
