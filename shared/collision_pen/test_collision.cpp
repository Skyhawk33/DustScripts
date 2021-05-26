#include '../../../cmann/lib/std.cpp';
#include '../../../cmann/lib/math/Line.cpp';
#include '../../../cmann/lib/input/Mouse.cpp';
#include '../../../cmann/lib/math/Bezier.cpp';

class script : callback_base
{
  
  [position,mode:world,layer:19,y:y1
  |label:"Left Point"]
  float x1;
  [hidden]
  float y1;
  
  [position,mode:world,layer:19,y:y2
  |label:"Right Point"]
  float x2;
  [hidden]
  float y2;
  
  [text] Bezier bez;
  Line bez_collision;
  
  scene@ g;
  Mouse mouse(false);
  controllable@ c;
  
  float t;
  Line line;
  Line c_line;
  float ox1, oy1;
  float ox2, oy2;
    
  script()
  {
    @g = get_scene();
  }
  
  void on_level_start()
  {
    @c = controller_controllable(0);
    
    
    if(@c != null)
    {
      g.layer_visible(18, true);
      c.set_collision_handler(this, 'collision_ground_override', 0);
      c.as_dustman().on_subframe_end_callback(this, 'subframe_step', 0);
    }
    bez.num_arcs = 20;
  }
  
  void step(int)
  {
    mouse.step();
    
    t++;
    line.x1 = x1;
    line.y1 = y1;
    line.x2 = x2;
    line.y2 = y2;
    
    if(mouse.left_press)
    {
      ox1 = line.x1 - mouse.x;
      oy1 = line.y1 - mouse.y;
    }
    else if(mouse.left_down)
    {
      x1 = line.x1 = mouse.x + ox1;
      y1 = line.y1 = mouse.y + oy1;
    }
    
    if(mouse.right_press)
    {
      ox2 = line.x2 - mouse.x;
      oy2 = line.y2 - mouse.y;
    }
    else if(mouse.right_down)
    {
      x2 = line.x2 = mouse.x + ox2;
      y2 = line.y2 = mouse.y + oy2;
    }
    
    //if(@c != null)
    //{
    //  c_line.x2 = c.x();
    //  c_line.y2 = c.y();
    //  c_line.x1 = c_line.x2;
    //  c_line.y1 = c_line.y2 - 48;
    //}
    
    //puts("Step");
  }
  
  void step_post(int entities) {
    //puts("Post Step");
  }
    
    void draw(float sub_frame)
    {
        for(int i = 0; i < bez.num_arcs; i++) {
            g.draw_quad_world(19, 10, false,
                bez.x(float(i)/bez.num_arcs), bez.y(float(i)/bez.num_arcs),
                bez.x(float(i+1)/bez.num_arcs), bez.y(float(i+1)/bez.num_arcs),
                bez.x(float(i+1)/bez.num_arcs), 0,
                bez.x(float(i)/bez.num_arcs), 0,
                0xff559955, 0xff559955, 0xff559955, 0xff559955);
            g.draw_line_world(19,10,bez.x(float(i)/bez.num_arcs), bez.y(float(i)/bez.num_arcs),bez.x(float(i+1)/bez.num_arcs), bez.y(float(i+1)/bez.num_arcs), 4, 0xff337733);
        }
        
        g.draw_line_world(19, 10, line.x1, line.y1, line.x2, line.y2, 4, 0xffff5555);
//      g.draw_line_world(19, 10, c_line.x1, c_line.y1, c_line.x2, c_line.y2, 1, 0xff55ff55);
    }
  
    void editor_draw(float sub_frame)
    {
        draw(sub_frame);
    }

    void subframe_step(dustman@ dm, int) {
        //puts("\tSubframe End");
        if(c.ground())
            c.rotation(c.ground_surface_angle());
    }

