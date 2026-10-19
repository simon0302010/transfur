#ifdef ENABLE_THEMES

#if defined(__APPLE__)
.section __TEXT,__const
#else
.section .rodata
#endif
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
EMBED_IMAGE classic_icon, "assets/classic_icon.bmp"
EMBED_IMAGE classic_light_icon, "assets/classic_light_icon.bmp"
EMBED_IMAGE coffee_icon, "assets/coffee_icon.bmp"
EMBED_IMAGE pastel_icon, "assets/pastel_icon.bmp"
EMBED_IMAGE german_icon, "assets/german_button_failure.bmp"
EMBED_IMAGE american_background, "assets/american_background.bmp"
EMBED_IMAGE american_button_disabled, "assets/american_button_disabled.bmp"
EMBED_IMAGE american_button_failure, "assets/american_button_failure.bmp"
EMBED_IMAGE american_button_success, "assets/american_button_success.bmp"
EMBED_IMAGE american_button_primary, "assets/american_button_primary.bmp"
EMBED_IMAGE american_borders_horizontal, "assets/american_borders_horizontal.bmp"
EMBED_IMAGE american_borders_vertical, "assets/american_borders_vertical.bmp"
EMBED_IMAGE american_input, "assets/american_input.bmp"
EMBED_IMAGE american_progress_bar_background, "assets/american_progress_bar_background.bmp"
EMBED_IMAGE american_progress_bar_segment, "assets/american_progress_bar_segment.bmp"
EMBED_IMAGE american_progress_bar_end, "assets/american_progress_bar_end.bmp"
EMBED_IMAGE american_icon, "assets/american_icon.bmp"
EMBED_IMAGE casino_background, "assets/casino_background.bmp"
EMBED_IMAGE casino_button_disabled, "assets/casino_button_disabled.bmp"
EMBED_IMAGE casino_button_failure, "assets/casino_button_failure.bmp"
EMBED_IMAGE casino_button_success, "assets/casino_button_success.bmp"
EMBED_IMAGE casino_button_primary, "assets/casino_button_primary.bmp"
EMBED_IMAGE casino_borders_horizontal, "assets/casino_borders_horizontal.bmp"
EMBED_IMAGE casino_borders_vertical, "assets/casino_borders_vertical.bmp"
EMBED_IMAGE casino_input, "assets/casino_input.bmp"
EMBED_IMAGE casino_progress_bar_background, "assets/casino_progress_bar_background.bmp"
EMBED_IMAGE casino_progress_bar_segment, "assets/casino_progress_bar_segment.bmp"
EMBED_IMAGE casino_progress_bar_end, "assets/casino_progress_bar_end.bmp"
EMBED_IMAGE casino_icon, "assets/casino_icon.bmp"
EMBED_IMAGE catppuccin_frappe_icon, "assets/catppuccin_frappe_icon.bmp"
EMBED_IMAGE catppuccin_latte_icon, "assets/catppuccin_latte_icon.bmp"
EMBED_IMAGE catppuccin_macchiato_icon, "assets/catppuccin_macchiato_icon.bmp"
EMBED_IMAGE catppuccin_mocha_icon, "assets/catppuccin_mocha_icon.bmp"
EMBED_IMAGE gruvbox_dark_icon, "assets/gruvbox_dark_icon.bmp"
EMBED_IMAGE gruvbox_light_icon, "assets/gruvbox_light_icon.bmp"


#endif