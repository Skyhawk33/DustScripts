
entity@ add_entity(scene@ g, string type, float x, float y) {
    return add_entity(g, type, x, y, false);
}

entity@ add_entity(scene@ g, string type, float x, float y, bool debug) {
    
    entity@ new_entity = create_entity(type);
    entity@ new_AI = create_entity("AI_controller");
    
    //positions
    new_entity.x(x);
    new_entity.y(y);
    new_AI.x(x);
    new_AI.y(y);
    
    g.add_entity(new_entity);
    g.add_entity(new_AI);
    
    //add a node to the path
    vararray@ nodes = new_AI.vars().get_var("nodes").get_array();
    nodes.resize(1);
    nodes.at(0).set_vec2(x, y);
    
    //give each node a default wait time
    new_AI.vars().get_var("nodes_wait_time").get_array().resize(1);
    
    //link the controller to the entity
    new_AI.vars().get_var("puppet_id").set_int32(new_entity.id());
    
    if(debug) {
        puts(entity_string(new_entity));
        puts(entity_string(new_AI));
    }
    
    return new_entity;
}

string entity_string(entity@ e) {
    return entity_string(e, "");
}

string entity_string(entity@ e, string prefix) {
    string res = "";
    
    res +=prefix+e.type_name()+"    id="+e.id()+"\n";
    res +=prefix+"  ( "+e.x()+", "+e.y()+" )\n";
    res += varstruct_string(e.vars(),prefix+"  ");
    
    return res;
}

string varstruct_string(varstruct@ v) {
    return varstruct_string(v, "");
}

string varstruct_string(varstruct@ v, string prefix) {
    string res = prefix+"<struct>:\n";
    
    prefix += "  ";
    
    for(uint i=0; i < v.num_vars(); i++){
        res += prefix+i+": "+v.var_name(i)+"\n";
        res += varvalue_string(v.get_var(i), prefix+"  ");
    }
    
    return res;
}

string vararray_string(vararray@ v) {
    return vararray_string(v, "");
}

string vararray_string(vararray@ v, string prefix) {
    string res = prefix+"<array<"+type_id_string(v.element_type_id())+">>:  \n";
    
    prefix += "  ";
    
    for(uint i=0; i < v.size(); i++){
        res += prefix+i+": "+varvalue_string(v.at(i), "");
    }
    
    return res;
}

string type_id_string(int type_id) {
    switch( type_id ) {
        //var_type_none=0,
        case 0:
            return "none";
        //var_type_bool=1,
        case 1:
            return "bool";
        //var_type_int8=2,
        case 2:
            return "int8";
        //var_type_int16=3,
        case 3:
            return "int16";
        //var_type_int32=4,
        case 4:
            return "int32";
        //var_type_int64=5,
        case 5:
            return "int64";
        //var_type_float=6,
        case 6:
            return "float";
        //var_type_string=7,
        case 7:
            return "string";
        //var_type_array=8,
        case 8:
            return "array";
        //var_type_struct=9,
        case 9:
            return "struct";
        //var_type_vec2=10,
        case 10:
            return "vec2";
        default:
            return "";
    }
    return "";
}

string varvalue_string(varvalue@ v, string prefix) {
    string res = "";
    
    switch( v.type_id() ) {
        //var_type_none=0,
        case 0:
            res += prefix+"<none>\n";
            break;
        //var_type_bool=1,
        case 1:
            res += prefix+"<bool>: "+v.get_bool()+"\n";
            break;
        //var_type_int8=2,
        case 2:
            res += prefix+"<i_8 >: "+v.get_int8()+"\n";
            break;
        //var_type_int16=3,
        case 3:
            res += prefix+"<i_16>: "+v.get_int16()+"\n";
            break;
        //var_type_int32=4,
        case 4:
            res += prefix+"<i_32>: "+v.get_int32()+"\n";
            break;
        //var_type_int64=5,
        case 5:
            res += prefix+"<i_64>: "+v.get_int64()+"\n";
            break;
        //var_type_float=6,
        case 6:
            res += prefix+"<flt >: "+v.get_float()+"\n";
            break;
        //var_type_string=7,
        case 7:
            res += prefix+"<str >: "+v.get_string()+"\n";
            break;
        //var_type_array=8,
        case 8:
            res += vararray_string(v.get_array(), prefix);
            break;
        //var_type_struct=9,
        case 9:
            res += varstruct_string(v.get_struct(), prefix);
            break;
        //var_type_vec2=10,
        case 10:
            res += prefix+"<vec2>: ( "+v.get_vec2_x()+", "+v.get_vec2_y()+" )\n";
            break;
    }
    return res;
}
