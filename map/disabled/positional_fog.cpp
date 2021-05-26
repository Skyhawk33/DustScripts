class script {
}

class fog_trigger : trigger_base {
	[entity] int fog_trigger_id = -1;
	scene@ g;
	scripttrigger@ self;
	
	
	void init(script@ s, scripttrigger@ st) {
		@g = get_scene();
		@self = st;
	}
	
	void editor_draw(float sf) {
		entity@ fog = entity_by_id(fog_trigger_id);
		if(fog !is null and fog.type_name() != "fog_trigger")
			@fog = null;
		
		if(fog !is null)
			g.draw_line_world(21, 1, self.x(), self.y(), fog.x(), fog.y(), 3, 0xFFFF0000);
	}
	
	void activate(controllable@ e) {
		entity@ fog = entity_by_id(fog_trigger_id);
		if(fog !is null and fog.type_name() != "fog_trigger")
			@fog = null;
		
		
		if (e.as_dustman() !is null and fog !is null) {
			
			fog_setting@ setting = null;
			float speed = 0;
			int size = 0;
			get_fog_setting(@fog, @setting, speed, size);
			
			get_active_camera().change_fog(@setting, speed);
		}
	}
}

/**
 * A hacky way to create new fog_setting objects.
 * Credits to Alexspeedy
 */
fog_setting@ create_fog_setting() {
    return get_active_camera().get_fog();
}

/**
 * Extract a fog_setting object from a fog trigger entity.
 * Credits to Alexspeedy
 *
 * Example usage:
 *
 * entity@ fog_trigger = entity_by_id(id);
 * fog_setting@ fog;
 * float fog_speed;
 * int trigger_size;
 * get_fog_setting(fog_trigger, fog, fog_speed, trigger_size);
 */
void get_fog_setting(entity@ fog_trigger, fog_setting@ &out fog, float &out fog_speed, int &out trigger_size) {
    if (fog_trigger is null) return;
    if (fog_trigger.type_name() != "fog_trigger") return;

    @fog = create_fog_setting();
    varstruct@ vars = fog_trigger.vars();

    vararray@ colour = vars.get_var("fog_colour").get_array();
    vararray@ percent = vars.get_var("fog_per").get_array();

    for (int layer=0; layer<=20; ++layer) {
        fog.layer_colour(layer, colour.at(layer).get_int32());
        fog.layer_percent(layer, percent.at(layer).get_float());
    }

    if (vars.get_var("has_sub_layers").get_bool()) {
        for (int layer=0; layer<=20; ++layer) {
            for (int sublayer=0; sublayer<=24; ++sublayer) {
                fog.colour(layer, sublayer, colour.at(21*(sublayer+1) + layer).get_int32());
                fog.percent(layer, sublayer, percent.at(21*(sublayer+1) + layer).get_float());
            }
        }
    }

    vararray@ gradient = vars.get_var("gradient").get_array();
    fog.bg_top(gradient.at(0).get_int32());
    fog.bg_mid(gradient.at(1).get_int32());
    fog.bg_bot(gradient.at(2).get_int32());

    fog.bg_mid_point(vars.get_var("gradient_middle").get_float());

    fog.stars_top(vars.get_var("star_top").get_float());
    fog.stars_mid(vars.get_var("star_middle").get_float());
    fog.stars_bot(vars.get_var("star_bottom").get_float());

    fog_speed = vars.get_var("fog_speed").get_float();

    trigger_size = vars.get_var("width").get_int32();
}