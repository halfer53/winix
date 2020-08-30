

.global get_sigreturn_func_ptr
.text
get_sigreturn_func_ptr:
	la	$1, __sigreturn
	jr $ra

.global	__sigreturn
.text
__sigreturn:
	# subui $sp, $sp, 11	# 12 = 3 + 8
	# 					# 3 is the parameters for syscall
	# 					# 9 is sizeof(struct message)
	# addi $3, $sp, 3		# $3 points to struct message used for syscall

	# addi $1, $0, 9		# 9 is the syscall number for sigreturn
	# sw $1, 1($3)		# mesg->type = 9
	# lw $1, 11($sp)
	# sw $1, 2($3)		# mesg->m1_i1 = signum
	# lhi	$1, 0x1337		# 0x13370003 is WINIX_SREC
	# ori	$1, $1, 0x3
	# sw $1, 0($sp)		# WINIX_SENDREC
	# sw $0, 1($sp)		# 0, SYSTEM_TASK
	# sw $3, 2($sp)		# address of mesg
	# jal wramp_syscall	# invoke vfork() system call

	addui $sp, $sp, 1
	syscall
	# Hopefull never get here
	# addui $1, $0, 2
	# sw $1, 0($1)
	# sw $1, 1($1)
	# j wramp_syscall

.extern	_pid 1
.extern	stderr 1
.extern	stdout 1
.extern	stdin 1
.extern	__iotab 20
.extern	__errno 1
