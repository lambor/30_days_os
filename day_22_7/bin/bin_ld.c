/* Simple linker script for the JOS kernel.
   See the GNU ld 'info' manual ("info ld") to learn the syntax. */

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(bin_entry)

SECTIONS
{
	/* Link the kernel at this address: "." means the current address */
	. = HEADER_SIZE;

	/* AT(...) gives the load address of this section, which tells
	   the boot loader where to load the kernel in physical memory */
	.text :  {
		*(.text .stub .text.* .gnu.linkonce.t.*)
	}
	. = STACK_SIZE;
	.rodata : {
		*(.rodata .rodata.* .gnu.linkonce.r.*)
	}

	.bss : {
		*(.bss)
	}
	
	.data : {
		*(.data)
	}
}