  void collision_ground_override(controllable@ c, tilecollision@ t, int side, bool mv, float snap, int)
  {
    if(side == 3) {
        //puts("\tCollision");
        if(c.x() > 96 && c.x() < 288 && c.y() > -48 && c.y() < 0)
        {
          t.hit(true);
          t.type(0);
          t.hit_x(c.x());
          t.hit_y(-21);
          return;
        }
        
        c_line.x2 = c.x();
        c_line.y2 = c.y() + 10;
        c_line.x1 = c_line.x2;
        c_line.y1 = c_line.y2 - 58;
        float x, y, dt;
        
        if(line.intersection(c_line, x, y, dt))
        {
          t.hit(true);
          t.type(0);
          t.type(int(-atan2(line.x2 - line.x1, line.y2 - line.y1) * RAD2DEG + 90));
          t.hit_x(x);
          t.hit_y(y);
          return;
        }
        
        for(int i = 0; i < bez.num_arcs; i++) {
          
          bez_collision.x1 = bez.x(float(i)/bez.num_arcs);
          bez_collision.y1 = bez.y(float(i)/bez.num_arcs);
          bez_collision.x2 = bez.x(float(i+1)/bez.num_arcs);
          bez_collision.y2 = bez.y(float(i+1)/bez.num_arcs);
          
          if(bez_collision.intersection(c_line, x, y, dt)) {
            t.hit(true);
            //t.type(45);
            t.type(int(-atan2(bez_collision.x2 - bez_collision.x1, bez_collision.y2 - bez_collision.y1) * RAD2DEG + 90));
            t.hit_x(x);
            t.hit_y(y);
            return;
          }
        }
    }
    c.check_collision(t, side, mv, snap);
    //puts(" "+t.hit()+" "+t.hit_x()+" "+t.hit_y()+" "+t.type()+" "+t.angle());
  }
  
}


// ----------------------------------------------------------------------


#include "../../../cmann/lib/math/math.cpp"

const array<string> STATES = {"idle", "victory", "run", "skid", "superskid", "fall", "land", "hover", "jump", "dash", "crouch_jump", "wall_run", "wall_grab", "wall_grab_idle", "wall_grab_release", "roof_grab", "roof_grab_idle", "roof_run", "slope_slide", "raise", "stun", "stun_wall", "stun_ground", "slope_run", "hop", "spawn", "fly", "thanks_cleansed", "idle_cleansed", "idle_cleansed_thanks", "fall_cleansed", "land_cleansed", "cleansed", "block", "wall_dash"};

