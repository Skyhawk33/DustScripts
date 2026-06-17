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
    
    array<layer_properties> active_layers;
    
    //the bounding box of the previous recorded zone
    int record_min_x, record_min_y, record_distance;
    
    //the dustblock locations
    [hidden] array<array<int>> dustblock_pos(0);
    [boolean] bool record = false;
    [position,mode:world,layer:19,y:world_min_y] float world_min_x;
    [hidden] float world_min_y;
    [position,mode:world,layer:19,y:world_max_y] float world_max_x;
    [hidden] float world_max_y;
    
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
        
        if (record) {
            editor_record();
            record = false;
            editor_sync_vars_menu();
        }
    }
    
    void on_level_start() {
        //emergency initialization in case the map was saved when it had the wrong number of layers. see editor_step
        layers = initialize_layers(layers);
        set_layer_properties(layers);
        g.get_filth_remaining(_, dustblocks_previous, _);
    }
    
    void checkpoint_load() {
        set_layer_properties(active_layers);
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
    }
    
    void step_post(int entities) {
        // if the number of dustblocks in the level has decreased this frame, some have been cleared, and we need to remove any on other layers
        g.get_filth_remaining(_, dustblocks, _);
        if(dustblocks < dustblocks_previous) {
            //puts("removing dustblocks!");
            set_bounds(int(player.x()/48), int(player.y()/48), MAX_SUPER_DISTANCE);
            clear_dustblock_layers(dustblocks_previous-dustblocks);
            g.get_filth_remaining(_, dustblocks, _);
        }
        dustblocks_previous = dustblocks;
    }
    
    void editor_record() {
        puts("Recording dustblocks!");
        dustblock_pos.resize(0);
        for(int x = int(world_min_x/48); x <= int(world_max_x/48); x++) {
            for(int y = int(world_min_y/48); y <= int(world_max_y/48); y++) {
                if(g.get_tile(x, y).is_dustblock()) {
                    dustblock_pos.insertLast(array<int> = {x, y});
                }
            }
        }
    }
    
    void set_bounds(int center_x, int center_y, int distance) {
        //get the bounding rectangle of the recording
        int min_x = center_x-distance, min_y= center_y-distance;
        
        //if the existing recording already covers this area, we dont need to re-record
        if(record_min_x <= min_x && min_x + 2*distance <= record_min_x + 2*record_distance && record_min_y <= min_y && min_y + 2*distance <= record_min_y + 2*record_distance)
            return;
        
        // otherwise, we need to make a new recording
        record_min_x = min_x;
        record_min_y = min_y;
        record_distance = distance;
    }
    
    void clear_dustblock_layers(int expected_dustblocks) {
        for(int i = dustblock_pos.length - 1; i >= 0; i--) {
            int x=dustblock_pos[i][0], y=dustblock_pos[i][1];
            
            // if the tile isnt in record bounds, dont bother checking it
            if(x < record_min_x || record_min_x + 2*record_distance < x ||
                y < record_min_y || record_min_y + 2*record_distance < y)
                continue;
            
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
        }
        puts("Could not find all missing dustblocks. "+ expected_dustblocks+ " unaccounted for.");
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