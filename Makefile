include_directory = -Ivendor -Isrc 
warnings = -Wall -Wextra -Wpedantic
#-Werror -Wfatal-errors

objects = objects/glad_gl.o \
          objects/stb_image.o\
          objects/stb_truetype.o\
          objects/nestbtt.o \
		  objects/nejson.o

src = src/obscwindow.c \
	  src/obscgraphic.c \
	  src/obscfs.c \
	  src/obscui.c \
	  src/obsclanguage.c \
	  src/obscapp.c \

debug_flags = -D_DEBUG -O0 

release_flags = -O3

libs = -lglfw3

ifeq ($(OS),Windows_NT)
	libs += -lgdi32 -lwinmm
	release_flags += -mwindows
else
	libs += -lm
endif

precompile:
	gcc -O3 -std=c99 -DSTB_TRUETYPE_IMPLEMENTATION -fexceptions -xc -g -c vendor/stb/stb_truetype.h -o objects/stb_truetype.o
	gcc -O3 -std=c99 -DNESTBTT_IMPLEMENTATION -fexceptions -xc -g -c vendor/stb/nestbtt.h -o objects/nestbtt.o
	gcc -O3 -std=c99 -DSTB_IMAGE_IMPLEMENTATION -fexceptions -xc -g -c vendor/stb/stb_image.h -o objects/stb_image.o
	gcc -O3 -std=c99 -DGLAD_GL_IMPLEMENTATION -fexceptions -xc -g -c vendor/glad/gl.h -o objects/glad_gl.o
	gcc -O3 -std=c99 -DNEJSON_IMPLEMENTATION -fexceptions -xc -g -c vendor/nelib/nejson.h -o objects/nejson.o

nepacker:
	gcc -O3 -std=c99 $(warnings) src/nepacker.c -o nepacker

pack_resources:
	make nepacker
	./nepacker r resources/gl_shader.vs src/gl_shader.vs.h
	./nepacker r resources/gl_shader.fs src/gl_shader.fs.h
	./nepacker rb resources/logo.png src/logo.png.h
	./nepacker rb resources/icon.png src/icon.png.h
	./nepacker rb resources/Iosevka-Heavy.ttf src/Iosevka-Heavy.ttf.h

debug:
	gcc $(debug_flags) -std=c99 $(warnings) $(src) src/main.c $(include_directory) -o obsc_d $(objects) $(libs)
	./obsc_d

release:
	make precompile
	make pack_resources
	gcc $(release_flags) -std=c99 $(warnings) $(src) src/main.c $(include_directory) -o obsc $(objects) $(libs)