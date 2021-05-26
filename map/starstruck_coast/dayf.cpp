/**
 * Contributors: Skyhawk, Fireball, BigDiesel
 */

#include "fog.as"

class script {
    scene@ g;
	
	int counter = 0;

    [boolean|tooltip:"Whether or not the script handles sublayers.\nDisable if not needed."]
    bool use_sublayers = false;

    /* This creates named annotations,
     * which lets you attach specific fog triggers to them in the editor */
    [entity] int midday_fog;
    [entity] int afternoon_fog;
    [entity] int sunset_fog;
    [entity] int dusk_fog;
    [entity] int evening_fog;
    [entity] int midnight_fog;
    
    //the number of frames between fog updates
    int PERIOD = 60;
    
    //the number of transitions in fog_settings and FOG_TIME
    int SIZE = 10;
    
    //this array is populated in on_level_start with the fog settings from each annotation
    //arranged from midnight to midday, then reverse order back to evening
    array<fog_setting@> fog_settings;
    
    //this array gives the time in hours of the day for each fog transition
    array<int> FOG_TIME = {4, 5, 6, 8, 12, 16, 18, 19, 20, 24};
    
    script() {
        @g = get_scene();
    }

    //debug counter for faster fog transitions
    //float counter2 = 0;
    void fog_time() {
        //counter2 = (counter2 + 0.05) % 24;
        //double second = counter2;
        
        camera@ cam = get_camera(0);
		timedate@ time = localtime();
		double second = time.hour() + time.min()/60.0 + time.sec()/3600.0;
		
        fog_setting@ currentFog = null;
        fog_setting@ nextFog = null;
        double lerp_ratio = 0;
        
        //find the correct period in the array for the current time
        int prev_time = 0;
        for(int i = 0; i < SIZE; i++) {
            if(prev_time <= second && second < FOG_TIME[i]) {
                @currentFog = @fog_settings[i];
                @nextFog = @fog_settings[(i+1)%SIZE];
                lerp_ratio = (second - prev_time) / (FOG_TIME[i] - prev_time);
                break;
            }
            prev_time = FOG_TIME[i];
        }
        
        //extra null check, just in case something isnt set correctly
        if(@currentFog != null || @nextFog != null) {
            fog_setting@ interpolatedFog = interpolateFog(@currentFog, @nextFog, lerp_ratio, @cam);
            cam.change_fog(interpolatedFog, 0.0);
            //puts(lerp_ratio);
        }
    }
    
    //lerp ratio is between 0 and 1
    fog_setting@ interpolateFog(fog_setting@ current, fog_setting@ next, double lerp_ratio, camera@ cam) {   
        fog_setting@ interpolatedFog = cam.get_fog();
        
        for(int layer = 0; layer<=22; layer++) {
            if(use_sublayers) {
                for(int sublayer = 0; sublayer<=24; sublayer++) {
                    interpolatedFog.colour(layer,sublayer,interpolateColors(current.colour(layer, sublayer), next.colour(layer,sublayer), lerp_ratio));
                    interpolatedFog.percent(layer, sublayer,interpolateFloat(current.percent(layer, sublayer), next.percent(layer, sublayer), lerp_ratio));
                }
            }
            else {
                interpolatedFog.layer_colour(layer, interpolateColors(current.colour(layer, 0), next.colour(layer, 0), lerp_ratio));
                interpolatedFog.layer_percent(layer, interpolateFloat(current.percent(layer, 0), next.percent(layer, 0), lerp_ratio));
            }
        }
        
        interpolatedFog.bg_top(interpolateColors(current.bg_top(), next.bg_top(), lerp_ratio));
        interpolatedFog.bg_mid(interpolateColors(current.bg_mid(), next.bg_mid(), lerp_ratio));
        interpolatedFog.bg_bot(interpolateColors(current.bg_bot(), next.bg_bot(), lerp_ratio));
        
        interpolatedFog.bg_mid_point(interpolateFloat(current.bg_mid_point(), next.bg_mid_point(), lerp_ratio));
        interpolatedFog.stars_top(interpolateFloat(current.stars_top(), next.stars_top(), lerp_ratio));
        interpolatedFog.stars_mid(interpolateFloat(current.stars_mid(), next.stars_mid(), lerp_ratio));
        interpolatedFog.stars_bot(interpolateFloat(current.stars_bot(), next.stars_bot(), lerp_ratio));
        
        return interpolatedFog;
    }
    
