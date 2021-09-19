#include "../lib/location_selector.cpp"

// TODO: sometimes trials fail when they shouldnt, may have to do with "Cant play sound?"
// TODO: gaps tend to fall on integer values, so test those first during center checks?
// TODO: after the gap has been measured below tolerance, test integer values within the uncertainty range?
// TODO: Memory leak with leftover entities???

class script {
    scene@ g;
    dustman@ player;
    LocationSelector@ ls;
    
    [text] int concurrency = 10;
    [text] int test_dist = 480;
    [text] float max_tolerance = 0.001;
    [text] float center_search = 0.5;
    [text] float search_width = 48;
    [boolean] bool jump = false;
    [boolean] bool fastfall = true;
    [boolean] bool disable = false;
    
    float start_x = 0;
    float start_y = 0;
    int trial_count = 0;
    
    float left_min = 0;
    float left_max = 0;
    float center = 0;
    float right_min = 0;
    float right_max = 0;
    
    float result_left = 1;
    float result_right = -1;
    
    int center_checks = 0;
    
    bool running = false;
    bool center_found = false;
    bool below_tolerance = false;
    bool taunt_held = false;
    
    array<Trial@> trials(0);
    array<float> failed_center_x(0);

    script() {
        @g = get_scene();
        //if the user tabbed to editor while in location select mode, we need to restart time
        g.time_warp(1);
        @ls = LocationSelector(g, 0);
    }
    
    void on_level_start() {
        trials.resize(concurrency);
        for(int i=0; i<concurrency; i++) {
            @trials[i] = Trial(g);
            trials[i].jump(jump);
            trials[i].downdash(fastfall);
            trials[i].length(test_dist);
        }
    }
    
    void move_cameras(){
        ls.move_cameras();
    }

    void step(int entities) {
        //if no player is defined, get the player
        if(@player == null){
            controllable@ c = controller_controllable(0);
            if(@c != null){
                @player = c.as_dustman();
                
                for(int i=0; i<concurrency; i++) {
                    trials[i].character(player.character());
                }
            }
        }
        else {
            if(ls.active()) {
                //allow the player to cancel the test
                if(player.taunt_intent() == 1 && !taunt_held) {
                    taunt_held = true;
                    ls.reset();
                    return;
                }
                else {
                    taunt_held = false;
                }
                
                ls.step(entities);
                
                if(ls.finished()) {
                    //puts("("+ls.x()+", "+ls.y()+")");
                    start_x = ls.x();
                    start_y = ls.y();
                    ls.reset();
                    //we have a start location. begin the test.
                    running = true;
                }
            }
            if(running) {
                //allow the player to cancel the test
                if(player.taunt_intent() == 1 && !taunt_held) {
                    taunt_held = true;
                    reset_test();
                    return;
                }
                else {
                    taunt_held = false;
                }
                
                for(int i=0; i<concurrency; i++) {
                    trials[i].step(entities);
                }
                update_trials();
                
                bool all_stopped = true;
                for(int i=0; i<concurrency && all_stopped; i++) {
                    all_stopped = !trials[i].started();
                }
                
                if(all_stopped) {
                    if(center_found) {
                        result_left = left_max;
                        result_right = right_min;
                        
                        puts("Test finished ----------- attempts: "+trial_count);
                        puts("\tleft : "+result_left+" ("+(result_left%48)+")");
                        puts("\tright: "+result_right+" ("+(result_right%48)+")");
                        puts("\twidth: "+(result_right-result_left));
                    }
                    else {
                        puts("Could not find drop");
                    }
                    
                    reset_test();
                }
            }
            else {
                if(player.taunt_intent() == 1 && !taunt_held) {
                    taunt_held = true;
                    ls.start();
                }
                else {
                    taunt_held = false;
                }
            }
        }
    }
    
