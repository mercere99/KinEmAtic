mergeInto(LibraryManager.library, {
    $emk_info: { objs:[], images:[], image_load_count:0 },

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


    // Function to load images into an image object and return the ID.
    EMK_Image_Load__deps: ['$emk_info'],
    EMK_Image_Load: function(file, callback_id) {
        file = Pointer_stringify(file);
        img_id = emk_info.images.length;
        emk_info.images[img_id] = new Image();
        emk_info.images[img_id].src = file;

        emk_info.images[img_id].onload = function() {
            emk_info.image_load_count += 1;
            emkJSDoCallback(callback_id);
        };
        return img_id;
    },

    EMK_Image_IsLoaded__deps: ['$emk_info'],
    EMK_Image_IsLoaded: function(img_id) {
        return emk_info.image[img_id].loaded;
    },

    EMK_Image_AllLoaded__deps: ['$emk_info'],
    EMK_Image_AllLoaded: function() {
        alert("Testing! " + emk_info.images.length + "  " + emk_info.image_load_count);
        return (emk_info.images.length == emk_info.image_load_count);
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

    EMK_Image_Build__deps: ['$emk_info'],
    EMK_Image_Build: function(in_x, in_y, img_id, in_w, in_h) {
        var obj_id = emk_info.objs.length;
        emk_info.objs[obj_id] = new Kinetic.Image({
            x: in_x,
            y: in_y,
            image: emk_info.image[img_id],
            width: in_w,
            height: in_h
        });
        return obj_id;
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


    EMK_Animation_Start__deps: ['$emk_info'],
    EMK_Animation_Start: function(obj_id) {
        emk_info.objs[obj_id].start();
    },


    EMK_Shape_SetFillPatternImage__deps: ['$emk_info'],
    EMK_Shape_SetFillPatternImage: function(obj_id, img_id) {
        delete emk_info.objs[obj_id].fill; // Delete fill, otherwise fill overwrite image.
        // emk_info.objs[obj_id].fill('red'); // Delete fill, otherwise fill overwrite image.
        emk_info.objs[obj_id].fillPatternImage(emk_info.images[img_id]);
    },

    EMK_Shape_SetOffset__deps: ['$emk_info'],
    EMK_Shape_SetOffset: function(obj_id, x_offset, y_offset) {
        emk_info.objs[obj_id].offset({x:x_offset, y:y_offset});
    },
    
    EMK_Shape_SetScale__deps: ['$emk_info'],
    EMK_Shape_SetScale: function(obj_id, x_scale, y_scale) {
        emk_info.objs[obj_id].scale({x:x_scale, y:y_scale});
    },

    EMK_Shape_DoRotate__deps: ['$emk_info'],
    EMK_Shape_DoRotate: function(obj_id, rot) {
        emk_info.objs[obj_id].rotate(rot);
    }
});
