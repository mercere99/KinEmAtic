//"use strict";

// An implementation of a libc for the web. Basically, implementations of
// the various standard C libraries, that can be called from compiled code,
// and work using the actual JavaScript environment.
//
// We search the Library object when there is an external function. If the
// entry in the Library is a function, we insert it. If it is a string, we
// do another lookup in the library (a simple way to write a function once,
// if it can be called by different names). We also allow dependencies,
// using __deps. Initialization code to be run after allocating all
// global constants can be defined by __postset.
//
// Note that the full function name will be '_' + the name in the Library
// object. For convenience, the short name appears here. Note that if you add a
// new function with an '_', it will not be found.

// Memory allocated during startup, in postsets, should only be ALLOC_STATIC

mergeInto(LibraryManager.library, {
    $emk_info: { objs:[], counter:0 },

    EMK_Alert: function(in_msg) {
        in_msg = Pointer_stringify(in_msg);
        alert(in_msg);
    },


    EMK_Setup_OnEvent__deps: ['$emk_info'],
    EMK_Setup_OnEvent: function(obj_id, trigger, callback_id) {
        trigger = Pointer_stringify(trigger);
        emk_info.objs[obj_id].on(trigger, function() {
            emkJSDoCallback(callback_id); // @CAO Will callback change for all if outer function run again? Test!
        });
    },


    EMK_Stage_Build__deps: ['$emk_info'],
    EMK_Stage_Build: function(in_w, in_h, in_name) {
        in_name = Pointer_stringify(in_name);
        var obj_id = emk_info.objs.length;
        emk_info.objs[obj_id] = new Kinetic.Stage({
                container: in_name,
                width: in_w,
                height: in_h
            });

        return obj_id;
    },

    EMK_Stage_AddLayer__deps: ['$emk_info'],
    EMK_Stage_AddLayer: function(stage_obj_id, layer_obj_id) {
        emk_info.objs[stage_obj_id].add(emk_info.objs[layer_obj_id]);  // Add the new layer to the stage.
    },

    EMK_Layer_Build__deps: ['$emk_info'],
    EMK_Layer_Build: function() {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object.
        emk_info.objs[obj_id] = new Kinetic.Layer();         // Build the new layer and save it as a Kinetic object.
        return obj_id;                                       // Return the Kinetic object id.
    },

    EMK_Layer_AddObject__deps: ['$emk_info'],
    EMK_Layer_AddObject: function(layer_obj_id, add_obj_id) 
    {
        emk_info.objs[layer_obj_id].add(emk_info.objs[add_obj_id]);
    },

    EMK_Layer_BatchDraw__deps: ['$emk_info'],
    EMK_Layer_BatchDraw: function(layer_obj_id)
    {
        emk_info.objs[layer_obj_id].batchDraw();
    },

    EMK_Text_Build__deps: ['$emk_info'],
    EMK_Text_Build: function(in_x, in_y, in_text, in_font_size, in_font_family, in_fill) {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object.
        in_text = Pointer_stringify(in_text);                // Make sure string values are properly converted (text)
        in_font_size = Pointer_stringify(in_font_size);      // Make sure string values are properly converted (size)
        in_font_family = Pointer_stringify(in_font_family);  // Make sure string values are properly converted (font)
        in_fill = Pointer_stringify(in_fill);                // Make sure string values are properly converted (color)
        
        emk_info.objs[obj_id] = new Kinetic.Text({           // Build the new text object!
                x: in_x,
                y: in_y,
                text: in_text,
                fontSize: in_font_size,
                fontFamily: in_font_family,
                fill: in_fill
            });
        return obj_id;                                       // Return the Kinetic object id.
    },


    EMK_Rect_Build__deps: ['$emk_info'],
    EMK_Rect_Build: function(in_x, in_y, in_w, in_h, in_fill, in_stroke, in_stroke_width, in_draggable) {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object.
        in_fill = Pointer_stringify(in_fill);                // Make sure string values are properly converted (colors)
        in_stroke = Pointer_stringify(in_stroke);
        emk_info.objs[obj_id] = new Kinetic.Rect({           // Build the new rectangle!
            x: in_x,
            y: in_y,
            width: in_w,
            height: in_h,
            fill: in_fill,
            stroke: in_stroke,
            strokeWidth: in_stroke_width,
            draggable: in_draggable
        });
        return obj_id;                                       // Return the Kinetic object id.
    },


    EMK_RegularPolygon_Build__deps: ['$emk_info'],
    EMK_RegularPolygon_Build: function(in_x, in_y, in_sides, in_radius, in_fill, in_stroke, in_stroke_width, in_draggable) {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object.
        in_fill = Pointer_stringify(in_fill);                // Make sure string values are properly converted (colors)
        in_stroke = Pointer_stringify(in_stroke);
        emk_info.objs[obj_id] = new Kinetic.RegularPolygon({ // Build the new regular polygon!
                x: in_x,
                y: in_y,
                sides: in_sides,
                radius: in_radius,
                fill: in_fill,
                stroke: in_stroke,
                strokeWidth: in_stroke_width,
                draggable: in_draggable
            });
        return obj_id;                                       // Return the Kinetic object id.
    },


    EMK_Animation_Build__deps: ['$emk_info'],
    EMK_Animation_Build: function(callback_id, layer_id) {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object.
        emk_info.objs[obj_id] = new Kinetic.Animation(function(frame) {
            emk_info.anim_frame = frame;
            emkJSDoCallback(callback_id); // @CAO Will callback change for all if outer function run again? Test!
        }, emk_info.objs[layer_id]);
        return obj_id;
    },

    EMK_AnimationFrame_GetTime__deps: ['$emk_info'],
    EMK_AnimationFrame_GetTime: function() { return emk_info.anim_frame.time; },

    EMK_AnimationFrame_GetTimeDiff__deps: ['$emk_info'],
    EMK_AnimationFrame_GetTimeDiff: function() { return emk_info.anim_frame.timeDiff; },

    EMK_AnimationFrame_GetLastTime__deps: ['$emk_info'],
    EMK_AnimationFrame_GetLastTime: function() { return emk_info.anim_frame.lastTime; },

    EMK_AnimationFrame_GetFrameRate__deps: ['$emk_info'],
    EMK_AnimationFrame_GetFrameRate: function() { return emk_info.anim_frame.frameRate; },


    EMK_Animation_Start: ['$emk_info'],
    EMK_Animation_Start: function(obj_id) {
        emk_info.objs[obj_id].start();
    },


    EMK_Shape_SetScale__deps: ['$emk_info'],
    EMK_Shape_SetScale: function(obj_id, x_scale, y_scale) {
        emk_info.objs[obj_id].scale({x:x_scale, y:y_scale});
    },

    EMK_Shape_SetOffset__deps: ['$emk_info'],
    EMK_Shape_SetOffset: function(obj_id, x_offset, y_offset) {
        emk_info.objs[obj_id].offset({x:x_offset, y:y_offset});
    },
    
    EMK_Shape_DoRotate__deps: ['$emk_info'],
    EMK_Shape_DoRotate: function(obj_id, rot) {
        emk_info.objs[obj_id].rotate(rot);
    }
});
