#include '../lib/cloneable_trigger_base.cpp'

const int NUM_LAYERS = 21;
const int MIN_DUSTBLOCK_LAYER = 6; //dustblocks can not be placed on background layers
const int MAX_SUPER_DISTANCE = 15; //dustblocks can not be cleared further than this distance from the player while supering
const int MAX_ATTACK_DISTANCE = 9; //dustblocks can not be cleared further than this distance from the player while light/heavy attacking
const int MAX_PASSIVE_DISTANCE = 5; //dustblocks can not be cleared further than this distance from the player while not attacking

//the modes used by the layer properties to determine what kinds of blocks to remove
const int IGNORE=0, REMOVE_ALL=1, REMOVE_DUSTBLOCKS=2;

array<layer_properties> initialize_layers(array<layer_properties> layers) {
    // make sure that there is the correct number of layers
    while(layers.length() > NUM_LAYERS) {
        layers.removeLast();
    }
    for(int i=layers.length(); i<NUM_LAYERS; i++) {
        layers.insertLast(layer_properties(REMOVE_DUSTBLOCKS));
        //puts("layer "+i+" mode: "+layers[i].mode);
    }
    
    // some layers cannot have blocks on them, and dont need to be checked
    for(int i=0; i<MIN_DUSTBLOCK_LAYER; i++) {
        layers[i].mode = IGNORE;
    }
    layers[18].mode = IGNORE;
    // layer 19 must be ignored to preserve game mechanics (invisible dustblocks)
    layers[19].mode = IGNORE;
    
    return layers;
}

class script{
    scene@ g;
    dustman@ player;
    hitbox@ attack;
    [option, 0:Ignore, 1:All, 2:Dustblocks] int apply_all = -1;
    [text] array<layer_properties> layers;
    bool editor_initialized;
    
    array<layer_properties> checkpoint_layers;
    array<layer_properties> active_layers;
    
    //the bounding box of the previous recorded dustblocks
    int record_min_x, record_min_y, record_distance;
    //the dustblock locations within the bounding box
    array<array<int>> dustblock_pos(0);
    
    int dustblocks;
    int dustblocks_previous;
    int _; //dummy variable for storing unwanted return values from get_filth_remaining

    script() {
        @g = get_scene();
        editor_initialized = false;
        @attack = null;
        record_min_x = 0;
        record_min_y = 0;
        record_distance = 0;
        
        // this initialization happens before user-saved values are applied,
        // so it is only used when the script is first added
        layers = initialize_layers(layers);
    }
    
    void set_layer_properties(array<layer_properties> properties) {
        active_layers = properties;
    }
    
    void editor_step() {
        // make sure the layer array is set up correctly
        // this cant be done in the constructor because the user-saved values overwrite anything created then.
        if(not editor_initialized) {
            layers = initialize_layers(layers);
            set_layer_properties(layers);
            editor_initialized = true;
        }
        if(apply_all >= 0) {
            puts("Applying mode "+apply_all+" to all available layers");
            for(int i=MIN_DUSTBLOCK_LAYER; i<NUM_LAYERS; i++) {
                if(i == 19)
                    continue;
                layers[i].mode = apply_all;
            }
            apply_all = -1;
        }
    }
    
    void on_level_start() {
        //emergency initialization in case the map was saved when it had the wrong number of layers. see editor_step
        layers = initialize_layers(layers);
        set_layer_properties(layers);
        g.get_filth_remaining(_, dustblocks_previous, _);
    }
    
    void checkpoint_save() {
        checkpoint_layers = active_layers;
    }
    
    void checkpoint_load() {
        set_layer_properties(checkpoint_layers);
        @player = null;
        g.get_filth_remaining(_, dustblocks_previous, _);
    }
    
    void step(int entities) {
        //if no player is defined, get the player
        if(@player == null){
            controllable@ c = controller_controllable(0);
            if(@c != null){
                @player = c.as_dustman();
            }
        }
        //puts(""+player.attack_state());
        if(player.attack_state() == 3) {
            double freeze_timer = player.freeze_frame_timer();
            if(0 < freeze_timer && freeze_timer < 0.3) {
                record_dustblocks(int(player.x()/48), int(player.y()/48), MAX_SUPER_DISTANCE);
            }
            else {
                record_dustblocks(int(player.x()/48), int(player.y()/48)-1, MAX_PASSIVE_DISTANCE);
            }
        }
        else if(player.attack_state() != 0) {
            // if we dont have the attack, or have an outdated attack, re-query
            if (attack is null || attack.hit_outcome() == 4) {
                @attack = player.hitbox();
            }
            double time_since_active = player.attack_timer()-attack.activate_time();
            if(-attack.timer_speed()/20 <= time_since_active && time_since_active <= attack.timer_speed()/20) {
                record_dustblocks(int(player.x()/48), int(player.y()/48)-1, MAX_ATTACK_DISTANCE);
            }
            else {
                record_dustblocks(int(player.x()/48), int(player.y()/48)-1, MAX_PASSIVE_DISTANCE);
            }
        }
        else {
            record_dustblocks(int(player.x()/48), int(player.y()/48)-1, MAX_PASSIVE_DISTANCE);
        }
    }
    
