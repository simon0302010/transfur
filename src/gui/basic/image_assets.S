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

EMBED_IMAGE coffee_background, "assets/coffee_background.bmp"
EMBED_IMAGE coffee_button_disabled, "assets/coffee_button_disabled.bmp"
EMBED_IMAGE coffee_button_failure, "assets/coffee_button_failure.bmp"
EMBED_IMAGE coffee_button_success, "assets/coffee_button_success.bmp"
EMBED_IMAGE coffee_button_primary, "assets/coffee_button_primary.bmp"
EMBED_IMAGE coffee_borders_horizontal, "assets/coffee_borders_horizontal.bmp"
EMBED_IMAGE coffee_borders_vertical, "assets/coffee_borders_vertical.bmp"

#endif