const dictionary CHARACTER = {
    {"dustman", "dm"},
    {"vdustman", "vdm"},
    {"dustgirl", "dg"},
    {"vdustgirl", "vdg"},
    {"dustkid", "dk"},
    {"vdustkid", "vdk"},
    {"dustworth", "do"},
    {"vdustworth", "vdo"}
};
const array<string> EFFECTS = {
    "dmairdash",
    "dmbjump",
    "dmdash",
    "dmdbljump",
    "dmfastfall",
    "dmfjump",
    "dmheavyland",
    "dmjump",
    "dmland",
    "dmwalljump",

    "dmairheavyd",
    "dmairstriked1",
    "dmgroundstrike1",
    "dmgroundstriked",
    "dmgroundstrikeu1",
    "dmheavyd",
    "dmheavyf",
    "dmheavyu",

    "dgairdash",
    "dgbjump",
    "dgdash",
    "dgdbljump",
    "dgfastfall",
    "dgfjump",
    "dgheavyland",
    "dgjump",
    "dgland",
    "dgwalljump",

    "dgairheavyd",
    "dgairstriked1",
    "dggroundstrike1",
    "dggroundstriked",
    "dggroundstrikeu1",
    "dgheavyd",
    "dgheavyf",
    "dgheavyu",

    "dkairdash",
    "dkbjump",
    "dkdash",
    "dkdbljump",
    "dkfastfall",
    "dkfjump",
    "dkheavyland",
    "dkjump",
    "dkland",
    "dkwalljump",

    "dkairheavyd",
    "dkairstriked1",
    "dkgroundstrike1",
    "dkgroundstriked",
    "dkgroundstrikeu1",
    "dkheavyd",
    "dkheavyf",
    "dkheavyu",

    "doairdash",
    "dobjump",
    "dodash",
    "dodbljump",
    "dofastfall",
    "dofjump",
    "doheavyland",
    "dojump",
    "doland",
    "dowalljump",

    "doairheavyd",
    "doairstriked1",
    "dogroundstrike1",
    "dogroundstriked",
    "dogroundstrikeu1",
    "doheavyd",
    "doheavyf",
    "doheavyu",

    "vdmairdash",
    "vdmbjump",
    "vdmdash",
    "vdmdbljump",
    "vdmfastfall",
    "vdmfjump",
    "vdmheavyland",
    "vdmjump",
    "vdmland",
    "vdmwalljump",

    "vdmairheavyd",
    "vdmairstriked1",
    "vdmgroundstrike1",
    "vdmgroundstriked",
    "vdmgroundstrikeu1",
    "vdmheavyd",
    "vdmheavyf",
    "vdmheavyu",

    "vdgairdash",
    "vdgbjump",
    "vdgdash",
    "vdgdbljump",
    "vdgfastfall",
    "vdgfjump",
    "vdgheavyland",
    "vdgjump",
    "vdgland",
    "vdgwalljump",

    "vdgairheavyd",
    "vdgairstriked1",
    "vdggroundstrike1",
    "vdggroundstriked",
    "vdggroundstrikeu1",
    "vdgheavyd",
    "vdgheavyf",
    "vdgheavyu",

    "vdkairdash",
    "vdkbjump",
    "vdkdash",
    "vdkdbljump",
    "vdkfastfall",
    "vdkfjump",
    "vdkheavyland",
    "vdkjump",
    "vdkland",
    "vdkwalljump",

    "vdkairheavyd",
    "vdkairstriked1",
    "vdkgroundstrike1",
    "vdkgroundstriked",
    "vdkgroundstrikeu1",
    "vdkheavyd",
    "vdkheavyf",
    "vdkheavyu",

    "vdoairdash",
    "vdobjump",
    "vdodash",
    "vdodbljump",
    "vdofastfall",
    "vdofjump",
    "vdoheavyland",
    "vdojump",
    "vdoland",
    "vdowalljump",

    "vdoairheavyd",
    "vdoairstriked1",
    "vdogroundstrike1",
    "vdogroundstriked",
    "vdogroundstrikeu1",
    "vdoheavyd",
    "vdoheavyf",
    "vdoheavyu",
};

const array<array<int>> FRAMES = {
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 2, 3, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},

    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},

    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 2, 3, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},

    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},

    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3, 2, 3, 2},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},

    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},

    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3, 2, 3, 2},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},

    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},

    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 2, 3, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},

    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},

    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 2, 3, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},
    {3, 3, 4, 3, 3},

    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},
    {4, 3, 4, 3, 2, 3},

    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3, 2, 3, 2},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},

    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},

    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3, 2, 3, 2},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},
    {3, 2, 3, 2, 3},

    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
    {4, 2, 3, 2, 3, 2},
};

class clone {
    int layer;
    int player_sublayer;
    int effect_sublayer;

    float offset_x = 0;
    float offset_y = 0;

    scene@ g;
    controllable@ p;
    sprites@ spr;

    player_state last_state;
    array<effect@> effects;
    array<hitbox@> attacks;

    clone(scene@ g, controllable@ p, int layer=18, int player_sublayer=10, int effect_sublayer=14) {
        @this.g = g;
        @this.p = p;
        @spr = p.get_sprites();

        this.layer = layer;
        this.player_sublayer = player_sublayer;
        this.effect_sublayer = effect_sublayer;
    }

    void set_controllable(controllable@ p) {
        @this.p = p;
    }

    void set_offset(float x, float y) {
        this.offset_x = x;
        this.offset_y = y;
    }

    void step_post() { 
        for (int i=effects.length()-1; i>=0; --i) {
            effects[i].step();
            if (effects[i].finished) {
                effects.removeAt(i);
            }
        }

        compute_fx();

        compute_attacks();
    }

