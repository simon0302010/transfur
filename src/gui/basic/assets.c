#if defined(OS_MACOS)
#define ASM_PREFIX "_"
#define ASM_SEC "__TEXT,__const"
#else
#define ASM_PREFIX ""
#define ASM_SEC ".rodata"
#endif

extern const unsigned char font_bmp_data[];
extern const unsigned int font_bmp_size;

__asm__(".section " ASM_SEC "\n"
        ".balign 4\n"
        ".globl " ASM_PREFIX "font_bmp_data\n"
        ".globl " ASM_PREFIX "font_bmp_size\n"
        ASM_PREFIX "font_bmp_data:\n"
        ".incbin \"font.bmp\"\n"
        ASM_PREFIX "font_bmp_end:\n"
        ASM_PREFIX "font_bmp_size:\n"
        ".long " ASM_PREFIX "font_bmp_end - " ASM_PREFIX "font_bmp_data\n");
        
#ifdef ENABLE_THEMES

#define EMBED(name, file)                                                       \
        ".globl " ASM_PREFIX #name "_bmp_data\n"                                \
        ".globl " ASM_PREFIX #name "_bmp_size\n"                                \
        ASM_PREFIX #name "_bmp_data:\n"                                         \
        ".incbin \"" file "\"\n"                                                \
        ASM_PREFIX #name "_bmp_end:\n"                                          \
        ASM_PREFIX #name "_bmp_size:\n"                                         \
        ".long " ASM_PREFIX #name "_bmp_end - " ASM_PREFIX #name "_bmp_data\n"

__asm__(".section " ASM_SEC "\n" 
        ".balign 4\n"
        EMBED(coffee_background, "assets/coffee_background.bmp")
        EMBED(coffee_button_primary, "assets/coffee_button_primary.bmp")
        EMBED(coffee_button_disabled, "assets/coffee_button_disabled.bmp")
        EMBED(coffee_button_success, "assets/coffee_button_success.bmp")
        EMBED(coffee_button_failure, "assets/coffee_button_failure.bmp")
        EMBED(coffee_borders_horizontal, "assets/coffee_borders_horizontal.bmp")
        EMBED(coffee_borders_vertical, "assets/coffee_borders_vertical.bmp")
        EMBED(coffee_input, "assets/coffee_input.bmp")
        EMBED(coffee_progress_bar_background, "assets/coffee_progress_bar_background.bmp")
        EMBED(coffee_progress_bar_segment, "assets/coffee_progress_bar_segment.bmp")
        EMBED(coffee_progress_bar_end, "assets/coffee_progress_bar_end.bmp")
        EMBED(pastel_background, "assets/pastel_background.bmp")
        EMBED(pastel_button_primary, "assets/pastel_button_primary.bmp")
        EMBED(pastel_button_disabled, "assets/pastel_button_disabled.bmp")
        EMBED(pastel_button_success, "assets/pastel_button_success.bmp")
        EMBED(pastel_button_failure, "assets/pastel_button_failure.bmp")
        EMBED(pastel_borders_horizontal, "assets/pastel_borders_horizontal.bmp")
        EMBED(pastel_borders_vertical, "assets/pastel_borders_vertical.bmp")
        EMBED(pastel_input, "assets/pastel_input.bmp")
        EMBED(pastel_progress_bar_background, "assets/pastel_progress_bar_background.bmp")
        EMBED(pastel_progress_bar_segment, "assets/pastel_progress_bar_segment.bmp")
        EMBED(pastel_progress_bar_end, "assets/pastel_progress_bar_end.bmp")
        EMBED(german_background, "assets/german_background.bmp")
        EMBED(german_button_primary, "assets/german_button_primary.bmp")
        EMBED(german_button_disabled, "assets/german_button_disabled.bmp")
        EMBED(german_button_success, "assets/german_button_success.bmp")
        EMBED(german_button_failure, "assets/german_button_failure.bmp")
        EMBED(german_borders_horizontal, "assets/german_borders_horizontal.bmp")
        EMBED(german_borders_vertical, "assets/german_borders_vertical.bmp")
        EMBED(german_progress_bar_background, "assets/german_progress_bar_background.bmp")
        EMBED(german_progress_bar_segment, "assets/german_progress_bar_segment.bmp")
        EMBED(german_progress_bar_end, "assets/german_progress_bar_end.bmp")
        EMBED(german_input, "assets/german_input.bmp")
        EMBED(classic_icon, "assets/classic_icon.bmp")
        EMBED(classic_light_icon, "assets/classic_light_icon.bmp")
        EMBED(coffee_icon, "assets/coffee_icon.bmp")
        EMBED(pastel_icon, "assets/pastel_icon.bmp")
        EMBED(german_icon, "assets/german_button_failure.bmp")
        EMBED(american_background, "assets/american_background.bmp")
        EMBED(american_button_primary, "assets/american_button_primary.bmp")
        EMBED(american_button_disabled, "assets/american_button_disabled.bmp")
        EMBED(american_button_success, "assets/american_button_success.bmp")
        EMBED(american_button_failure, "assets/american_button_failure.bmp")
        EMBED(american_borders_horizontal, "assets/american_borders_horizontal.bmp")
        EMBED(american_borders_vertical, "assets/american_borders_vertical.bmp")
        EMBED(american_input, "assets/american_input.bmp")
        EMBED(american_progress_bar_background, "assets/american_progress_bar_background.bmp")
        EMBED(american_progress_bar_segment, "assets/american_progress_bar_segment.bmp")
        EMBED(american_progress_bar_end, "assets/american_progress_bar_end.bmp")
        EMBED(american_icon, "assets/american_icon.bmp")
        EMBED(casino_background, "assets/casino_background.bmp")
        EMBED(casino_button_primary, "assets/casino_button_primary.bmp")
        EMBED(casino_button_disabled, "assets/casino_button_disabled.bmp")
        EMBED(casino_button_success, "assets/casino_button_success.bmp")
        EMBED(casino_button_failure, "assets/casino_button_failure.bmp")
        EMBED(casino_borders_horizontal, "assets/casino_borders_horizontal.bmp")
        EMBED(casino_borders_vertical, "assets/casino_borders_vertical.bmp")
        EMBED(casino_input, "assets/casino_input.bmp")
        EMBED(casino_progress_bar_background, "assets/casino_progress_bar_background.bmp")
        EMBED(casino_progress_bar_segment, "assets/casino_progress_bar_segment.bmp")
        EMBED(casino_progress_bar_end, "assets/casino_progress_bar_end.bmp")
        EMBED(casino_icon, "assets/casino_icon.bmp")
        EMBED(catppuccin_frappe_icon, "assets/catppuccin_frappe_icon.bmp")
        EMBED(catppuccin_latte_icon, "assets/catppuccin_latte_icon.bmp")
        EMBED(catppuccin_macchiato_icon, "assets/catppuccin_macchiato_icon.bmp")
        EMBED(catppuccin_mocha_icon, "assets/catppuccin_mocha_icon.bmp")        
        EMBED(gruvbox_dark_icon, "assets/gruvbox_dark_icon.bmp")
        EMBED(gruvbox_light_icon, "assets/gruvbox_light_icon.bmp")
        EMBED(textures_strip, "assets/textures_strip.bmp"));

#undef EMBED

#endif