clang snowball2.c glad_gl.c -Ofast -lglfw -lm -o snowball2
i686-w64-mingw32-gcc snowball2.c glad_gl.c -Ofast -Llib -lglfw3dll -lm -o snowball2.exe
upx snowball2