    void compute_fx() {
        player_state new_state(p);
        dustman@ d = p.as_dustman();

        string c = string(CHARACTER[d.character()]);
        float x = new_state.x + offset_x;
        float y = new_state.y + offset_y;
        int face = new_state.face;

        if (
            (new_state.state == "jump" && last_state.state != "jump") ||
            (new_state.jump_intent == 2 && last_state.jump_intent != 2)
        ) {
            if (last_state.air_charges == new_state.air_charges) {
                if (last_state.ground) {
                    float direction = sign(new_state.x_speed);
                    if (direction == 0) {
                        effects.insertLast(@effect(c+"jump", x, y, face));
                    } else if (direction == new_state.face) {
                        effects.insertLast(@effect(c+"fjump", x, y, face));
                    } else {
                        effects.insertLast(@effect(c+"bjump", x, y, face));
                    }
                } else if (last_state.wall_left) {
                    effects.insertLast(@effect(c+"walljump", x, y+8, -1));
                } else if (last_state.wall_right) {
                    effects.insertLast(@effect(c+"walljump", x, y+8, 1));
                }
            } else {
                effects.insertLast(@effect(c+"dbljump", x, y, 1));
            }
        }

        if (new_state.dash_intent == 2) {
            if (last_state.air_charges <= new_state.air_charges && (last_state.ground || (!last_state.ground && new_state.ground))) {
                effects.insertLast(@effect(c+"dash", x, y, face));
            } else {
                if (last_state.state == "wall_dash" || last_state.state == "wall_run" || last_state.state == "wall_grab" || last_state.state == "wall_grab_idle") {
                    effects.insertLast(@effect(c+"airdash", x + 24*last_state.face, y, last_state.face));
                } else {
                    effects.insertLast(@effect(c+"airdash", x, y, face));
                }
            }
        }

        if (new_state.fall_intent == 2) {
            effects.insertLast(@effect(c+"fastfall", x, y, 1));
        }

        if (new_state.ground && !last_state.ground) {
            if (last_state.y_speed >= 1500.0) {
                effects.insertLast(@effect(c+"heavyland", x, y, 1));
            } else if (new_state.state != "dash") {
                effects.insertLast(@effect(c+"land", x, y, 1));
            }
        }

        if (new_state.state == "roof_run" && last_state.state != "roof_run") {
            effects.insertLast(@effect(c+"dash", x, y-96, -face, 180));
        }

        if (new_state.state == "wall_run" && last_state.state != "wall_run") {
            effects.insertLast(@effect(c+"dash", x+20*face, y-32, face, -90*face));
        }

        last_state = new_state;
    }

    void compute_attacks() {
        for (int i=attacks.length()-1; i>=0; --i) {
            if (@attacks[i] is null || attacks[i].hit_outcome() == 4) {
                attacks.removeAt(i);
                continue;
            }

            if (attacks[i].hit_outcome() == 1 || attacks[i].hit_outcome() == 2 || attacks[i].hit_outcome() == 3) {
                effect@ e;
                dustman@ d = p.as_dustman();
                string c = string(CHARACTER[d.character()]);
                int damage = attacks[i].damage();
                int direction = abs(attacks[i].attack_dir());
                float x = p.x() + offset_x;
                float y = p.y() + offset_y;
                int face = p.attack_face();
                int freeze = attacks[i].hit_outcome() == 2 ? 0 : (damage == 1 ? 2 : 7);
                if (damage == 1) {
                    if (direction == 30) {
                        @e = @effect(c+"groundstrikeu1", x, y, face, 0, freeze);
                    } else if (direction == 85) {
                        @e = @effect(c+"groundstrike1", x, y, face, 0, freeze);
                    } else if (direction == 150) {
                        @e = @effect(c+"airstriked1", x, y, face, 0, freeze);
                    } else if (direction == 151) {
                        @e = @effect(c+"groundstriked", x, y, face, 0, freeze);
                    }
                } else if (damage == 3) {
                    if (direction == 30) {
                        @e = @effect(c+"heavyu", x, y, face, 0, freeze);
                    } else if (direction == 85) {
                        @e = @effect(c+"heavyf", x, y, face, 0, freeze);
                    } else if (direction == 150) {
                        if (p.ground()) {
                            @e = @effect(c+"heavyd", x, y, face, 0, freeze);
                        } else {
                            @e = @effect(c+"airheavyd", x, y, face, 0, freeze);
                        }
                    }
                }
                effects.insertLast(@e);
                attacks.removeAt(i);
            }
        }
    }

