/*
  var long_result = "Target Info: ";
  for (var i in evt) {
    long_result += " (" + i + ":::" + evt[i] + ")  ";
  }
  alert(long_result);
*/            

mergeInto(LibraryManager.library, {
    $emk_info: { objs:[], images:[], image_load_count:0 },

    // Below are a functions that weren't as easy to inline in the C++

    EMK_Tween_Build__deps: ['$emk_info'],
    EMK_Tween_Build: function(target_id, seconds) {
        var obj_id = emk_info.objs.length;
        emk_info.objs[obj_id] = {
            node: emk_info.objs[target_id],
            duration: seconds
        };  // Fill out configuration info.
        emk_info.objs[obj_id+1] = 0; // Preserve for tween.
        return obj_id;        
    },



    // Function to load images into an image object and return the ID.
    EMK_Image_Load__deps: ['$emk_info'],
    EMK_Image_Load: function(file, callback_ptr) {
        file = Pointer_stringify(file);
        var img_id = emk_info.images.length;
        var obj_id = emk_info.objs.length;
        emk_info.images[img_id] = new Image();
        emk_info.images[img_id].src = file;
        emk_info.objs[obj_id] = emk_info.images[img_id]; // Make accessible by obj_id.

        emk_info.images[img_id].onload = function() {
            emk_info.image_load_count += 1;
            emkJSDoCallback(callback_ptr, 0);
        };
        return obj_id;
    },

    EMK_Image_AllLoaded__deps: ['$emk_info'],
    EMK_Image_AllLoaded: function() {
        return (emk_info.images.length == emk_info.image_load_count);
    },

    EMK_Stage_Build__deps: ['$emk_info'],
    EMK_Stage_Build: function(_w, _h, _name) {
        _name = Pointer_stringify(_name);
        var obj_id = emk_info.objs.length;
        emk_info.objs[obj_id] = new Kinetic.Stage({
                container: _name,
                width: _w,
                height: _h
            });

        return obj_id;
    },

    EMK_Stage_AddLayer__deps: ['$emk_info'],
    EMK_Stage_AddLayer: function(stage_obj_id, layer_obj_id) {
        emk_info.objs[stage_obj_id].add(emk_info.objs[layer_obj_id]);  // Add the new layer to the stage.
    },

    EMK_Stage_ResizeMax__deps: ['$emk_info'],
    EMK_Stage_ResizeMax: function(stage_obj_id, min_x, min_y) {
        var new_width = Math.max(window.innerWidth - 10, min_x);
        var new_height = Math.max(window.innerHeight - 10, min_y);
        emk_info.objs[stage_obj_id].setWidth( new_width );
        emk_info.objs[stage_obj_id].setHeight( new_height );
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
    EMK_Image_Build: function(_x, _y, img_id, _w, _h) {
        var obj_id = emk_info.objs.length;
        emk_info.objs[obj_id] = new Kinetic.Image({
            x: _x,
            y: _y,
            image: emk_info.objs[img_id],
            width: _w,
            height: _h
        });
        return obj_id;
    },


    EMK_Rect_Build__deps: ['$emk_info'],
    EMK_Rect_Build: function(_x, _y, _w, _h, _fill, _stroke, _stroke_width, _draggable) {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object.
        _fill = Pointer_stringify(_fill);                // Make sure string values are properly converted (colors)
        _stroke = Pointer_stringify(_stroke);
        emk_info.objs[obj_id] = new Kinetic.Rect({           // Build the new rectangle!
            x: _x,
            y: _y,
            width: _w,
            height: _h,
            fill: _fill,
            stroke: _stroke,
            strokeWidth: _stroke_width,
            draggable: _draggable
        });
        return obj_id;                                       // Return the Kinetic object id.
    },


    EMK_RegularPolygon_Build__deps: ['$emk_info'],
    EMK_RegularPolygon_Build: function(_x, _y, _sides, _radius, _fill, _stroke, _stroke_width, _draggable) {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object.
        _fill = Pointer_stringify(_fill);                // Make sure string values are properly converted (colors)
        _stroke = Pointer_stringify(_stroke);
        emk_info.objs[obj_id] = new Kinetic.RegularPolygon({ // Build the new regular polygon!
                x: _x,
                y: _y,
                sides: _sides,
                radius: _radius,
                fill: _fill,
                stroke: _stroke,
                strokeWidth: _stroke_width,
                draggable: _draggable
            });
        return obj_id;                                       // Return the Kinetic object id.
    },


    EMK_Animation_Build__deps: ['$emk_info'],
    EMK_Animation_Build: function(callback_ptr, layer_id) {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object.
        emk_info.objs[obj_id] = new Kinetic.Animation(function(frame) {
            var ptr= Module._malloc(16); // 4 ints @ 4 bytes each...
            setValue(ptr,    frame.timeDiff,  'i32');
            setValue(ptr+4,  frame.lastTime,  'i32');
            setValue(ptr+8,  frame.time,      'i32');
            setValue(ptr+12, frame.frameRate, 'i32');

            emkJSDoCallback(callback_ptr, ptr);

            Module._free(ptr);
        }, emk_info.objs[layer_id]);
        return obj_id;
    },


    EMK_Animation_Build_NoFrame__deps: ['$emk_info'],
    EMK_Animation_Build_NoFrame: function(callback_ptr, layer_id) {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object.
        emk_info.objs[obj_id] = new Kinetic.Animation(function(frame) {
            emkJSDoCallback(callback_ptr, 0);
        }, emk_info.objs[layer_id]);
        return obj_id;
    },


    EMK_Custom_Shape_Build__deps: ['$emk_info'],
    EMK_Custom_Shape_Build: function(_x, _y, _w, _h, draw_callback) {
        var obj_id = emk_info.objs.length;                   // Determine the next free id for a Kinetic object
        emk_info.objs[obj_id] = new Kinetic.Shape({          // Build the shape!
            x: _x,
            y: _y,
            width: _w,
            height: _h,
            drawFunc: function(canvas) {                     // For an arbitrary shape, we just have a draw function
                emk_info.canvas = canvas;
                emk_info.ctx = canvas._context;              // WTF??  This should be canvas.getContext();
                emkJSDoCallback(draw_callback, 0);
                emk_info.canvas = null;
                emk_info.ctx = null;
            }
        });
        return obj_id;                                       // Return the Kinetic object id.
    },
  

    EMK_Shape_SetDrawFunction__deps: ['$emk_info'],
    EMK_Shape_SetDrawFunction: function(obj_id, callback_ptr) {
        emk_info.objs[obj_id].setDrawFunc( function(_ctx) {
            emk_info.ctx = _ctx._context;
            emkJSDoCallback(callback_ptr, 0);
            emk_info.ctx = null;
        } );
    },


    EMK_Cursor_Set__deps: ['$emk_info'],
    EMK_Cursor_Set: function(cur_type) {
        cur_type  = Pointer_stringify(cur_type);
        document.body.style.cursor = cur_type;
    },


    // Stand-alone functions
    EMK_ScheduleCallback__deps: ['$emk_info'],
    EMK_ScheduleCallback: function(callback_ptr, delay) {
        window.setTimeout(function() { emkJSDoCallback(callback_ptr); }, delay);
    }
});
