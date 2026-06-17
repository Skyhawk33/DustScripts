const int SPRITE_COUNT = 10;
const string EMBED_sprite0 = 'sprite.png';
const string EMBED_sprite1 = 'sprite.png';
const string EMBED_sprite2 = 'sprite.png';
const string EMBED_sprite3 = 'sprite.png';
const string EMBED_sprite4 = 'sprite.png';
const string EMBED_sprite5 = 'sprite.png';
const string EMBED_sprite6 = 'sprite.png';
const string EMBED_sprite7 = 'sprite.png';
const string EMBED_sprite8 = 'sprite.png';
const string EMBED_sprite9 = 'sprite.png';


class Prop {
    [text] uint sprite_index = 0;
    [text] uint layer=15;
    [text] uint sublayer=20;
    [position,mode:world,layer:15,y:"y"] int x=-1531;
    [hidden] int y=-540;
    [text] float x_scale=3;
    [text] float y_scale=3;
    [angle] int rotation=0;
    [colour, alpha] uint color=0xFFFFFFFF;
}


class script: callback_base {
    scene@ g;
    sprites@ spr;

    [text] array<Prop> prop;

    script() {
        @spr = create_sprites();
    }

    void build_sprites(message@ msg) {
        for(int i=0; i< SPRITE_COUNT; i++)
            msg.set_string("sprite"+i,"sprite"+i);
    }

    void draw(float subframe) {
      spr.add_sprite_set("script");
      for(int i=0; i< prop.length; i++)
        spr.draw_world(prop[i].layer, prop[i].sublayer, "sprite"+prop[i].sprite_index, 0, 0, prop[i].x, prop[i].y, prop[i].rotation, prop[i].x_scale, prop[i].y_scale, prop[i].color);
    }

    void editor_draw(float subframe) {
      draw(subframe);
    }

    void on_level_start() {
      spr.add_sprite_set("script");
    }

    void step(int)
    {
       spr.add_sprite_set("script");
    }

}
