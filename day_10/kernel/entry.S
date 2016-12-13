#include <mmu.h>
#include "const.h"
# Start the CPU: switch to 32-bit protected mode, jump into C.
# The BIOS loads this code from the first sector of the hard disk into
# memory at physical address 0x7c00 and starts executing in real mode
# with %cs=0 %ip=7c00.

#define CYLS            0x0ff0          
#define LEDS            0x0ff1
#define VMODE           0x0ff2          
#define SCRNX           0x0ff4          
#define SCRNY           0x0ff6          
#define VRAM            0x0ff8  


#define BOTPAK          0X00280000
#define DSKCAC          0x00100000
#define DSKCAC0         0X00008000 

.set PROT_MODE_CSEG, 0x8         # kernel code segment selector
.set PROT_MODE_DSEG, 0x10        # kernel data segment selector
.set CR0_PE_ON,      0x1         # protected mode enable flag

.globl start
start:
  .code16                     # Assemble for 16-bit mode


  


  # Set up the important data segment registers (DS, ES, SS).
  xorw    %ax,%ax             # Segment number zero
  movw    %ax,%ds             # -> Data Segment
  movw    %ax,%es             # -> Extra Segment
  movw    %ax,%ss             # -> Stack Segment
  
  movb $0x13,%al  # ;vga 320x200x8 位,color mode 
  movb $0x00,%ah
  int $0x10
  
#save color mode in ram 0x0ff0
 movb $10, (CYLS)
 movb $8,(VMODE)
 movw $320,(SCRNX)
 movw $200,(SCRNY)
 movl $0x000a0000,(VRAM)

 #get keyboard led status
 movb	$0x02,%ah 
 int     $0x16			#keyboard interrupts
 movb   %al,(LEDS)
		
		
		
#diplay something
  movw $msg,%si
  call puts
  
  movw $try,%si
  call puts
  
  #disable pic0 int
  movb $0xff,%al
  outb %al,$0x21
  nop
  #disable pic1 int
  out %al,$0xa1
  #disable cpu int
  cli                         # Disable interrupts

  # Enable A20:
  #   For backwards compatibility with the earliest PCs, physical
  #   address line 20 is tied low, so that addresses higher than
  #   1MB wrap around to zero by default.  This code undoes this. 
seta20.1:
  inb     $0x64,%al               # Wait for not busy
  testb   $0x2,%al
  jnz     seta20.1

  movb    $0xd1,%al               # 0xd1 -> port 0x64
  outb    %al,$0x64

seta20.2:
  inb     $0x64,%al               # Wait for not busy
  testb   $02,%al
  jnz     seta20.2

  movb    $0xdf,%al               # 0xdf -> port 0x60
  outb    %al,$0x60

  # Switch from real to protected mode, using a bootstrap GDT       this is vip ,but i don`t know it clearly now
  # and segment translation that makes virtual addresses 
  # identical to their physical addresses, so that the 
  # effective memory map does not change during the switch.
  lgdt    gdtdesc
  movl    %cr0, %eax
  andl    $0x7fffffff, %eax
  orl     $CR0_PE_ON, %eax
  movl    %eax, %cr0
  
  # Jump to next instruction, but in 32-bit code segment.
  # Switches processor into 32-bit mode.
  ljmp    $(2*8), $protcseg
  msg:
  .asciz "\r\n\n\rmy kernel is runing jos"
  
 try:
  .asciz "\r\n\n\rtry it again"

puts:

	movb (%si),%al
	add $1,%si
	cmp $0,%al
	je over
	movb $0x0e,%ah
	movw $15,%bx
	int $0x10
	jmp puts
over:
	ret	
	
	
  .code32                     # Assemble for 32-bit mode
protcseg:
  # Set up the protected-mode data segment registers
  movw    $(1*8), %ax    		  # Our data segment selector
  movw    %ax, %ds                # -> DS: Data Segment
  movw    %ax, %es                # -> ES: Extra Segment
  movw    %ax, %fs                # -> FS
  movw    %ax, %gs                # -> GS
  movw    %ax, %ss                # -> SS: Stack Segment
  
  movl 	  $0x3fffff, %esp

  # memcpy(bootpack,BOTPAK,512*1024/4)
  # memcpy(0x7C00,DSKCAC,512/4)
  # memcpy(DSKCAC0+512,DSKCAC+512,cyls*512*18*2/4 - 512/4)
  movl $main, %esi
  movl $BOTPAK, %edi
  movl $(512*1024/4),%ecx
  call memcpy

  movl $0x7c00, %esi
  movl $DSKCAC, %edi
  movl $(512/4), %ecx
  call memcpy

  movl $(DSKCAC0+512),%esi
  movl $(DSKCAC+512),%edi
  movl $0,%ecx
  movb (CYLS), %cl
  imul $(512*18*2/4), %ecx
  subl $(512/4), %ecx
  call memcpy 

  ljmp $(3*8),$0 #goto bootmain //the third segment descriptor

  # If bootmain returns (it shouldn't), loop.
spin:
  jmp spin

memcpy:
	movl (%esi),%eax
	addl $4,%esi
	movl %eax,(%edi)
	addl $4,%edi
	subl $1,%ecx
	jnz memcpy
	ret


# Bootstrap GDT
# bootmain will rebuild the GDT, but the new gdt must be similiar with bootstrap gdt.
.p2align 2                                # force 4 byte alignment
gdt:
  SEG_NULL				# null seg
  #SEG(STA_X|STA_R, 0x0, 0xffffffff)	# code seg
  #SEG(STA_W, 0x0, 0xffffffff)	        # data seg
  SEG(STA_R,       0x0,      0xffffffff)	#data seg
  SEG(STA_X|STA_R, 0X0, 	 0xffffffff)	#entry code seg
  SEG(STA_X|STA_R, 0x280000, 0x0007ffff)	#c code seg

gdtdesc:
  .word   31                              # sizeof(gdt) - 1 = 4 * 8 - 1
  .long   gdt                             # address gdt

main:
#the bootmain cobj follows the tail of entry.
