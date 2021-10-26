all:
  clang snowball2.c glad_gl.c -Ofast -lglfw -lm -o snowball

install:
	cp snowball $(DESTDIR)

uninstall:
	rm $(DESTDIR)/snowball

clean:
	rm snowball