    void step_post(int entities) {
        // if the number of dustblocks in the level has decreased this frame, some have been cleared, and we need to remove any on other layers
        g.get_filth_remaining(_, dustblocks, _);
        if(dustblocks < dustblocks_previous) {
            //puts("removing dustblocks!");
            clear_dustblock_layers(dustblocks_previous-dustblocks);
            g.get_filth_remaining(_, dustblocks, _);
        }
        dustblocks_previous = dustblocks;
    }
    
    void record_dustblocks(int center_x, int center_y, int distance) {
        //get the bounding rectangle of the recording
        int min_x = center_x-distance, min_y= center_y-distance;
        
        //if the existing recording already covers this area, we dont need to re-record
        if(record_min_x <= min_x && min_x + 2*distance <= record_min_x + 2*record_distance && record_min_y <= min_y && min_y + 2*distance <= record_min_y + 2*record_distance)
            return;
        
        // otherwise, we need to make a new recording
        record_min_x = min_x;
        record_min_y = min_y;
        record_distance = distance;
        //puts("recording dustblocks! distance:"+distance);
        
        dustblock_pos.resize(0);
        for(int x = min_x; x <= min_x+2*distance; x++) {
            for(int y = min_y; y <= min_y+2*distance; y++) {
                if(g.get_tile(x, y).is_dustblock()) {
                    dustblock_pos.insertLast(array<int> = {x, y});
                }
            }
        }
    }
    
    void clear_dustblock_layers(int expected_dustblocks) {
        uint i=0;
        while(i < dustblock_pos.length) {
            int x=dustblock_pos[i][0], y=dustblock_pos[i][1];
            if(!g.get_tile(x, y).is_dustblock()) {
                //the dustblock was cleared. remove any other layers and remove the position from the list
                for(int l=MIN_DUSTBLOCK_LAYER; l<NUM_LAYERS; l++) {
                    if(active_layers[l].mode == IGNORE)
                        continue;
                    else if((active_layers[l].mode == REMOVE_ALL) ||
                            (active_layers[l].mode == REMOVE_DUSTBLOCKS && g.get_tile(x, y, l).is_dustblock()))
                    {
                        g.set_tile(x, y, l, false, 0, 0, 0, 0);
                    }
                }
                dustblock_pos.removeAt(i);
                expected_dustblocks--;
                if(expected_dustblocks <= 0)
                    return;
            }
            else {
                //the dustblock was not cleared. move on
                i++;
            }
        }
        puts("Could not find all missing dustblocks. "+ expected_dustblocks+ " unaccounted for.");
    }
}

class layer_dust_trigger : cloneable_trigger_base {
    scripttrigger@ self;
    script@ s;
    bool activated;
    bool active_this_frame;
    
    [option, 0:Ignore, 1:All, 2:Dustblocks] int apply_all = -1;
    [text] array<layer_properties> layers;
    
    void init(script@ s, scripttrigger@ self) {
        @this.s = @s;
        @this.self = @self;
        activated = false;
        active_this_frame = false;
        layers = initialize_layers(layers);
    }
    
    void trigger() {
        s.set_layer_properties(layers);
    }
    
    void step() {
        if(activated) {
            if(not active_this_frame) {
                activated = false;
            }
            active_this_frame = false;
        }
    }
    
    void editor_step() {
        cloneable_trigger_base::editor_step();
        
        if(apply_all >= 0) {
            puts("Applying mode "+apply_all+" to all available layers");
            for(int i=MIN_DUSTBLOCK_LAYER; i<NUM_LAYERS; i++) {
                if(i == 19)
                    continue;
                layers[i].mode = apply_all;
            }
            apply_all = -1;
        }
    }
    
    void activate(controllable@ e) {
        if(e.is_same(controller_controllable(0))) {
            if(not activated) {
                trigger();
                activated = true;
            }
            active_this_frame = true;
        }
    }
    
    cloneable_trigger_base@ clone() {
        layer_dust_trigger@ ldt = layer_dust_trigger();
        for(uint i = 0; i<layers.length(); i++){
            ldt.layers.insertLast(layers[i].clone());
        }
        return ldt;
    }
}

class layer_properties {
    [option, 0:Ignore, 1:All, 2:Dustblocks] int mode;
    
    layer_properties() {
        layer_properties(IGNORE);
    }
    
    layer_properties(int mode) {
        this.mode = mode;
    }
    
    layer_properties@ clone() {
        return layer_properties(mode);
    }
}