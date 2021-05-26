/**
 * Author: AlexSpeedy
 * https://github.com/AlexMorson/dustforce-scripts
 */

#include "../../../cmann/lib/std.cpp"
#include "../../../cmann/lib/math/math.cpp"

/**
 * A hacky way to create new fog_setting objects.
 */
fog_setting@ create_fog_setting() {
    return get_active_camera().get_fog();
}

/**
 * Extract a fog_setting object from a fog trigger entity.
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


/**
 * Create a fog trigger entity from a fog_setting object.
 *
 * Note: It will not appear unless it is added to the scene.
 *
 * scene@ g = get_scene();
 * g.add_entity(fog_trigger);
 */
entity@ create_fog_trigger(fog_setting@ fog, float fog_speed, int trigger_size) {
    entity@ fog_trigger = create_entity("fog_trigger");
    set_fog_trigger(fog_trigger, fog, fog_speed, trigger_size);
    return fog_trigger;
}

/**
 * Update an existing fog trigger entity with a fog_setting object.
 */
void set_fog_trigger(entity@ fog_trigger, fog_setting@ fog, float fog_speed, int trigger_size) {
    if (fog_trigger is null) return;
    if (fog_trigger.type_name() != "fog_trigger") return;

    varstruct@ vars = fog_trigger.vars();

    // Figure out if we (need to) use sublayers
    bool use_sublayers = false;
    for (int layer=0; layer<=20; ++layer) {
        uint layer_colour = fog.colour(layer, 0);
        float layer_percent = fog.percent(layer, 0);
        for (int sublayer=1; sublayer<=24; ++sublayer) {
            if (
                fog.colour(layer, sublayer) != layer_colour ||
                fog.percent(layer, sublayer) != layer_percent
            ) {
                use_sublayers = true;
                break;
            }
        }
        if (use_sublayers) break;
    }

    vars.get_var("has_sub_layers").set_bool(use_sublayers);

    int array_size = use_sublayers ? 546 : 21;
    vararray@ colour = vars.get_var("fog_colour").get_array();
    vararray@ percent = vars.get_var("fog_per").get_array();
    colour.resize(array_size);
    percent.resize(array_size);

    // Set the '-' sublayer (copying settings from sublayer 10)
    for (int layer=0; layer<=20; ++layer) {
        colour.at(layer).set_int32(fog.colour(layer, 10));
        percent.at(layer).set_float(fog.percent(layer, 10));
    }

    // Set the rest of the sublayers
    if (use_sublayers) {
        for (int layer=0; layer<=20; ++layer) {
            for (int sublayer=0; sublayer<=24; ++sublayer) {
                colour.at(21*(sublayer+1) + layer).set_int32(fog.colour(layer, sublayer));
                percent.at(21*(sublayer+1) + layer).set_float(fog.percent(layer, sublayer));
            }
        }
    }

    vararray@ gradient = vars.get_var("gradient").get_array();
    gradient.resize(3);
    
    gradient.at(0).set_int32(fog.bg_top());
    gradient.at(1).set_int32(fog.bg_mid());
    gradient.at(2).set_int32(fog.bg_bot());

    vars.get_var("gradient_middle").set_float(fog.bg_mid_point());

    vars.get_var("star_top").set_float(fog.stars_top());
    vars.get_var("star_middle").set_float(fog.stars_mid());
    vars.get_var("star_bottom").set_float(fog.stars_bot());

    vars.get_var("fog_speed").set_float(fog_speed);

    vars.get_var("width").set_int32(trigger_size);
}
