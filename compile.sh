#!/bin/tcsh
python /Users/charles/Local/emsdk_portable/emscripten/1.16.0/emcc --js-library library_kinetic.js -Werror test.cc -o test.js -s EXPORTED_FUNCTIONS="['_emkMain', '_emkJSDoCallback']"
cp test.js /Users/charles/Sites/EmCanvas/
