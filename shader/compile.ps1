glslangValidator -I"." -V -o chunk/editor.comp.spv chunk/editor.comp
glslangValidator -I"." -V -o chunk/generator.comp.spv chunk/generator.comp
glslangValidator -I"." -V -o chunk/mipmap.comp.spv chunk/mipmap.comp
glslangValidator -I"." -V -o chunk/renderer.vert.spv chunk/renderer.vert
glslangValidator -I"." -V -o chunk/renderer.frag.spv chunk/renderer.frag

glslangValidator -I"." -V -o debug/line.vert.spv debug/line.vert
glslangValidator -I"." -V -o debug/line.frag.spv debug/line.frag