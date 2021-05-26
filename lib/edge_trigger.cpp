//class script{}

class edge_trigger : trigger_base {
    bool activated;
    bool active_this_frame;
    controllable@ trigger_entity;
    
    void init(script@ s, scripttrigger@ self) {
        activated = false;
        active_this_frame = false;
    }
    
    void rising_edge(controllable@ e) {
        @trigger_entity = @e;
        // do stuff
    }

    void falling_edge(controllable@ e) {
        @trigger_entity = null;
        // do stuff
    }
    
    void step() {
        if(activated) {
            if(not active_this_frame) {
                activated = false;
                falling_edge(@trigger_entity);
            }
            active_this_frame = false;
        }
    }
    
    void activate(controllable@ e) {
        if(e.player_index() == 0) {
            if(not activated) {
                rising_edge(@e);
                activated = true;
            }
            active_this_frame = true;
        }
    }
}