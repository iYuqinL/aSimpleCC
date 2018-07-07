.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text

read:
subu $sp, $sp, 8
sw $ra, 4($sp)
sw $fp, 0($sp)
addi $fp, $sp, 8
li $v0, 4
la $a0, _prompt
syscall
li $v0, 5
syscall
subu $sp, $fp, 8
lw $ra, 4($sp)
lw $fp, 0($sp)
jr $ra

write:
subu $sp, $sp, 8
sw $ra, 4($sp)
sw $fp, 0($sp)
addi $fp, $sp, 8
li $v0, 1
syscall
li $v0, 4
la $a0, _ret
syscall
subu $sp, $fp, 8
lw $ra, 4($sp)
lw $fp, 0($sp)
move $v0, $0
jr $ra

main:
subu $sp, $sp, 8
sw $ra, 4($sp)
sw $fp, 0($sp)
addi $fp, $sp, 8
subu $sp, $fp, 8
jal read
move $s7, $v0
subu $v1, $fp, 12
sw $s7, 0($v1)
subu $v1, $fp, 12
lw $s7, 0($v1)
li $s6, 0
bgt $s7, $s6, label1
subu $v1, $fp, 12
sw $s7, 0($v1)
j label2
label1:
subu $sp, $fp, 12
li $s7, 1
move $a0, $s7
jal write
j label3
label2:
subu $v1, $fp, 12
lw $s5, 0($v1)
li $s4, 0
blt $s5, $s4, label4
subu $v1, $fp, 12
sw $s5, 0($v1)
j label5
label4:
li $s5, 0
li $s3, 1
sub $s2, $s5, $s3
subu $sp, $fp, 16
move $a0, $s2
jal write
subu $v1, $fp, 16
sw $s2, 0($v1)
j label6
label5:
subu $sp, $fp, 16
li $s2, 0
move $a0, $s2
jal write
label6:
label3:
li $s1, 0
subu $sp, $fp, 8
lw $ra, 4($sp)
lw $fp, 0($sp)
li $v0, 0
jr $ra