    void entity_on_add(entity@ e) {
        hitbox@ h = e.as_hitbox();
        if (h !is null && h.owner().is_same(p)) {
            attacks.insertLast(@h);
        }
    }

    void draw(float subframe) {
        dustman@ d = p.as_dustman();

        // Offset camera subframe nonsense
        camera@ c = get_camera(0);
        float px = p.prev_x() + subframe * (c.x() - c.prev_x());
        float py = p.prev_y() + subframe * (c.y() - c.prev_y());

        // adjust the sprite rotation based on the surface
        float rotation = 0;
        if( p.ground() )
            rotation = p.ground_surface_angle();

        // Draw the player
        if (p.attack_state() == 0) {
            // State timer doesn't loop in fall
            int state_timer = p.state_timer();
            if (p.state() == 5) state_timer %= 4;

            spr.draw_world(layer, player_sublayer, p.sprite_index(), state_timer, 1, px+offset_x, py+offset_y, rotation, p.face(), 1, 0xFFFFFFFF);
        } else {
            spr.draw_world(layer, player_sublayer, p.attack_sprite_index(), p.attack_timer(), 1, px+offset_x, py+offset_y, rotation, p.attack_face(), 1, 0xFFFFFFFF);
        }
        // Draw the effects
        for (int i=0; i<effects.length(); ++i) {
            effects[i].draw(spr, layer, effect_sublayer);
        }
    }
}

class player_state {
    bool ground = true;
    bool wall_left = false;
    bool wall_right = false;
    int air_charges = 0;
    string state = "idle";
    float x_speed = 0.0;
    float y_speed = 0.0;
    float x = 0.0;
    float y = 0.0;
    int jump_intent = 0;
    int dash_intent = 0;
    int fall_intent = 0;
    int face = 1;

    player_state() {}
    player_state(controllable@ p) {
        dustman@ d = p.as_dustman();

        ground = p.ground();
        wall_left = p.wall_left();
        wall_right = p.wall_right();
        air_charges = d.dash();
        state = STATES[p.state()];
        x_speed = p.x_speed();
        y_speed = p.y_speed();
        x = p.x();
        y = p.y();
        jump_intent = p.jump_intent();
        dash_intent = p.dash_intent();
        fall_intent = p.fall_intent();
        face = p.face();
    }
}

class effect {
    string sprite_name;
    int effect_index;
    int face;
    float x;
    float y;
    float rotation;
    int freeze;

    int frame = 0;
    int sprite_index = 0;

    bool finished = false;

    effect(string sprite_name, float x, float y, int face, float rotation=0, int freeze=0) {
        this.sprite_name = sprite_name;
        this.effect_index = EFFECTS.find(sprite_name);
        this.x = x;
        this.y = y;
        this.face = face;
        this.rotation = rotation;
        this.freeze = freeze;
    }

    void step() {
        if (++frame >= FRAMES[effect_index][sprite_index] + (sprite_index == 0 ? freeze : 0)) {
            frame = 0;
            if (++sprite_index >= FRAMES[effect_index].length()) {
                finished = true;
            }
        }
    }

    void draw(sprites@ spr, int layer, int sub_layer) {
        if (!finished) {
            spr.draw_world(layer, sub_layer, sprite_name, sprite_index, 1, x, y, rotation, face, 1, 0xFFFFFFFF);
        }
    }
}