    void update_trials() {
        bool check_trials = true;
                
        while(check_trials) {
            //assume we only need one pass
            check_trials = false;
            
            for(int i=0; i<concurrency; i++) {
                if(trials[i].finished()) {
                    trial_count++;
                    if(center_found) {
                        // update the bound according to the finished trial
                        // and stop all out of bounds trials
                        
                        if(trials[i].x() < center) {
                            if(trials[i].success() && trials[i].x() < left_max)
                                left_max = trials[i].x();
                            else if (!trials[i].success() && trials[i].x() > left_min)
                                left_min = trials[i].x();
                        }
                        else {
                            if(trials[i].success() && trials[i].x() > right_min)
                                right_min = trials[i].x();
                            else if (!trials[i].success() && trials[i].x() < right_max)
                                right_max = trials[i].x();
                        }
                        // the newly stopped trials will need to be checked in a second pass
                        stop_trials();
                        check_trials = true;
                    }
                    else {
                        if(trials[i].success()) {
                            center_found = true;
                            center = trials[i].x();
                            left_min = center - search_width/2;
                            left_max = center;
                            right_min = center;
                            right_max = center + search_width/2;
                            
                            // use the previous failed center trials to limit the bounds
                            for(uint j=0; j<failed_center_x.length(); j++) {
                                float x = failed_center_x[j];
                                if(left_min < x && x < center)
                                    left_min = x;
                                else if(center < x && x < right_max)
                                    right_max = x;
                            }
                            
                            // the newly stopped trials will need to be checked in a second pass
                            stop_trials();
                            check_trials = true;
                        }
                        else {
                            failed_center_x.insertLast(trials[i].x());
                        }
                    }
                    
                    trials[i].reset();
                }
                if(!trials[i].started() && !below_tolerance) {
                    if(center_found) {
                        array<float> positions = {left_min, left_max, right_min, right_max};
                        float gap;
                        float largest_gap = 0;
                        float midpoint;
                        float gap_midpoint = 0;
                        
                        for(int j=0; j<concurrency; j++) {
                            if(trials[j].started())
                                positions.insertLast(trials[j].x());
                        }
                        positions.sortAsc();
                        
                        //string debug = "{";
                        //for(uint j=0; j<positions.length(); j++) {
                        //    debug = debug + positions[j]+", ";
                        //}
                        //debug = debug + "}";
                        //puts(debug);
                        
                        for(uint j=1; j<positions.length(); j++) {
                            gap = positions[j] - positions[j-1];
                            midpoint = positions[j-1] + gap/2;
                            if(gap > largest_gap && (midpoint < left_max || midpoint > right_min)) {
                                largest_gap = gap;
                                gap_midpoint = midpoint;
                            }
                        }
                        
                        if(largest_gap >= max_tolerance) {
                            trials[i].x(gap_midpoint);
                            trials[i].y(start_y);
                            trials[i].start();
                        }
                        else {
                            below_tolerance = true;
                        }
                    }
                    else {
                        float trial_x = next_center_check();
                        if(abs(trial_x-start_x) < search_width/2) {
                            trials[i].x(trial_x);
                            trials[i].y(start_y);
                            trials[i].start();
                        }
                    }
                }
            }
        }
    }
    
    float next_center_check() {
        center_checks++;
        
        float x_pos = start_x;
        
        float diff = center_search;
        diff *= center_checks/2;
        diff *= center_checks%2*2-1;
        
        x_pos += diff;
        
        return x_pos;
    }
    
    //stop all trials working on positions outside of the current bounds
    void stop_trials() {
        for(int i=0; i<concurrency; i++) {
            if(trials[i].x() < left_min
                    || (left_max < trials[i].x() && trials[i].x() < right_min)
                    || right_max < trials[i].x()) {
                trials[i].reset();
            }
        }
    }
    
