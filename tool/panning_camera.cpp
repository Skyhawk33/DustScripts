class script {
    scene@ g;
    camera@ cam;
    
    [position,mode:world,layer:19,y:start_y] float start_x;
    [hidden] float start_y;
    
    [position,mode:world,layer:19,y:end_y] float end_x;
    [hidden] float end_y;
    
    [text|tooltip:'changes the zoom. sometimes doesnt work, just tab in again if it doesnt'] float screen_height = 1080;
    
    [text|tooltip:'the number of seconds the pan should last'] float duration = 5;
    
    float percent_speed;
    float current_percent = 0;
    float dx, dy;

    script() {
        @g = get_scene();
    }
    
    void on_level_start() {
        g.disable_score_overlay(true);
        
        setup_cam();
        
        dx = end_x - start_x;
        dy = end_y - start_y;
        percent_speed = 1.0 / 60 / duration;
    }
    
    void move_cameras() {
        if(@cam != null) {
            cam.x( (start_x + dx * current_percent));
            cam.y( (start_y + dy * current_percent));
            
            if (current_percent < 1) {
                current_percent += percent_speed;
                puts("" + current_percent + " " + percent_speed);
            }
        }
    }
    
    void setup_cam() {
        @cam = get_camera(0);
        cam.screen_height(screen_height);
        cam.script_camera(true);
        
        cam.x(start_x);
        cam.prev_x(start_x);
        cam.y(start_y);
        cam.prev_y(start_y);
    }
}