glslangValidator.exe -I"." -V -o "brdf/brdf.vert.spv" "brdf/brdf.vert"
glslangValidator.exe -I"." -V -o "brdf/brdf.frag.spv" "brdf/brdf.frag"

glslangValidator.exe -I"." -V -o "debug/line.vert.spv" "debug/line.vert"
glslangValidator.exe -I"." -V -o "debug/line.frag.spv" "debug/line.frag"