    void reset_test() {
        ls.reset();
        for(int i=0; i<concurrency; i++) {
            trials[i].reset();
            trials[i].remove_entity();
        }
        failed_center_x = array<float>(0);
        start_x = 0;
        start_y = 0;
        trial_count = 0;
        left_min = 0;
        left_max = 0;
        center = 0;
        right_min = 0;
        right_max = 0;
        center_checks = 0;
        running = false;
        center_found = false;
        below_tolerance = false;
    }
    
    void draw(float sub_frame) {
        ls.draw(sub_frame);
    }
    
    void checkpoint_load() {
        @player = null;
    }
}

class Trial {
    private scene@ g;
    private dustman@ trial_player = null;
    
    private string trial_character = "dustman";
    private float trial_x = 0;
    private float trial_y = 0;
    private int trial_length = 480;
    
    private bool start_jump = false;
    private bool start_downdash = true;
    
    private bool player_in_scene = false;
    
    private bool trial_started = false;
    private bool trial_finished = false;
    private bool trial_success = false;
    
    Trial(scene@ sc) {
        @g = sc;
    }
    
    string character() {
        return trial_character;
    }
    void character(string character) {
        if(!trial_started) {
            trial_character = character;
        }
    }
    
    float x() {
        return trial_x;
    }
    void x(float x) {
        if(!trial_started) {
            trial_x = x;
        }
    }
    
    float y() {
        return trial_y;
    }
    void y(float y) {
        if(!trial_started) {
            trial_y = y;
        }
    }
    
    void length(int length) {
        if(!trial_started) {
            trial_length = length;
        }
    }
    
    void jump(bool jump) {
        if(!trial_started) {
            start_jump = jump;
        }
    }
    
    void downdash(bool downdash) {
        if(!trial_started) {
            start_downdash = downdash;
        }
    }
    
    void start() {
        if(!trial_started) {
            trial_started = true;
            
            if(@trial_player == null) {
                @trial_player = create_entity("dust_man").as_dustman();
            }
            if(!player_in_scene) {
                player_in_scene = true;
                g.add_entity(trial_player.as_entity());
            }
            
            trial_player.auto_respawn(false);
            trial_player.character(trial_character);
            trial_player.ai_disabled(true);
            trial_player.dash(1);
            trial_player.state(7);
            trial_player.set_xy(trial_x, trial_y);
            trial_player.set_speed_xy(0,0);
            trial_player.time_warp(1);
            
            if(start_jump)
                trial_player.jump_intent(1);
            else if(start_downdash)
                trial_player.fall_intent(1);
            
            //puts("trial started at "+trial_x);
        }
    }
    
    void step(int entities) {
        if(trial_started && !trial_finished) {
            if((!start_jump && trial_player.y()-trial_y >= trial_length) ||
                    (start_jump && trial_y-trial_player.y() >= trial_length)) {
                //g.remove_entity(trial_player.as_entity());
                trial_finished = true;
                trial_success = true;
                trial_player.time_warp(0);
                //if(trial_x < 838 || trial_x >= 842)
                //    puts("False positive! "+trial_x);
            }
            else if(trial_player.dead() || trial_player.state()==0 || (start_jump && trial_player.y_speed() > 100)) {
                //g.remove_entity(trial_player.as_entity());
                trial_player.dead(false);
                trial_player.stun_timer(0);
                trial_finished = true;
                trial_success = false;
                trial_player.time_warp(0);
                //if(trial_x >= 838 && trial_x < 842)
                //    puts("False negative! "+trial_x);
            }
            else {
                //test in progress
            }
        }
    }
    
    void reset() {
        if(trial_started) {
            trial_started = false;
            trial_finished = false;
            trial_success = false;
            //if(!trial_finished)
            //    g.remove_entity(trial_player.as_entity());
        }
    }
    
    void remove_entity() {
        if(!trial_started || trial_finished) {
            g.remove_entity(trial_player.as_entity());
            player_in_scene = false;
        }
    }
    
    bool started() {
        return trial_started;
    }
    
    bool finished() {
        return trial_finished;
    }
    
    bool success() {
        return trial_success;
    }
}

