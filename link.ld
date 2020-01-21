ENTRY(_begin)

_virtual_offset = 0xffff000000000000;

MEMORY
{
    pre_mmu : org = 0x80000, len = 1M
    post_mmu : org = _virtual_offset, len = 4096M  /*Basically no limit*/
}

SECTIONS
{
    .pre_mmu : {
        head.so (.text* .data* .bss* .rodata*)
        pre_mmu.co (.text* .data* .bss* .rodata*)
    } >pre_mmu

    /*TODO: Add randomization*/
    _p_post_mmu_alignment = ALIGN(4k);

    .text (_p_post_mmu_alignment + _virtual_offset) : AT(_p_post_mmu_alignment) {
        *(.text .text.* .gnu.linkonce.t*)    
    } >post_mmu

    .rodata : { 
        *(.rodata .rodata.* .gnu.linkonce.r*) 
    } >post_mmu

    .data : {
        *(.data .data.* .gnu.linkonce.d*)
    } >post_mmu

    .bss (NOLOAD) : {
        . = ALIGN(16);
        _v_bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(16);
        _v_bss_end = .;
    } >post_mmu

    . = ALIGN(16);
    _v_bin_end = .;

    _p_bin_end = _v_bin_end - _virtual_offset;  /*For initial stack*/
    _p_bss_start = _v_bss_start - _virtual_offset;
    _p_bss_end = _v_bin_end - _virtual_offset;

    /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
