/*
  var long_result = "Target Info: ";
  for (var i in evt) {
    long_result += " (" + i + ":::" + evt[i] + ")  ";
  }
  alert(long_result);
*/            

mergeInto(LibraryManager.library, {
    $emk_info: { objs:[], images:[], image_load_count:0 },

    EMK_Alert: function(in_msg) {
        in_msg = Pointer_stringify(in_msg);
        alert(in_msg);
    },


    EMK_Setup_OnEvent__deps: ['$emk_info'],
    EMK_Setup_OnEvent: function(obj_id, trigger, callback_ptr) {
        trigger = Pointer_stringify(trigger);
        emk_info.objs[obj_id].on(trigger, function() {
            emkJSDoCallback(callback_ptr);
        });
    },

    EMK_Setup_OnEvent_Info__deps: ['$emk_info'],
    EMK_Setup_OnEvent_Info: function(obj_id, trigger, callback_ptr) {
        trigger = Pointer_stringify(trigger);
        emk_info.objs[obj_id].on(trigger, function(event) {
            var evt = event.evt;
            var ptr = Module._malloc(32); // 8 ints @ 4 bytes each...
            setValue(ptr,    evt.layerX,   'i32');
            setValue(ptr+4,  evt.layerY,   'i32');
            setValue(ptr+8,  evt.button,   'i32');
            setValue(ptr+12, evt.keyCode,  'i32');
            setValue(ptr+16, evt.altKey,   'i32');
            setValue(ptr+20, evt.ctrlKey,  'i32');
            setValue(ptr+24, evt.metaKey,  'i32');
            setValue(ptr+28, evt.shiftKey, 'i32');

            emkJSDoCallback(callback_ptr, ptr);
            Module._free(ptr);

        });
    },


    EMK_Object_Destroy__deps: ['$emk_info'],
    EMK_Object_Destroy: function(obj_id) {
        if (obj_id >= 0) emk_info.objs[obj_id].destroy();
    },


    EMK_Object_GetX__deps: ['$emk_info'],
    EMK_Object_GetX: function (obj_id) {
        return emk_info.objs[obj_id].x();
    },
    EMK_Object_GetY__deps: ['$emk_info'],
    EMK_Object_GetY: function (obj_id) {
        return emk_info.objs[obj_id].y();
    },
    EMK_Object_GetWidth__deps: ['$emk_info'],
    EMK_Object_GetWidth: function (obj_id) {
        return emk_info.objs[obj_id].width();
    },
    EMK_Object_GetHeight__deps: ['$emk_info'],
    EMK_Object_GetHeight: function (obj_id) {
        return emk_info.objs[obj_id].height();
    },
    EMK_Object_GetVisible__deps: ['$emk_info'],
    EMK_Object_GetVisible: function (obj_id) {
        return emk_info.objs[obj_id].visible;
    },
    EMK_Object_GetOpacity__deps: ['$emk_info'],
    EMK_Object_GetOpacity: function (obj_id) {
        return emk_info.objs[obj_id].opacity;
    },
    EMK_Object_GetListening__deps: ['$emk_info'],
    EMK_Object_GetListening: function (obj_id) {
        return emk_info.objs[obj_id].listening;
    },
    EMK_Object_GetScaleX__deps: ['$emk_info'],
    EMK_Object_GetScaleX: function (obj_id) {
        return emk_info.objs[obj_id].scaleX;
    },
    EMK_Object_GetScaleY__deps: ['$emk_info'],
    EMK_Object_GetScaleY: function (obj_id) {
        return emk_info.objs[obj_id].scaleY;
    },
    EMK_Object_GetOffsetX__deps: ['$emk_info'],
    EMK_Object_GetOffsetX: function (obj_id) {
        return emk_info.objs[obj_id].offsetX;
    },
    EMK_Object_GetOffsetY__deps: ['$emk_info'],
    EMK_Object_GetOffsetY: function (obj_id) {
        return emk_info.objs[obj_id].offsetY;
    },
    EMK_Object_GetRotation__deps: ['$emk_info'],
    EMK_Object_GetRotation: function (obj_id) {
        return emk_info.objs[obj_id].rotation;
    },
    EMK_Object_GetDraggable__deps: ['$emk_info'],
    EMK_Object_GetDraggable: function (obj_id) {
        return emk_info.objs[obj_id].draggable;
    },

    EMK_Object_SetX__deps: ['$emk_info'],
    EMK_Object_SetX: function(obj_id, x) {
        emk_info.objs[obj_id].x(x);
    },

    EMK_Object_SetY__deps: ['$emk_info'],
    EMK_Object_SetY: function(obj_id, y) {
        emk_info.objs[obj_id].y(y);
    },
 
    EMK_Object_SetXY__deps: ['$emk_info'],
    EMK_Object_SetXY: function(obj_id, x, y) {
        emk_info.objs[obj_id].x(x);
        emk_info.objs[obj_id].y(y);
    },

    EMK_Object_SetWidth__deps: ['$emk_info'],
    EMK_Object_SetWidth: function(obj_id, w) {
        emk_info.objs[obj_id].width(w);
    },

    EMK_Object_SetHeight__deps: ['$emk_info'],
    EMK_Object_SetHeight: function(obj_id, h) {
        emk_info.objs[obj_id].height(h);
    },

    EMK_Object_SetSize__deps: ['$emk_info'],
    EMK_Object_SetSize: function(obj_id, w, h) {
        emk_info.objs[obj_id].width(w);
        emk_info.objs[obj_id].height(h);
    },

    EMK_Object_SetVisible__deps: ['$emk_info'],
    EMK_Object_SetVisible: function(obj_id, visible) {
        emk_info.objs[obj_id].visible(visible);
    },

    EMK_Object_SetOpacity__deps: ['$emk_info'],
    EMK_Object_SetOpacity: function(obj_id, opacity) {
        emk_info.objs[obj_id].opacity(opacity);
    },

    EMK_Object_SetListening__deps: ['$emk_info'],
    EMK_Object_SetListening: function(obj_id, listening) {
        emk_info.objs[obj_id].listening(listening);
    },

    EMK_Object_SetRotation__deps: ['$emk_info'],
    EMK_Object_SetRotation: function(obj_id, rotation) {
        emk_info.objs[obj_id].rotation(rotation);
    },

    EMK_Object_SetDraggable__deps: ['$emk_info'],
    EMK_Object_SetDraggable: function(obj_id, draggable) {
        emk_info.objs[obj_id].draggable(draggable);
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

    EMK_Tween_SetXY__deps: ['$emk_info'],
    EMK_Tween_SetXY: function(settings_id, _x, _y) {
        emk_info.objs[settings_id].x = _x;
        emk_info.objs[settings_id].y = _y;
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
    EMK_Image_Build: function(in_x, in_y, img_id, in_w, in_h) {
        var obj_id = emk_info.objs.length;
        emk_info.objs[obj_id] = new Kinetic.Image({
            x: in_x,
            y: in_y,
            image: emk_info.objs[img_id],
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

    EMK_Text_SetText__deps: ['$emk_info'],
    EMK_Text_SetText: function(obj_id, in_text) {
        in_text = Pointer_stringify(in_text);
        emk_info.objs[obj_id].text(in_text);
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
        emk_info.objs[obj_id].setDrawFunc( function(in_ctx) {
            emk_info.ctx = in_ctx._context;
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
