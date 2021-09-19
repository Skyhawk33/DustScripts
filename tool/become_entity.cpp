class script {
    scene@ g;
    [entity] int entity_id = 0;
    entity@ player;
    entity@ old_player;
    
    [boolean] bool keep_old_player = true; 
    [boolean] bool show_debug = false; 
    bool needs_init = true;
    

    script() {
        @g = get_scene();
    }
    
    void on_level_start() {
        if(entity_id == 0) {
            needs_init = false;
        }
    }
    
    void step(int entities) {
        if(needs_init) {
            if(show_debug)
                puts("running!");
            @old_player = controller_entity(0);
            @player = entity_by_id(entity_id);
            controller_entity(0,player.as_controllable());
            if(!keep_old_player)
                g.remove_entity(old_player);
            needs_init = false;
        }
        if(show_debug) {
            puts("{Vx:"+player.as_controllable().x_speed()+", Vy:"+player.as_controllable().y_speed()+" V:"+player.as_controllable().speed()+"}");
        }
    }
    
    void entity_on_remove(entity@ e) {
        if(e.is_same(player)) {
            if(show_debug)
                puts("Removing player!");
            if(keep_old_player)
                controller_entity(0, old_player.as_controllable());
        }
    }
}