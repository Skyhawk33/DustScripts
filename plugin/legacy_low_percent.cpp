const int FADE_MAX = 60;

class script : callback_base {
    scene@ g;
    dustman@ player;
    int frame_discarded = 0;
    int discarded = 0;
    int cleaned_save = 0;
    int total_cleaned = 0;
    int fadeout = -40;
    bool is_nexus = true;
    bool level_ended = false;
    bool score_displayed = false;
    string display = "";

    script() {
        @g = get_scene();
    }
    
    void on_level_start() {
        is_nexus = get_nexus_api() !is null;
        level_ended = false;
    }

    void step(int entities) {
        entity@ e;
        if (@player == null) {
            @e = controller_entity(0);
            if (@e != null)
                @player = e.as_dustman();
        }
        if(!level_ended && player !is null) {
            total_cleaned = player.total_cleaned();
            frame_discarded = discarded;
        }
    }
    
    void step_fixed() {
        if(level_ended && fadeout < FADE_MAX)
            fadeout++;
    }
    
    void draw(float sub_frame) {
        if(is_nexus || fadeout >= FADE_MAX)
            return;
        textfield@ txt = create_textfield();
        if(fadeout > 0) {
            int alpha = 0xFF*(FADE_MAX-fadeout)/FADE_MAX*0x1000000;
            txt.colour(0xFFFFFF+alpha);
        }
        txt.set_font("sans_bold", 20);
        txt.text("CLEANED");
        txt.draw_hud(0, 0, -740, 220, 1, 1, -15);
        txt.text(""+(total_cleaned-discarded));
        txt.align_horizontal(0);
        txt.set_font("sans_bold", 36);
        txt.draw_hud(0, 0, -690, 260, 1, 1, 0);
    }
    
    void checkpoint_save() {
        if(player is null)
            return;
        if(player.dead())
            discarded = player.total_cleaned() - cleaned_save; // hitting a checkpoint while dead causes any unsaved dust to be discarded
        else
            cleaned_save = player.total_cleaned() - discarded; // hitting a checkpoint while alive saves any undiscarded dust
    }
    
    void checkpoint_load() {
        @player = null;
    }
    
    void on_level_end() {
        discarded = frame_discarded; // the end trigger respawn itself counts as a checkpoint_save, which needs to be ignored
        if(player !is null && !player.dead())
            total_cleaned = player.total_cleaned(); // if the player was alive during the final attack, it counts
        g.plugin_score(total_cleaned-discarded);
        level_ended = true;
    }
    
    bool plugin_get_score_line() {
        if (score_displayed) return false;
        g.plugin_set_score_line("CLEANED", ""+(total_cleaned-discarded));
        score_displayed = true;
        return true;
    }
}