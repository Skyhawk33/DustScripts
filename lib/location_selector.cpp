
const bool SELECTOR_USE_ZOOM = false;
class LocationSelector {
    private scene@ g;
    private camera@ c;
    private float start_x, start_y;
    private textfield@ tf;
    private controllable@ player;
    private float prev_time_warp;
    private bool is_active = false;
    
    private int consecutive_held_x = 0;
    private int consecutive_held_y = 0;
    private float goal_zoom = 1;
    
    private bool is_finished = false;
    private float selected_x, selected_y;
    
    LocationSelector(scene@ sc, int player_index) {
        @g = sc;
        @c = get_camera(player_index);
        @tf = create_textfield();
        tf.align_horizontal(-1);
        tf.align_vertical(1);
    }
    
    void move_cameras() {
        if(is_active) {
            float amount = 10;
            
            if(player.light_intent() > 0 && player.heavy_intent() > 0) {
                amount = 0.1;
                goal_zoom = 5;
            }
            else if(player.light_intent() > 0) {
                amount = 1;
                goal_zoom = 2;
            }
            else if(player.heavy_intent() > 0) {
                amount = 48;
                goal_zoom = 1;
            }
            else {
                goal_zoom = 1;
            }
            
            if(SELECTOR_USE_ZOOM) {
                if(c.scale_x() - goal_zoom > 0.4) {
                    c.scale_x(c.scale_x()-0.5);
                    c.scale_y(c.scale_y()-0.5);
                }
                if(c.scale_x() - goal_zoom < -0.4) {
                    c.scale_x(c.scale_x()+0.5);
                    c.scale_y(c.scale_y()+0.5);
                }
            }
            
            if(player.x_intent() != 0) {
                consecutive_held_x++;
                if(consecutive_held_x > 10 || consecutive_held_x == 1) {
                    float new_x = c.x() + amount*player.x_intent();
                    if(abs(new_x-player.x()) < 11000)
                        c.x(new_x);
                }
            }
            else
                consecutive_held_x = 0;
            
            if(player.y_intent() != 0) {
                consecutive_held_y++;
                if(consecutive_held_y > 10 || consecutive_held_y == 1) {
                    float new_y = c.y() + amount*player.y_intent();
                    if(abs(new_y-player.y()) < 11000)
                        c.y(new_y);
                }
            }
            else
                consecutive_held_y = 0;
        }
    }
    
    void step(int entities) {
        if(is_active) {
            if(player.jump_intent() == 1)
                finish();
            
            player.x_intent(0);
            player.y_intent(0);
            player.jump_intent(0);
            player.dash_intent(0);
            player.light_intent(0);
            player.heavy_intent(0);
        }
    }
    
    void draw(float sub_frame) {
        if(is_active) {
            float x = int(c.x()*10)/10.0;
            float y = int(c.y()*10)/10.0;
            tf.text("X: "+x+"  ("+x%48+")\nY: "+y+"  ("+y%48+")");
            tf.draw_hud(0, 1, -770, 400, 1, 1, 0);
            
            g.draw_rectangle_world(21,1,x-1,y-24,x+1,y+24,0,0xFFFFFFFF);
            g.draw_rectangle_world(21,1,x-24,y-1,x+24,y+1,0,0xFFFFFFFF);
        }
    }
    
    bool active() {
        return is_active;
    }
    
    bool finished() {
        return is_finished;
    }
    
    float x() {
        return selected_x;
    }
    
    float y() {
        return selected_y;
    }
    
    void start() {
        if(!is_active) {
            prev_time_warp = g.time_warp();
            g.time_warp(0);
            g.disable_score_overlay(true);
            
            c.script_camera(true);
            c.controller_mode(1);
            start_x = c.x();
            start_y = c.y();
            
            @player = controller_controllable(c.player());
            
            is_active = true;
            is_finished = false;
        }
    }
    
    void finish() {
        if(is_active) {
            g.time_warp(prev_time_warp);
            g.disable_score_overlay(false);
            
            selected_x = c.x();
            selected_y = c.y();
            
            c.x(start_x);
            c.y(start_y);
            c.script_camera(false);
            c.controller_mode(0);
            reset_camera(c.player());
            
            is_active = false;
            is_finished = true;
        }
    }
    
    void reset() {
        finish();
        is_finished = false;
        selected_x = 0;
        selected_y = 0;
    }
}

/*
// an example script showing how the location selector can be used
class script {
    scene@ g;
    dustman@ player;
    LocationSelector@ ls;

    example_script() {
        @g = get_scene();
        //if the user tabbed to editor while in location select mode, we need to restart time
        g.time_warp(1);
        @ls = LocationSelector(g, 0);
    }
    
    void move_cameras(){
        ls.move_cameras();
    }

    void step(int entities) {
        if(ls.active())
            ls.step(entities);
        else {
            //if no player is defined, get the player
            if(@player == null){
                controllable@ c = controller_controllable(0);
                if(@c != null){
                    @player = c.as_dustman();
                }
            }
            else {
                if(ls.finished()) {
                    puts("("+ls.x()+", "+ls.y()+")");
                    ls.reset();
                }
                if(player.taunt_intent() == 1) {
                    ls.start();
                }
            }
        }
    }
    
    void draw(float sub_frame) {
        ls.draw(sub_frame);
    }
    
    void checkpoint_load() {
        @player = null;
    }
}
/**/