    //changed to not create objects, for performance
    uint32 interpolateColors(uint32 current, uint32 next, double lerp_ratio) {
        double a1 = ((current & 0xFF000000) >> 24);
        double r1 = ((current & 0x00FF0000) >> 16);
        double g1 = ((current & 0x0000FF00) >> 8);
        double b1 = (current & 0x000000FF);
        double a2 = ((next & 0xFF000000) >> 24);
        double r2 = ((next & 0x00FF0000) >> 16);
        double g2 = ((next & 0x0000FF00) >> 8);
        double b2 = (next & 0x000000FF);
        
        double r = r1 + lerp_ratio * (r2-r1);
        double g = g1 + lerp_ratio * (g2-g1);
        double b = b1 + lerp_ratio * (b2-b1);
        double a = a1 + lerp_ratio * (a2-a1);
        
        return (uint32(a) << 24) + (uint32(r) << 16) + (uint32(g) << 8) + uint32(b);
    }
    
    float interpolateFloat(float current, float next, double lerp_ratio) {
        return (next * lerp_ratio + current * (1- lerp_ratio));
    }
    
    void on_level_start() {
        //we only care about the fog_settings of each trigger
        float temp_speed;
        int temp_size;
        
        //the colors used for each time of day
        fog_setting@ midday_settings;
        fog_setting@ afternoon_settings;
        fog_setting@ sunset_settings;
        fog_setting@ dusk_settings;
        fog_setting@ evening_settings;
        fog_setting@ midnight_settings;

		get_fog_setting(entity_by_id(midday_fog), midday_settings, temp_speed, temp_size);
		get_fog_setting(entity_by_id(afternoon_fog), afternoon_settings, temp_speed, temp_size);
		get_fog_setting(entity_by_id(sunset_fog), sunset_settings, temp_speed, temp_size);
		get_fog_setting(entity_by_id(dusk_fog), dusk_settings, temp_speed, temp_size);
		get_fog_setting(entity_by_id(evening_fog), evening_settings, temp_speed, temp_size);
		get_fog_setting(entity_by_id(midnight_fog), midnight_settings, temp_speed, temp_size);
        
        //make sure the array is empty, then insert the settings
        //I couldnt find a nicer syntax to set fixed contents of an array in the documentation
        fog_settings.resize(0);
        fog_settings.insertAt(0, array<fog_setting@> =
            {midnight_settings, evening_settings, dusk_settings, sunset_settings, afternoon_settings,
            midday_settings, afternoon_settings, sunset_settings, dusk_settings, evening_settings});
        
        fog_time();
    }
	
	void checkpoint_load() {
        fog_time();
    }
    
	void step(int entities) {
		counter++;
		if (counter >= PERIOD) {
			counter = 0;
            
            ///int start = get_time_us();
			
            fog_time();
            
            //int end = get_time_us();
            //puts(""+(end-start)/1000.0+" ms");
		}
	}

    void draw_bordered_text(textfield@ tf, int fog_id, string text, uint color) {
        entity@ e = entity_by_id(fog_id);
        if(@e != null) {
            tf.text(text);
            tf.colour(color);
			
            tf.draw_world(22, 1, e.x(), e.y()-15, 1, 1, 0);
            
            tf.colour(0xFFFFFFFF);
            tf.draw_world(22, 0, e.x()-1, e.y()-16, 1, 1, 0);
            tf.draw_world(22, 0, e.x()-1, e.y()-14, 1, 1, 0);
            tf.draw_world(22, 0, e.x()+1, e.y()-16, 1, 1, 0);
            tf.draw_world(22, 0, e.x()+1, e.y()-14, 1, 1, 0);
        }
    }
    
    void editor_draw(float subframe) {
        textfield@ tf = create_textfield();
        
        tf.align_horizontal(0);
        tf.align_vertical(1);
        
        draw_bordered_text(@tf, midday_fog, "Midday", 0xFF000000);
        draw_bordered_text(@tf, afternoon_fog, "Afternoon", 0xFF000000);
        draw_bordered_text(@tf, sunset_fog, "Sunset", 0xFF000000);
        draw_bordered_text(@tf, dusk_fog, "Dusk", 0xFF000000);
        draw_bordered_text(@tf, evening_fog, "Evening", 0xFF000000);
        draw_bordered_text(@tf, midnight_fog, "Midnight", 0xFF000000);
    }
}