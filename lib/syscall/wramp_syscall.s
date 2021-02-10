##
# WRAMP system call assembly snippet.
#
# Revision History:
#  2016-09-19	Paul Monigatti			Original
##

##
# int wramp_syscall(int operation, ...);
#
# Note: The kernel follows WRAMP calling conventions.
#       Parameters are passed on the stack, return value in $1
##
.global wramp_syscall
.global ptr_wramp_syscall

ptr_wramp_syscall:
wramp_syscall:
	syscall
	jr $ra

.global wramp_break
wramp_break:
	break
	jr $ra



