/**
 *  changes the controller_mode to 1 to allow reading held inputs,
 *  while still allowing for regular dustforce behaviour.
 *
 *  BUG: inputs held through level countdown and death are read on the first actionable frame.
 */

class script {
    scene@ g;
    controllable@ player;
    
    bool debug_print = true;
    bool jump_used = false;
    bool taunt_pressed = false;
    bool dash_pressed = false;
    int heavy_counter = 0;
    int light_counter = 0;

    script() {
        @g = get_scene();
    }

    void step(int entities) {
        if (@player == null){
            //set the controller mode to is_pressed
            get_camera(0).controller_mode(1);
            @player = controller_controllable(0);
        }
        
        //before running standardize_intents, we can check for held dash/taunt inputs
        if (debug_print && player.dash_intent() > 0)
            puts("Dash Held!!!");
        if (debug_print && player.taunt_intent() > 0)
            puts("Taunt Held!!!");
        
        //running this converts the intents to standard values
        standardize_intents(@player);
    }
    
    void step_post(int entities) {
        //update the variables used to control intents next frame
        standardize_intents_post(@player);
    }
    
    /**
     * takes inputs in is_pressed controller mode and converts them into standard controller mode.
     * must be called from step so that it can correct the intents before the game uses them.
     *
     * Some information is only available in step_post.
     * To gather that information, see "standardize_intents_post"
     */
    void standardize_intents(controllable@ player) {
        
        if (player is null)
            return;
        
        // dash is only 1 on the first frame it is pressed. after that, return it to 0
        int dash = player.dash_intent();
        if (dash > 0) {
            if (dash_pressed)
                player.dash_intent(0);
            
            // if it's the first frame dash is pressed, and the player is holding down
            // check if they should dash, fall, or both
            else if (player.y_intent() == 1) {
                player.fall_intent(1);
                if (!player.ground()) {
                    player.dash_intent(0);
                }
            }
        }
        dash_pressed = dash > 0;
        
        // taunt is only 1 on the first frame it is pressed. after that, return it to 0
        int taunt = player.taunt_intent();
        if (taunt > 0 && taunt_pressed) {
            player.taunt_intent(0);
        }
        taunt_pressed = taunt > 0;
        
        // jump is 1 when held but unused, and 2 when it is held and already used
        if (player.jump_intent() > 0 && jump_used)
            player.jump_intent(2);
        else
            jump_used = false;
        
        // attacks are 10 when held but unused, and 11 when held and already used
        // when released and unused, attacks buffer by counting down to 0
        if (player.light_intent() == 10 && light_counter == 11) {
                player.light_intent(11);
        }
        else if (player.light_intent() == 0) {
            if (light_counter == 11)
                light_counter = 0;
            else if (light_counter > 0) {
                light_counter--;
                player.light_intent(light_counter);
            }
        }
        
        if (player.heavy_intent() == 10 && heavy_counter == 11) {
                player.heavy_intent(11);
        }
        else if (player.heavy_intent() == 0) {
            if (heavy_counter == 11)
                heavy_counter = 0;
            else if (heavy_counter > 0) {
                heavy_counter--;
                player.heavy_intent(heavy_counter);
            }
        }
    }
    
    /**
     * Gathers information about which intents were used during the frame in order to correctly
     * assign intents during future frames
     */
    void standardize_intents_post(controllable@ player) {
        
        if (player is null)
            return;
        
        // even though the input mode has changed, the game will still change the intent to 2
        // if it was used during this frame
        if (player.jump_intent() == 2)
            jump_used = true;
        
        // for attacks, if the intent was used, it will be changed to 11. otherwise, it will stay 10
        if (player.light_intent() > 9)
            light_counter = player.light_intent();
        
        if (player.heavy_intent() > 9)
            heavy_counter = player.heavy_intent();
    }
    
    
    void checkpoint_load() {
        @player = null;
    }
}