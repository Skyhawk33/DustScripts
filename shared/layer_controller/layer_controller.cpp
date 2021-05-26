#include '../../lib/cloneable_trigger_base.cpp'

const int FPS = 60;
const int NUM_LAYERS = 23;
const int DEFAULT_COLLISION = 19;
const array<float> DEFAULT_SCALE = {0.02,0.05,0.1,0.15,0.2,0.25,0.7,0.75,0.8,0.85,0.9,0.95,1,1,1,1,1,1,1,1,1,1,1};

bool valid_collision(int collision) { 
	return 6 <= collision && collision <= 20 && collision != 18; 
}

array<layer_properties> initialize_layers(array<layer_properties> layers) {
	// make sure that there is the correct number of layers
	while(layers.length() > NUM_LAYERS) {
		layers.removeLast();
	}
	for(int i=layers.length(); i<NUM_LAYERS; i++) {
		layers.insertLast(layer_properties(i, DEFAULT_SCALE[i], true));
		//puts("layer "+i+" scale "+layers[i].scale);
	}
	return layers;
}

class script {
	scene@ g;
	[boolean] bool apply_in_editor = true;
	[text] int collision = DEFAULT_COLLISION;
	[text] array<layer_properties> layers;
	bool editor_initialized;
	
	array<layer_properties> checkpoint_layers;
	array<layer_properties> checkpoint_goal_layers;
	array<float> checkpoint_layer_delta(0);
	int checkpoint_transition_length;
	int checkpoint_collision_layer;
	
	array<layer_properties> active_layers;
	array<layer_properties> goal_layers;
	array<float> layer_delta(0);
	int active_transition_length;
	int active_collision_layer;

	script() {
		@g = get_scene();
		editor_initialized = false;
		
		// this initialization happens before user-saved values are applied,
		// so it is only used when the script is first added
		layers = initialize_layers(layers);
	}
	
	void set_layer_properties(array<layer_properties> properties, int collision_layer) {
		set_layer_properties(properties, collision_layer, 0, false);
	}
    
    void set_layer_properties(array<layer_properties> properties, int collision_layer, float transition_period) {
		set_layer_properties(properties, collision_layer, 0, false);
	}
	
	void set_layer_properties(array<layer_properties> properties, int collision_layer, float transition_period, bool transition_first) {
		active_transition_length = int(transition_period);
		goal_layers = properties;
		
		if(active_transition_length == 0) {
			active_layers = properties;
			layer_delta.resize(0);
		}
		else {
			layer_delta = array<float>(properties.length, 0);
			for(uint i = 0; i < properties.length; i++) {
                if(!transition_first) {
                    active_layers[i].draw_order = goal_layers[i].draw_order;
                    active_layers[i].visible = goal_layers[i].visible;
                }
				layer_delta[i] = (goal_layers[i].scale - active_layers[i].scale) / active_transition_length;
			}
		}
		active_collision_layer = collision_layer;
	}
	
	void apply_layer_changes() {
		apply_layer_changes(true,true,true);
	}
	
	// if any of the params are set to false, the respective property will be reset to default
	void apply_layer_changes(bool reorder_layers, bool scale_layers, bool hide_layers) {
		//organize the visual order of the layers
		g.reset_layer_order();
		array<int> visual_layer;
		for(int i=0; i<NUM_LAYERS; i++) {
			
			if(reorder_layers) {
				int pos=i;
				while(pos>0 and active_layers[i].draw_order < active_layers[visual_layer[pos-1]].draw_order) {
					g.swap_layer_order(i, visual_layer[pos-1]);
					pos--;
				}
				visual_layer.insertAt(pos, i);
			}
			
			if(scale_layers) {
				g.layer_scale(i, active_layers[i].scale);
			}
			else {
				g.layer_scale(i, DEFAULT_SCALE[i]);
			}
			
			
			if(hide_layers) {
				g.layer_visible(i, active_layers[i].visible);
			}
			else {
				g.layer_visible(i, true);
			}
		}
		g.default_collision_layer(active_collision_layer);
	}
	
	void editor_step() {
		// make sure the layer array is set up correctly
		// this cant be done in the constructor because the user-saved values overwrite anything created then.
		if(not editor_initialized) {
			layers = initialize_layers(layers);
			collision = valid_collision(collision) ? collision : DEFAULT_COLLISION;
			
			if(apply_in_editor) {
				set_layer_properties(layers, collision);
				apply_layer_changes();
			}
			else {
				set_layer_properties(initialize_layers(array<layer_properties> = {}), DEFAULT_COLLISION);
				apply_layer_changes();
			}
			
			editor_initialized = true;
		}
	}
	
	void on_level_start() {
		//emergency initialization in case the map was saved when it had the wrong number of layers. see editor_step
		layers = initialize_layers(layers);
		collision = valid_collision(collision) ? collision : DEFAULT_COLLISION;
		set_layer_properties(layers, collision);
		apply_layer_changes();
	}
	
