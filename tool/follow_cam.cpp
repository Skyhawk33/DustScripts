class script {
    scene@ g;
    camera@ cam;
    bool enable = true;
    
    script() {
        @g = get_scene();
    }
    
    void move_cameras() {
        @cam = get_active_camera();
        if(enable && @cam != null){
            cam.script_camera(true);
            entity@ p = cam.puppet();
            cam.x(p.x());
            cam.y(p.y());
            cam.prev_x(p.x());
            cam.prev_y(p.y());
        }
    }
    
    void editor_step() {
        enable = false;
    }
}