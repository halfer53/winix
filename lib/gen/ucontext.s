.global getcontext
getcontext:
	subui $sp, $sp, 2
	sw $1, 0($sp)
	sw $2, 1($sp)
	#load ucp's pointer
	lw $1, 2($sp)
	
	#load register 1
	lw $2, 0($sp)
	#save reguster 1
	sw $2, ucontext_reg1($1)
	#load register 2
	lw $2, 1($sp)
	sw $2, ucontext_reg2($1)
	sw $3, ucontext_reg3($1)
	sw $4, ucontext_reg4($1)
	sw $5, ucontext_reg5($1)
	sw $6, ucontext_reg6($1)
	sw $7, ucontext_reg7($1)
	sw $8, ucontext_reg8($1)
	sw $9, ucontext_reg9($1)
	sw $10, ucontext_reg10($1)
	sw $11, ucontext_reg11($1)
	sw $12, ucontext_reg12($1)
	sw $13, ucontext_reg13($1)
	sw $ra, ucontext_pc($1) #$ra is the only register not saved

	addui $sp, $sp, 2
	sw $sp, ucontext_sp($1)

	lw $1, -2($sp)
	lw $2, -1($sp)

	jr $ra

.global setcontext
setcontext:
	#13: pointer to ucp
	lw $13, 0($sp)
	lw $1, ucontext_reg1($13)
	lw $2, ucontext_reg2($13)
	lw $3, ucontext_reg3($13)
	lw $4, ucontext_reg4($13)
	lw $5, ucontext_reg5($13)
	lw $6, ucontext_reg6($13)
	lw $7, ucontext_reg7($13)
	lw $8, ucontext_reg8($13)
	lw $9, ucontext_reg9($13)
	lw $10, ucontext_reg10($13)
	lw $11, ucontext_reg11($13)
	lw $12, ucontext_reg12($13)
	
	#load return address as the pc
	lw $ra, ucontext_pc($13)
	lw $sp, ucontext_sp($13)

	# restore reg 13 from the stack
	lw $13, ucontext_reg13($13)
	jr $ra


.global _ctx_start
_ctx_start:
	# Load func set by makecontext
	lw $2, 0($sp)
	# Load argc set by makecontext
	lw $3, 1($sp)
	addui $sp, $sp, 2
	# call the user context function
	jalr $2
	# after popping the stack, now ucp is sitting 
	# at the top of the stack
	addu $sp, $sp, $3
	# call _ctx_end
	jr $8


.global swapcontext
swapcontext:
	subui $sp, $sp, 1
	sw $1, 0($sp)
	#load ucp's pointer
	lw $1, 1($sp)
	#save register 2
	sw $2, ucontext_reg2($1)
	#load register 1's value into reg 2
	lw $2, 0($sp)
	#save reguster 1's value to ucontext struct
	sw $2, ucontext_reg1($1)
	sw $3, ucontext_reg3($1)
	sw $4, ucontext_reg4($1)
	sw $5, ucontext_reg5($1)
	sw $6, ucontext_reg6($1)
	sw $7, ucontext_reg7($1)
	sw $8, ucontext_reg8($1)
	sw $9, ucontext_reg9($1)
	sw $10, ucontext_reg10($1)
	sw $11, ucontext_reg11($1)
	sw $12, ucontext_reg12($1)
	sw $13, ucontext_reg13($1)
	sw $ra, ucontext_pc($1) #$ra is the only register not saved

	addui $sp, $sp, 1
	sw $sp, ucontext_sp($1)

	addui $sp, $sp, 1
	j setcontext

.equ ucontext_reg1, 0
.equ ucontext_reg2, 1
.equ ucontext_reg3, 2
.equ ucontext_reg4, 3
.equ ucontext_reg5, 4
.equ ucontext_reg6, 5
.equ ucontext_reg7, 6
.equ ucontext_reg8, 7
.equ ucontext_reg9, 8
.equ ucontext_reg10, 9
.equ ucontext_reg11, 10
.equ ucontext_reg12, 11
.equ ucontext_reg13, 12
.equ ucontext_sp, 13
.equ ucontext_ra, 14
.equ ucontext_pc, 15
.equ ucontext_ss_sp, 16
.equ ucontext_ss_flags, 17
.equ ucontext_ss_size, 18