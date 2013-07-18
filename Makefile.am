pkgdata_DATA = data/world.opw data/font.png

bin_PROGRAMS = openpandemic
openpandemic_SOURCES = src/font.c src/land.c src/main.c src/sphere.c src/texture.c src/vertex.c src/world_loader.c

openpandemic_CFLAGS = $(GL_CFLAGS) $(GLU_CFLAGS) $(GLEW_CFLAGS) $(SDL2_CFLAGS) $(SDL2_IMAGE_CFLAGS)
openpandemic_LDADD = $(GL_LIBS) $(GLU_LIBS) $(GLEW_LIBS) $(SDL2_LIBS) $(SDL2_IMAGE_LIBS) -lm
openpandemic_CPPFLAGS = -DDATADIR=\"$(pkgdatadir)\"
