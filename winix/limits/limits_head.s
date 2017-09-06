##
# Segment base addresses  for the boot image.
# NOTE: MUST BE THE FIRST FILE PASSED TO THE LINKER
#
# Revision History:
#  2016-09-19	Paul Monigatti			Original
##

.global TEXT_BEGIN
.global DATA_BEGIN
.global BSS_BEGIN

.text
TEXT_BEGIN:		# Start of the text segment

.data
DATA_BEGIN:		# Start of the data segment

.bss
BSS_BEGIN:		# Start of the BSS segment
