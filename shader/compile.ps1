glslangValidator.exe -I"." -V -o "principled/brdf.vert.spv" "principled/brdf.vert"
glslangValidator.exe -I"." -V -o "principled/brdf.frag.spv" "principled/brdf.frag"

glslangValidator.exe -I"." -V -o "debug/line.vert.spv" "debug/line.vert"
glslangValidator.exe -I"." -V -o "debug/line.frag.spv" "debug/line.frag"