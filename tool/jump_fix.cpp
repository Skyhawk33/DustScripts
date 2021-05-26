class script {
	[text] uint jumps = 2;
	
	void on_level_start() {
		controllable@ c = controller_controllable(0);
		if(@c != null){
			dustman@ p = c.as_dustman();
			if(@p != null)
				p.dash(jumps);
		}
	}
}