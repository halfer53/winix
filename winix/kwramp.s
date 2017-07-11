##
# Assembly routines for the WRAMP processor.
#
# Revision History:
#  2016-09-19	Paul Monigatti			Original
##

.text

##
# void wramp_set_handler(void (*handler)(int estat));
##
.global wramp_set_handler
wramp_set_handler:
	la $1, handler
	movgs $evec, $1

	lw $1, 0($sp)
	sw $1, handler_address($0)
	jr $ra

##
# void wramp_load_context();
##
.global wramp_load_context
wramp_load_context:
	lw $13, current_proc($0)

	lw $1, pcb_reg13($13)
	movgs $ers, $1

	lw $1, pcb_ear($13)
	movgs $ear, $1
	
	lw $1, pcb_rbase($13)
	movgs $rbase, $1
	
	lw $1, pcb_ptable($13)
	movgs $ptable, $1

	lw $1, pcb_cctrl($13)
	movgs $cctrl, $1
	
	lw $1, pcb_reg1($13)
	lw $2, pcb_reg2($13)
	lw $3, pcb_reg3($13)
	lw $4, pcb_reg4($13)
	lw $5, pcb_reg5($13)
	lw $6, pcb_reg6($13)
	lw $7, pcb_reg7($13)
	lw $8, pcb_reg8($13)
	lw $9, pcb_reg9($13)
	lw $10, pcb_reg10($13)
	lw $11, pcb_reg11($13)
	lw $12, pcb_reg12($13)
	lw $sp, pcb_sp($13)
	lw $ra, pcb_ra($13)
	rfe

################### PRIVATE ###################
handler:	
	#Save Context
	lw $13, current_proc($0)
	beqz $13, handler #lock up - current_task should never be null!

	sw $1, pcb_reg1($13)
	sw $2, pcb_reg2($13)
	sw $3, pcb_reg3($13)
	sw $4, pcb_reg4($13)
	sw $5, pcb_reg5($13)
	sw $6, pcb_reg6($13)
	sw $7, pcb_reg7($13)
	sw $8, pcb_reg8($13)
	sw $9, pcb_reg9($13)
	sw $10, pcb_reg10($13)
	sw $11, pcb_reg11($13)
	sw $12, pcb_reg12($13)
	sw $sp, pcb_sp($13)
	sw $ra, pcb_ra($13)
	
	movsg $1, $ers
	sw $1, pcb_reg13($13)

	movsg $1, $ear
	sw $1, pcb_ear($13)
	
	movsg $1, $rbase
	sw $1, pcb_rbase($13)
	
	movsg $1, $ptable
	sw $1, pcb_ptable($13)
	
	movsg $1, $cctrl
	sw $1, pcb_cctrl($13)
	
	#Switch to exception stack and call C handler
	la $sp, exception_stack
	subui $sp, $sp, 1
	
	movsg $13, $estat
	sw $13, 0($sp)
	lw $13, handler_address($0)
	jalr $13
	
	#Note: the C handler doesn't necessarily return here.
	#It may go direct to wramp_load_context instead.
	j wramp_load_context
	
.data
# Function pointer for the main exception handler
handler_address: .word 0

.bss
# Stack used while handling exceptions.
	.space 200
exception_stack: 

### PROCESS CONTROL BLOCK DEFINITIONS ###
### Note: strongly tied to format of proc_t ###
.equ pcb_reg1, 0
.equ pcb_reg2, 1
.equ pcb_reg3, 2
.equ pcb_reg4, 3
.equ pcb_reg5, 4
.equ pcb_reg6, 5
.equ pcb_reg7, 6
.equ pcb_reg8, 7
.equ pcb_reg9, 8
.equ pcb_reg10, 9
.equ pcb_reg11, 10
.equ pcb_reg12, 11
.equ pcb_reg13, 12
.equ pcb_sp, 13
.equ pcb_ra, 14
.equ pcb_ear, 15
.equ pcb_rbase, 16
.equ pcb_ptable, 17
.equ pcb_cctrl, 18