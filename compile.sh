#!/bin/tcsh
python /Users/charles/Local/emsdk_portable/emscripten/1.16.0/emcc --js-library libs/library_kinetic.js -Werror -DEMK_DEBUG $1.cc -o $1.js -s EXPORTED_FUNCTIONS="['_emkMain', '_emkJSDoCallback']"
cp $1.js /Users/charles/Sites/EmCanvas/
