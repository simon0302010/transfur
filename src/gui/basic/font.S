#if defined(__APPLE__)
.section __TEXT,__const
#else
.section .rodata
#endif
.global font_bmp_data
.global font_bmp_size
.align 4

font_bmp_data:
    .incbin "font.bmp"
font_bmp_end:

font_bmp_size:
    .long font_bmp_end - font_bmp_data