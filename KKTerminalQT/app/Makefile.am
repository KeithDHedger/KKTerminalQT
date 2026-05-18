AUTOMAKE_OPTIONS = subdir-objects
include ../flagsandlibs
include ../sources

kkterminalqt_SOURCES = $(COMMONSRC)

pixfilesdir = $(pkgdatadir)/pixmaps
pixfiles_DATA = ../resources/pixmaps/*

userthemesdir = $(pkgdatadir)/themes
userthemes_DATA = ../resources/themes/*

bin_PROGRAMS = kkterminalqt

man1_MANS = ../resources/man/kkterminalqt.1

install-exec-hook:
	install -D ../resources/pixmaps/KKTerminalQT.png $(DESTDIR)${prefix}/share/pixmaps/KKTerminalQT.png
	install -D ../resources/pixmaps/KKTerminalQT.desktop $(DESTDIR)${prefix}/share/applications/KKTerminalQT.desktop
	cp -r "../resources/icons" "$(DESTDIR)$(pkgdatadir)"

uninstall-hook:
	rm -f $(DESTDIR)${prefix}/share/applications/KKTerminalQT.desktop
	rm -f $(DESTDIR)${prefix}/share/pixmaps/KKTerminalQT.png
	rm -fr $(pkgdatadir)

