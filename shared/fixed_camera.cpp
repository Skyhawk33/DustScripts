class script {
	
	[position, mode:WORLD, layer:19, y:fixed_y]
				float fixed_x;
	[hidden]	float fixed_y;
	[text]		float screen_height;
	
	float scale_x, scale_y;
	
	void on_level_start() {
		
		if(screen_height > 0)
			scale_x = scale_y = 1080 / screen_height;
		
		camera@ cam = get_active_camera();
		
		cam.script_camera(true);
		
		fix_camera(cam);
		fix_prev_camera(cam);
	}
	
	// might have to do something on checkpoint_save
	// to make sure state saves properly? not sure.
	
	void checkpoint_load() {
		camera@ cam = get_active_camera();
		cam.script_camera(true);
		fix_prev_camera(cam);
		fix_camera(cam);
	}
	
	void fix_prev_camera(camera@ cam) {
		cam.prev_x(fixed_x);
		cam.prev_y(fixed_y);
		
		if(screen_height > 0) {
			cam.prev_scale_x(scale_x);
			cam.prev_scale_y(scale_y);
		}
	}
	
	void fix_camera(camera@ cam) {
		cam.x(fixed_x);
		cam.y(fixed_y);
		
		if(screen_height > 0) {
			cam.scale_x(scale_x);
			cam.scale_y(scale_y);
		}
	}
}