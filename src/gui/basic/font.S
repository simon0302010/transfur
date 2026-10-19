#if defined(__APPLE__)
.section __TEXT,__const
.global _font_bmp_data
.global _font_bmp_size
.align 4

_font_bmp_data:
    .incbin "font.bmp"
_font_bmp_end:

_font_bmp_size:
    .long _font_bmp_end - _font_bmp_data
#else
.section .rodata
.global font_bmp_data
.global font_bmp_size
.align 4

font_bmp_data:
    .incbin "font.bmp"
font_bmp_end:

font_bmp_size:
    .long font_bmp_end - font_bmp_data
#endif