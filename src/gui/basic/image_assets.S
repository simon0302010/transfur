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
EMBED_IMAGE coffee_input, "assets/coffee_input.bmp"
EMBED_IMAGE coffee_progress_bar_background, "assets/coffee_progress_bar_background.bmp"
EMBED_IMAGE coffee_progress_bar_segment, "assets/coffee_progress_bar_segment.bmp"
EMBED_IMAGE coffee_progress_bar_end, "assets/coffee_progress_bar_end.bmp"
EMBED_IMAGE pastel_background, "assets/pastel_background.bmp"
EMBED_IMAGE pastel_button_disabled, "assets/pastel_button_disabled.bmp"
EMBED_IMAGE pastel_button_failure, "assets/pastel_button_failure.bmp"
EMBED_IMAGE pastel_button_success, "assets/pastel_button_success.bmp"
EMBED_IMAGE pastel_button_primary, "assets/pastel_button_primary.bmp"
EMBED_IMAGE pastel_borders_horizontal, "assets/pastel_borders_horizontal.bmp"
EMBED_IMAGE pastel_borders_vertical, "assets/pastel_borders_vertical.bmp"
EMBED_IMAGE pastel_input, "assets/pastel_input.bmp"
EMBED_IMAGE pastel_progress_bar_background, "assets/pastel_progress_bar_background.bmp"
EMBED_IMAGE pastel_progress_bar_segment, "assets/pastel_progress_bar_segment.bmp"
EMBED_IMAGE pastel_progress_bar_end, "assets/pastel_progress_bar_end.bmp"
EMBED_IMAGE german_background, "assets/german_background.bmp"
EMBED_IMAGE german_button_disabled, "assets/german_button_disabled.bmp"
EMBED_IMAGE german_button_failure, "assets/german_button_failure.bmp"
EMBED_IMAGE german_button_success, "assets/german_button_success.bmp"
EMBED_IMAGE german_button_primary, "assets/german_button_primary.bmp"
EMBED_IMAGE german_borders_horizontal, "assets/german_borders_horizontal.bmp"
EMBED_IMAGE german_borders_vertical, "assets/german_borders_vertical.bmp"
EMBED_IMAGE german_input, "assets/german_input.bmp"
EMBED_IMAGE german_progress_bar_background, "assets/german_progress_bar_background.bmp"
EMBED_IMAGE german_progress_bar_segment, "assets/german_progress_bar_segment.bmp"
EMBED_IMAGE german_progress_bar_end, "assets/german_progress_bar_end.bmp"
EMBED_IMAGE classic_icon, "assets/pastel_icon.bmp"
EMBED_IMAGE coffee_icon, "assets/coffee_icon.bmp"
EMBED_IMAGE pastel_icon, "assets/pastel_icon.bmp"
EMBED_IMAGE german_icon, "assets/german_button_failure.bmp"


#endif