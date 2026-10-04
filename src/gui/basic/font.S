.section .rodata
.global font_bmp_data
.global font_bmp_size
.align 4

font_bmp_data:
    .incbin "font.bmp"
font_bmp_end:

font_bmp_size:
    .long font_bmp_end - font_bmp_data