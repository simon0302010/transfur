#ifdef ENABLE_THEMES

.section .rodata
.align 4

/* any BMP */
.macro EMBED_IMAGE name, file
.global \name\()_bmp_data
.global \name\()_bmp_size
\name\()_bmp_data:
    .incbin "\file"
\name\()_bmp_end:
\name\()_bmp_size:
    .long \name\()_bmp_end - \name\()_bmp_data
.endm

EMBED_IMAGE test_font, "font.bmp"

#endif