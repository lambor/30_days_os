OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(bin_entry)
SECTIONS
{
 . = 64;
 .text : {
  *(.text .stub .text.* .gnu.linkonce.t.*)
 }
 . = 0x2c00;
 .bss : {
  *(.bss)
 }
 .rodata : {
  *(.rodata .rodata.* .gnu.linkonce.r.*)
 }
 .data : {
  *(.data)
 }
}
