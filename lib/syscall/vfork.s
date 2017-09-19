
.global vfork
vfork:
	subui $sp, $sp, 5
	sw $ra, 4($sp)
	sw $3, 3($sp)
	lw $3, vfork_m($0)
	beqz $3, m_is_null

invoke_vfork:
	addui $1, $0, 4		# 4 is the syscall number for vfork
	sw $1, 1($3)		# vfork_m->type = 4
	lhi	$1, 0x1337		# 0x13370003 is WINIX_SREC
	ori	$1, $1, 0x3
	sw $1, 0($sp)		# WINIX_SENDREC
	sw $0, 1($sp)		# 0, SYSTEM_TASK
	sw $3, 2($sp)		# address of vfork_m
	jal wramp_syscall	# invoke vfork() system call
	lw $1, 1($3)		# load m->reply_res
	beqz $1, is_vfork_child		#if reply is 0

is_vfork_parent:
	sw $3, 0($sp)		# if parent is waken, that indicates 
						# the completion of the whole vfork system call
						# we thus free the allocated struct message
	jal free
	sw $0, vfork_m($0)	# store 0 to message
	lw $ra, 4($sp)
	lw $3, 3($sp)
	addui $sp, $sp, 5
	jr $ra

is_vfork_child:		#in child, we do not modify the stack
					#stack is only popped when parent returns
	lw $ra, 4($sp)
	lw $3, 3($sp)
	jr $ra

m_is_null:
	addi $3, $0, 9	# 9 is sizeof(struct message)
	sw $3, 0($sp)
	jal malloc		# call malloc to allocate the struct message
					# used for initialising a system call
	add $3, $1, $0
	sw $3, vfork_m($0)
	j invoke_vfork


.bss
	vfork_m: .word
    