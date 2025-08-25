glslangValidator.exe -I"./shader" -V -o "shader/chunk/editor.comp.spv" "shader/chunk/editor.comp"
glslangValidator.exe -I"./shader" -V -o "shader/chunk/generator.comp.spv" "shader/chunk/generator.comp"
glslangValidator.exe -I"./shader" -V -o "shader/chunk/mipmap.comp.spv" "shader/chunk/mipmap.comp"
glslangValidator.exe -I"./shader" -V -o "shader/chunk/renderer.vert.spv" "shader/chunk/renderer.vert"
glslangValidator.exe -I"./shader" -V -o "shader/chunk/renderer.frag.spv" "shader/chunk/renderer.frag"

glslangValidator.exe -I"./shader" -V -o "shader/debug/line.vert.spv" "shader/debug/line.vert"
glslangValidator.exe -I"./shader" -V -o "shader/debug/line.frag.spv" "shader/debug/line.frag"