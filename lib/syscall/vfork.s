
.global vfork
vfork:
	subui $sp, $sp, 14	# 14 = 5 + 9
						# 5 is the function scope stack
						# 9 is sizeof(struct message)
	sw $ra, 4($sp)
	sw $3, 3($sp)
	addi $3, $sp, 5		# $3 points to struct message used for syscall

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
	beqz $1, is_vfork_child		#if reply is 0

is_vfork_parent:
	lw $ra, 4($sp)
	lw $3, 3($sp)
	addui $sp, $sp, 14	# pop the function stack and message
	jr $ra

is_vfork_child:			# in child, we do not modify the stack.
						# stack is only popped when parent returns
	lw $ra, 4($sp)
	lw $3, 3($sp)
	jr $ra

    