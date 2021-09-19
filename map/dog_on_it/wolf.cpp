//const int jump_boost = -420;
//const int wolf_jump_state = 7;

class script {
    scene@ g;
    controllable@ player;
    controllable@ old_player;
    
    bool show_debug = false; 
    bool needs_init = true;
    

    script() {
        @g = get_scene();
    }
    
    void on_level_start() {
        @player = controller_controllable(0);
    }
    
    void on_level_end() {
        g.remove_entity(player.as_entity());
    }
    
    void step(int entities) {
        if(needs_init) {
            if(show_debug)
                puts("running!");
            @old_player = controller_controllable(0);
            
            @player = create_entity("enemy_wolf").as_controllable();
            player.x(old_player.x());
            player.y(old_player.y());
            player.team(old_player.team());
            g.add_entity(player.as_entity());
            controller_entity(0,@player);
            
            g.remove_entity(old_player.as_entity());
            needs_init = false;
        }
        
        //if(prev_state != player.state()) {
        //    if(player.state() == wolf_jump_state && player.y_intent() == -1) {
        //        player.set_speed_xy(player.x_speed(), player.y_speed() + jump_boost);
        //    }
        //    prev_state = player.state();
        //}
        
        if(show_debug) {
            puts("{Vx:"+player.as_controllable().x_speed()+", Vy:"+player.as_controllable().y_speed()+" V:"+player.as_controllable().speed()+"}");
            puts("{X:"+player.x()+", Y:"+player.y()+"}");
            puts("State: "+player.as_controllable().state());
        }
    }
}