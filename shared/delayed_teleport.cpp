class script {
    [text|tooltip:"The number of frames the player can leave the trigger\nwithout resetting the countdown"]
    int countdown_buffer = 6;
}

class DelayedTeleport : trigger_base, callback_base
{
    scene@ g;
    scripttrigger@ self;
    [text] int delay_frames = 0;
    [position,mode:world,layer:19,y:y] float x;
    [hidden] float y;
    [text] bool reset_cam = false;
    
    int counter = 0;
    int countdown_buffer = 0;
    bool activated;
    int active_cooldown;
    controllable@ trigger_entity;
    
    DelayedTeleport() {
        @g = get_scene();
    }
    
    void init(script@ s, scripttrigger@ self) {
        activated = false;
        active_cooldown = 0;
        @this.self = self;
        this.countdown_buffer = s.countdown_buffer;
    }
    
    void rising_edge(controllable@ e) {
        @trigger_entity = @e;
    }

    void falling_edge(controllable@ e) {
        @trigger_entity = null;
        counter = 0;
    }
    
    void teleport(controllable@ e) {
        counter = 0;
        e.set_xy(x, y);
        if(reset_cam)
            reset_camera(e.player_index());
    }
    
    void step() {
        if(activated) {
            if(active_cooldown == 0) {
                activated = false;
                falling_edge(@trigger_entity);
            } else {
                counter++;
                if(counter >= delay_frames)
                    teleport(@trigger_entity);
            }
            active_cooldown -= 1;
        }
    }
    
    void activate(controllable@ e) {
        if(e.player_index() >= 0) {
            if(not activated) {
                rising_edge(@e);
                activated = true;
            }
            active_cooldown = countdown_buffer;
        }
    }

    void editor_draw(float)
    {
        if(x == 0 && y == 0)
            return;

        g.draw_line_world(22, 0, self.x(), self.y(), x, y, 4, 0xFFFF0000);
    }
}