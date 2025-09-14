glslangValidator.exe -I"." -V -o "chunk/editor.comp.spv" "chunk/editor.comp"
glslangValidator.exe -I"." -V -o "chunk/generator.comp.spv" "chunk/generator.comp"
glslangValidator.exe -I"." -V -o "chunk/mipmap.comp.spv" "chunk/mipmap.comp"
glslangValidator.exe -I"." -V -o "chunk/renderer.vert.spv" "chunk/renderer.vert"
glslangValidator.exe -I"." -V -o "chunk/renderer.frag.spv" "chunk/renderer.frag"

glslangValidator.exe -I"." -V -o "debug/line.vert.spv" "debug/line.vert"
glslangValidator.exe -I"." -V -o "debug/line.frag.spv" "debug/line.frag"