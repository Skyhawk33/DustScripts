
//a base script must be included to test the classes.
// IMPORTANT: COMMENT THIS LINE OUT BEFORE INCLUDING THIS FILE IN OTHER SCRIPTS

//class script {}

[hidden]
class cloneable_trigger_base : trigger_base {
    [boolean] bool copy;
    
    cloneable_trigger_base() {
        copy = false;
    }
    
    void editor_step() {
        if(copy){
            cloneable_trigger_base@ ct = clone();
            if(ct !is null) {
                scripttrigger@ st = create_scripttrigger(ct);
                if(st !is null) {
                    camera@ c = get_camera(0);
                    st.set_xy(c.x(), c.y());
                    get_scene().add_entity(st.as_entity());
                }
                else
                    puts("failed to create script trigger");
            }
            else
                puts("no cloned object was returned");
            copy = false;
        }
    }
    
    //overwrite this to make the trigger copyable
    cloneable_trigger_base@ clone() {
        return null;
    }
}

/**
class test_trigger : cloneable_trigger_base {
    [text] int data;
    
    void editor_step() {
        //a call to cloneable_trigger_base's editor_step must be included if editor_step is overwritten
        cloneable_trigger_base::editor_step();
    }
    
    void init(script@ s, scripttrigger@ self) {
        puts("initialized!");
    }
    
    cloneable_trigger_base@ clone() {
        puts("cloning!");
        test_trigger@ tt = test_trigger();
        tt.data = data;
        return tt;
    }
}
/**/