	void step(int entities) {
		if(active_transition_length > 1) {
			for(uint i = 0; i < active_layers.length; i++) {
				active_layers[i].scale += layer_delta[i];
			}
			active_transition_length--;
			apply_layer_changes();
		}
		else if(active_transition_length == 1) {
			active_layers = goal_layers;
			apply_layer_changes();
			active_transition_length = 0;
		}
	}
	
	void checkpoint_save() {
		checkpoint_layers = active_layers;
		checkpoint_goal_layers = goal_layers;
		checkpoint_transition_length = active_transition_length;
		checkpoint_layer_delta = layer_delta;
		checkpoint_collision_layer = active_collision_layer;
	}
	
	void checkpoint_load() {
		active_layers = checkpoint_layers;
		goal_layers = checkpoint_goal_layers;
		active_transition_length = checkpoint_transition_length;
		layer_delta = checkpoint_layer_delta;
		active_collision_layer = checkpoint_collision_layer;
		apply_layer_changes();
	}
}

class layer_control_trigger : cloneable_trigger_base {
	scripttrigger@ self;
	script@ s;
	bool activated;
	bool active_this_frame;
	bool editor_initialized = false;
	
    [option,0:None,1:Up,2:Down,3:Left,4:Right,5:Jump,6:Light,7:Heavy,8:Dash,9:Taunt|tooltip:"This key must be pressed while in the trigger area\nto activate the trigger"]
    int activator_button = 0;
    
	[boolean|tooltip:"Displays the effects of this trigger while in the editor.\nOnly one trigger can have this active at a time"]
    bool apply_in_editor = false;
    
	[text] int collision = DEFAULT_COLLISION;
	[text] array<layer_properties> layers;
    
    [boolean|tooltip:"if the transition time is not instant, this decides whether to\napply other changes before or after the scaling finishes"]
    bool transition_first = false;
	[slider,min:0,max:600|tooltip:"How slowly the scale should transition to the new values.\nLeft is instant, right is 10 seconds."]
    float transition_period = 0;
	
	void init(script@ s, scripttrigger@ self) {
		@this.s = @s;
		@this.self = @self;
		activated = false;
		active_this_frame = false;
		layers = initialize_layers(layers);
		collision = valid_collision(collision) ? collision : DEFAULT_COLLISION;
	}
	
	void editor_step() {
		cloneable_trigger_base::editor_step();
		
		if(not editor_initialized) {
			if(apply_in_editor) {
				layers = initialize_layers(layers);
				collision = valid_collision(collision) ? collision : DEFAULT_COLLISION;
				s.set_layer_properties(layers, collision);
				s.apply_layer_changes();
			}
			editor_initialized = true;
		}
	}
	
	void trigger() {
		s.set_layer_properties(layers, collision, transition_period, transition_first);
		s.apply_layer_changes();
	}
	
	void step() {
		if(activated) {
			if(not active_this_frame) {
				activated = false;
			}
			active_this_frame = false;
		}
	}
	
	void activate(controllable@ e) {
		if(e.is_same(controller_controllable(0)) && activator_pressed(@e)) {
			if(not activated) {
				trigger();
				activated = true;
			}
			active_this_frame = true;
		}
	}
    
    bool activator_pressed(controllable@ e) {
        switch(activator_button) {
            case 0: //None
                return true;
            case 1: //Up
                return e.y_intent() == -1;
            case 2: //Down
                return e.y_intent() == 1;
            case 3: //Left
                return e.x_intent() == -1;
            case 4: //Right
                return e.x_intent() == 1;
            case 5: //Jump
                return e.jump_intent() > 0;
            case 6: //Light
                return e.light_intent() >= 10;
            case 7: //Heavy
                return e.heavy_intent() >= 10;
            case 8: //Dash
                return e.dash_intent() > 0;
            case 9: //Taunt
                return e.taunt_intent() > 0;
        }
        return false;
    }
	
	cloneable_trigger_base@ clone() {
		layer_control_trigger@ lct = layer_control_trigger();
        lct.activator_button = activator_button;
		lct.collision = collision;
        lct.transition_first = transition_first;
		lct.transition_period = transition_period;
		for(uint i = 0; i<layers.length(); i++){
			lct.layers.insertLast(layers[i].clone());
		}
		return lct;
	}
}

class layer_properties {
	[text] float draw_order;
	[text] float scale;
	[boolean] bool visible;
	
	bool debug=false;
	
	layer_properties() {
		layer_properties(0,1, true);
	}
	
	layer_properties(float index, float size, bool vis) {
		draw_order = index;
		scale = size;
		visible = vis;
	}
	
	layer_properties@ clone() {
		return layer_properties(draw_order, scale, visible);
	}
}