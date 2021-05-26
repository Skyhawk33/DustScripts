class script {
    scene@ g;
    
    [entity] int dm_fog;
    [entity] int dg_fog;
    [entity] int dk_fog;
    [entity] int dw_fog;

    script() {
        @g = get_scene();
    }
    
    void select_fog(int fog_id, string fog_character, string player_character, float new_x, float new_y) {
        entity@ fog = entity_by_id(fog_id);
        if(@fog != null) {
            if(player_character == fog_character || player_character == "v"+fog_character)
                fog.set_xy(new_x, new_y);
            else
                g.remove_entity(@fog);
        }
    }
    
    void on_level_start() {
        entity@ e;
        camera@ cam = get_camera(0);
        controllable@ c = controller_controllable(0);
        dustman@ player;
        string character = "";
        
        if(@c != null) {
            @player = c.as_dustman();
            if(@player != null) {
                character = player.character();
            }
        }
        
        select_fog(dm_fog, "dustman", character, cam.x(), cam.y());
        select_fog(dg_fog, "dustgirl", character, cam.x(), cam.y());
        select_fog(dk_fog, "dustkid", character, cam.x(), cam.y());
        select_fog(dw_fog, "dustworth", character, cam.x(), cam.y());
    }
    
    void draw_bordered_text(textfield@ tf, int fog_id, string text, uint color) {
        entity@ e = entity_by_id(fog_id);
        if(@e != null) {
            tf.text(text);
            tf.colour(color);
            tf.draw_world(22, 1, e.x(), e.y(), 1, 1, 0);
            
            tf.colour(0xFFFFFFFF);
            tf.draw_world(22, 0, e.x()-1, e.y()-1, 1, 1, 0);
            tf.draw_world(22, 0, e.x()-1, e.y()+1, 1, 1, 0);
            tf.draw_world(22, 0, e.x()+1, e.y()-1, 1, 1, 0);
            tf.draw_world(22, 0, e.x()+1, e.y()+1, 1, 1, 0);
        }
    }
    
    void editor_draw(float subframe) {
        textfield@ tf = create_textfield();
        
        tf.align_horizontal(0);
        tf.align_vertical(1);
        
        draw_bordered_text(@tf, dm_fog, "DM", 0xFF101090);
        draw_bordered_text(@tf, dg_fog, "DG", 0xFF800000);
        draw_bordered_text(@tf, dk_fog, "DK", 0xFF400080);
        draw_bordered_text(@tf, dw_fog, "DW", 0xFF008000);
    }
}