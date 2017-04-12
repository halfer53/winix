##
# Segment limits for the boot image.
# NOTE: MUST BE THE FINAL FILE PASSED TO THE LINKER
#
# Revision History:
#  2016-09-19	Paul Monigatti			Original
##

.global TEXT_END
.global DATA_END
.global BSS_END

.text
TEXT_END:			# The final word of the boot image's text segment
.word 0xdeadbeef

.data				# The final word of the boot image's data segment
DATA_END:
.word 0x0badf00d

.bss				# The final word of the boot image's BSS segment
BSS_END:
.word
