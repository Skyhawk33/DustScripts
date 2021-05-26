class script {

  controllable@ x;
  int time = 0;
  int cooldown = 0;
  void on_level_start() {
    @x = controller_controllable(0);
  }

  void step(int entities) {
    time -= 1;
    cooldown -=1;
    if(x.taunt_intent()==1 && cooldown <= 0){
      time = 50;
      cooldown = 110;
      invincible();
    }
    if(cooldown <= 0){
      set_player_colour(0, 0);
    }
    
    if(time == 0){
      end_invincible();
    } 
  }
  void invincible(){
    set_player_colour(0xf54e42, 0.6);
    x.time_warp(0.85);
    x.team(0);
  }
  void end_invincible(){
    set_player_colour(0x002efc, 0.45);
    x.time_warp(1);
    x.team(1);

  }
  void set_player_colour(uint colour, float percent) {
  fog_setting@ fog = get_camera(0).get_fog();
  fog.colour(18, 10, colour);
  fog.percent(18, 10, percent);
  get_camera(0).change_fog(fog, 0.0);
  }

}