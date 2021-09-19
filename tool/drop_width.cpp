class script {
    scene@ g;
    dustman@ player;
    [text] int test_dist = 960;
    [text] float max_tolerance = 0.001;
    [text] float center_search = 0.5;
    [text] float search_width = 48;
    [boolean] bool jump = false;
    [boolean] bool fastfall = true;
    [boolean] bool disable = false;
    
    float start_x = 0;
    float start_y = 0;
    float test_x = -1;
    float previous_test_x = -1;
    int test_count = 0;
    
    float left_min = 0;
    float left_max = 0;
    float center = 0;
    float right_min = 0;
    float right_max = 0;
    
    int center_check = 0;
    bool found_center = false;
    bool found_left_edge = false;
    bool done_final_left_test = false;
    bool found_right_edge = false;
    bool done_final_right_test = false;
    bool test_complete = false;
    
    script() {
        @g = get_scene();
    }
    
    void step(int entities) {
        //if no player is defined, get the player
        if(@player == null){
            controllable@ c = controller_controllable(0);
            if(@c != null){
                @player = c.as_dustman();
                
                start_x = player.x();
                start_y = player.y();
                left_min = start_x - search_width/2;
                right_max = start_x + search_width/2;
                puts(""+player.x()%48);
                
                if(disable) {
                    test_complete = true;
                }
                else {
                    //time warp messes with results, as some subframe positions are warped past
                    //g.time_warp(4);
                    //player.time_warp(4);
                    puts("finding center");
                    player.auto_respawn(false);
                    start_center_test();
                    //dont check the results of the test on the same frame
                    return;
                }
            }
        }
        if(!test_complete) {
            if((!jump && player.y()-start_y >= test_dist) || (jump && player.y()-start_y <= -test_dist)) {
                
                //test passed, update estimates, restart
                if(!found_center) {
                    found_center = true;
                    puts("center found. finding left edge.");
                    
                    //we have values for the inner boundary
                    center = test_x;
                    left_max = center;
                    right_min = center;
                    
                    //we know a rough outer boundary from the previous center checks
                    if(previous_test_x != -1 && center_search < search_width/2) {
                        puts("using previous center check as boundary");
                        if(previous_test_x < center)
                            left_min = center - center_search;
                        else if (center < previous_test_x)
                            right_max = center + center_search;
                    }
                    
                    start_left_test();
                }
                else if(!found_left_edge) {
                    left_max = test_x;
                    evaluate_left_side();
                }
                else if(!found_right_edge){
                    right_min = test_x;
                    evaluate_right_side();
                }
            }
            else if(player.dead() || player.state()==0) {
                
                //test failed, update estimates, restart
                if(!found_center) {
                    //update the center check for the next test
                    // this alternates between checking left and right of the initial position
                    center_check *= -1;
                    if(center_check >= 0)
                        center_check++;
                    
                    start_center_test();
                }
                else if(!found_left_edge) {
                    left_min = test_x;
                    evaluate_left_side();
                }
                else if(!found_right_edge) {
                    right_max = test_x;
                    evaluate_right_side();
                }
            }
            else {
                //test in progress
            }
        }
    }
    
    void reset_player(float x_pos) {
        player.dead(false);
        player.dash(1);
        player.state(0);
        player.set_xy(x_pos, start_y);
        player.set_speed_xy(0,0);
        
        if(jump)
            player.jump_intent(1);
        else if(fastfall)
            player.fall_intent(1);
    }
    
    void start_test(float x_pos) {
        test_count++;
        previous_test_x = test_x;
        test_x = x_pos;
        
        reset_player(test_x);
    }
    
    void start_center_test() {
        float x = start_x+center_search*center_check;
        
        // if we've gone outside the search range, stop the test
        if(x < left_min) {
            test_complete = true;
            player.auto_respawn(true);
            reset_player(start_x);
            puts("Could not find a drop in the given area");
        }
        else
            start_test(x);
    }
    
    void start_left_test() {
        start_test(left_min + (left_max-left_min)/2);
    }
    
    void evaluate_left_side() {
        if(left_max - left_min < max_tolerance) {
            if(!done_final_left_test && left_max-floor(left_max) < max_tolerance) {
                done_final_left_test = true;
                start_test(floor(left_max));
            }
            else {
                found_left_edge = true;
                puts("left edge found. finding right edge.");
                start_right_test();
            }
        }
        else
            start_left_test();
    }
    
    void start_right_test() {
        start_test(right_min + (right_max-right_min)/2);    
    }
    
    void evaluate_right_side() {
        if(right_max - right_min < max_tolerance) {
            if(!done_final_right_test && ceil(right_min)-right_min < max_tolerance) {
                done_final_right_test = true;
                start_test(ceil(right_min));
            }
            else {
                found_right_edge = true;
                puts("right edge found.");
                finish_test();
            }
        }
        else
            start_right_test();
    }
    
    void finish_test() {
        test_complete = true;
        puts("Test finished ----------- attempts: "+test_count);
        puts("\tleft : "+left_max+" ("+(left_max%48)+")");
        puts("\tright: "+right_min+" ("+(right_min%48)+")");
        puts("\twidth: "+(right_min-left_max));
        
        //g.time_warp(1);
        //player.time_warp(1);
        player.auto_respawn(true);
        reset_player(center);
    }
    
    void checkpoint_save() {
        
    }
    
    void checkpoint_load() {
        @player = null;
    }
}