/*
  var long_result = "Target Info: ";
  for (var i in evt) {
    long_result += " (" + i + ":::" + evt[i] + ")  ";
  }
  alert(long_result);
*/            

mergeInto(LibraryManager.library, {
    $emk_info: { objs:[], images:[], image_load_count:0 },

    EMK_Alert: function(msg) {
        msg = Pointer_stringify(msg);
        alert(msg);
    },




    EMK_Object_Draw__deps: ['$emk_info'],
    EMK_Object_Draw: function (obj_id) {
        emk_info.objs[obj_id].draw();
    },

    EMK_Object_MoveToTop__deps: ['$emk_info'],
    EMK_Object_MoveToTop: function (obj_id) {
        emk_info.objs[obj_id].moveToTop();
    },


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

    EMK_Tween_Configure__deps: ['$emk_info'],
    EMK_Tween_Configure: function(settings_id, obj_id) {
        emk_info.objs[obj_id] = new Kinetic.Tween( emk_info.objs[settings_id] );
    },

    EMK_Tween_SetFinishedCallback__deps: ['$emk_info'],
    EMK_Tween_SetFinishedCallback: function(settings_id, callback_ptr, info_ptr) {
        emk_info.objs[settings_id].onFinish = function() {
            emkJSDoCallback(callback_ptr, info_ptr);
        }
    },

    EMK_Tween_SetX__deps: ['$emk_info'],
    EMK_Tween_SetX: function(settings_id, _x) {
        emk_info.objs[settings_id].x = _x;
    },

    EMK_Tween_SetY__deps: ['$emk_info'],
    EMK_Tween_SetY: function(settings_id, _y) {
        emk_info.objs[settings_id].y = _y;
    },

    EMK_Tween_SetXY__deps: ['$emk_info'],
    EMK_Tween_SetXY: function(settings_id, _x, _y) {
        emk_info.objs[settings_id].x = _x;
        emk_info.objs[settings_id].y = _y;
    },

    EMK_Tween_SetScaleX__deps: ['$emk_info'],
    EMK_Tween_SetScaleX: function(settings_id, _x) {
        emk_info.objs[settings_id].scaleX = _x;
    },

    EMK_Tween_SetScaleY__deps: ['$emk_info'],
    EMK_Tween_SetScaleY: function(settings_id, _y) {
        emk_info.objs[settings_id].scaleY = _y;
    },

    EMK_Tween_SetScaleXY__deps: ['$emk_info'],
    EMK_Tween_SetScaleXY: function(settings_id, _x, _y) {
        emk_info.objs[settings_id].scaleX = _x;
        emk_info.objs[settings_id].scaleY = _y;
    },

    EMK_Tween_Play__deps: ['$emk_info'],
    EMK_Tween_Play: function(obj_id) {
        emk_info.objs[obj_id].play();
    },


    // The following series of wrappers deal with the canvas context.
    // NOTE, we assume that a context has been placed in emk_info.ctx
    EMK_Canvas_SetFillStyle__deps: ['$emk_info'],
    EMK_Canvas_SetFillStyle: function(fs) {
        fs = Pointer_stringify(fs);
        emk_info.ctx.fillStyle = fs;
    },

    EMK_Canvas_SetStroke__deps: ['$emk_info'],
    EMK_Canvas_SetStroke: function(_stroke) {
        _stroke = Pointer_stringify(_stroke);
        emk_info.ctx.strokeStyle = _stroke;
    },

    EMK_Canvas_SetLineJoin__deps: ['$emk_info'],
    EMK_Canvas_SetLineJoin: function(lj) {
        lj = Pointer_stringify(lj);
        emk_info.ctx.lineJoin = lj;
    },

    EMK_Canvas_SetLineWidth__deps: ['$emk_info'],
    EMK_Canvas_SetLineWidth: function(lw) {
        emk_info.ctx.lineWidth = lw;
    },

    EMK_Canvas_SetFont__deps: ['$emk_info'],
    EMK_Canvas_SetFont: function(font) {
        font = Pointer_stringify(font);
        emk_info.ctx.font = font;
    },

    EMK_Canvas_SetTextAlign__deps: ['$emk_info'],
    EMK_Canvas_SetTextAlign: function(ta) {
        ta = Pointer_stringify(ta);
        emk_info.ctx.textAlign = ta;
    },

    EMK_Canvas_SetShadowColor__deps: ['$emk_info'],
    EMK_Canvas_SetShadowColor: function(sc) {
        sc = Pointer_stringify(sc);
        emk_info.ctx.shadowColor = sc;
    },

    EMK_Canvas_SetShadowBlur__deps: ['$emk_info'],
    EMK_Canvas_SetShadowBlur: function(blur) {
        emk_info.ctx.shadowBlur = blur;
    },

    EMK_Canvas_SetShadowOffsetX__deps: ['$emk_info'],
    EMK_Canvas_SetShadowOffsetX: function(offset_x) {
        emk_info.ctx.shadowOffsetX = offset_x;
    },

    EMK_Canvas_SetShadowOffsetY__deps: ['$emk_info'],
    EMK_Canvas_SetShadowOffsetY: function(offset_y) {
        emk_info.ctx.shadowOffsetY = offset_y;
    },

    EMK_Canvas_FillText__deps: ['$emk_info'],
    EMK_Canvas_FillText: function(msg, x, y) {
        msg = Pointer_stringify(msg);
        emk_info.ctx.fillText(msg, x, y);
    },

    EMK_Canvas_StrokeText__deps: ['$emk_info'],
    EMK_Canvas_StrokeText: function(msg, x, y) {
        msg = Pointer_stringify(msg);
        emk_info.ctx.strokeText(msg, x, y);
    },

    EMK_Canvas_FillRect__deps: ['$emk_info'],
    EMK_Canvas_FillRect: function(x, y, width, height) {
        emk_info.ctx.fillRect(x, y, width, height);
    },

    EMK_Canvas_StrokeRect__deps: ['$emk_info'],
    EMK_Canvas_StrokeRect: function(x, y, width, height) {
        emk_info.ctx.strokeRect(x, y, width, height);
    },

    EMK_Canvas_Arc__deps: ['$emk_info'],
    EMK_Canvas_Arc: function(x, y, radius, start, end, cclockwise) {
        emk_info.ctx.arc(x, y, radius, start, end, cclockwise);
    },

    EMK_Canvas_DrawImage__deps: ['$emk_info'],
    EMK_Canvas_DrawImage: function(obj_id, x, y) {
        emk_info.ctx.drawImage(emk_info.objs[obj_id], x, y)
    },

    EMK_Canvas_DrawImage_Size__deps: ['$emk_info'],
    EMK_Canvas_DrawImage_Size: function(obj_id, x, y, w, h) {
        emk_info.ctx.drawImage(emk_info.objs[obj_id], x, y, w, h)
    },

    EMK_Canvas_BeginPath__deps: ['$emk_info'],
    EMK_Canvas_BeginPath: function() {
        emk_info.ctx.beginPath();
    },

    EMK_Canvas_ClosePath__deps: ['$emk_info'],
    EMK_Canvas_ClosePath: function() {
        emk_info.ctx.closePath();
    },

    EMK_Canvas_Fill__deps: ['$emk_info'],
    EMK_Canvas_Fill: function() {
        emk_info.ctx.fill();
    },

    EMK_Canvas_LineTo__deps: ['$emk_info'],
    EMK_Canvas_LineTo: function(x, y) {
        emk_info.ctx.lineTo(x, y);
    },

    EMK_Canvas_MoveTo__deps: ['$emk_info'],
    EMK_Canvas_MoveTo: function(x, y) {
        emk_info.ctx.moveTo(x, y);
    },

    EMK_Canvas_Restore__deps: ['$emk_info'],
    EMK_Canvas_Restore: function() {
        emk_info.ctx.restore();
    },

    EMK_Canvas_Save__deps: ['$emk_info'],
    EMK_Canvas_Save: function() {
        emk_info.ctx.save();
    },

    EMK_Canvas_Scale__deps: ['$emk_info'],
    EMK_Canvas_Scale: function(x, y) {
        emk_info.ctx.scale(x, y);
    },

    EMK_Canvas_Translate__deps: ['$emk_info'],
    EMK_Canvas_Translate: function(x, y) {
        emk_info.ctx.translate(x, y);
    },

    EMK_Canvas_Rotate__deps: ['$emk_info'],
    EMK_Canvas_Rotate: function(angle) {
        emk_info.ctx.rotate(angle);
    },

    EMK_Canvas_Stroke__deps: ['$emk_info'],
    EMK_Canvas_Stroke: function() {
        emk_info.ctx.stroke();
    },

    EMK_Canvas_SetupTarget__deps: ['$emk_info'],
    EMK_Canvas_SetupTarget: function(obj_id) {
        emk_info.canvas.fillStrokeShape(emk_info.objs[obj_id]);
    },


    // Function to load images into an image object and return the ID.
    EMK_Image_Load__deps: ['$emk_info'],
    EMK_Image_Load: function(file, callback_ptr) {
        file = Pointer_stringify(file);
        img_id = emk_info.images.length;
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


    EMK_Animation_Start__deps: ['$emk_info'],
    EMK_Animation_Start: function(obj_id) {
        emk_info.objs[obj_id].start();
    },


    EMK_Animation_Stop__deps: ['$emk_info'],
    EMK_Animation_Stop: function(obj_id) {
        emk_info.objs[obj_id].stop();
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
  

    EMK_Shape_SetCornerRadius__deps: ['$emk_info'],
    EMK_Shape_SetCornerRadius: function(obj_id, radius) {
        emk_info.objs[obj_id].cornerRadius(radius);
    },

    EMK_Shape_SetFillPatternImage__deps: ['$emk_info'],
    EMK_Shape_SetFillPatternImage: function(obj_id, img_id) {
        emk_info.objs[obj_id].setFillPriority('pattern');
        emk_info.objs[obj_id].setFillPatternImage(emk_info.objs[img_id]);
    },

    EMK_Shape_SetFillPatternScale__deps: ['$emk_info'],
    EMK_Shape_SetFillPatternScale: function(obj_id, scale) {
        emk_info.objs[obj_id].fillPatternScaleX(scale);
        emk_info.objs[obj_id].fillPatternScaleY(scale);
    },

    EMK_Shape_SetLineJoin__deps: ['$emk_info'],
    EMK_Shape_SetLineJoin: function(obj_id, join_type) {
        join_type = Pointer_stringify(join_type);
        emk_info.objs[obj_id].lineJoin(join_type);
    },
    
    EMK_Shape_SetOffset__deps: ['$emk_info'],
    EMK_Shape_SetOffset: function(obj_id, x_offset, y_offset) {
        emk_info.objs[obj_id].offset({x:x_offset, y:y_offset});
    },
    
    EMK_Shape_SetScale__deps: ['$emk_info'],
    EMK_Shape_SetScale: function(obj_id, x_scale, y_scale) {
        emk_info.objs[obj_id].scale({x:x_scale, y:y_scale});
    },

    EMK_Shape_SetStroke__deps: ['$emk_info'],
    EMK_Shape_SetStroke: function(obj_id, color) {
        color = Pointer_stringify(color);
        emk_info.objs[obj_id].stroke(color); // @CAO Test!
    },
    
    EMK_Shape_DoRotate__deps: ['$emk_info'],
    EMK_Shape_DoRotate: function(obj_id, rot) {
        emk_info.objs[obj_id].rotate(rot);
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
