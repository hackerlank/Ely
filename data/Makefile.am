SUBDIRS = models shaders sounds textures

dist_pkgdata_DATA = game.xml config.prc

EXTRA_DIST = config.prc.in game.xml.in

if !ELY_DEVEL
substDataDir = sed -e 's|@elydatadir[@]|$(pkgdatadir)|g'
else
substDataDir = sed -e 's|@elydatadir[@]|$(abs_top_srcdir)|g'
endif 

config.prc : $(top_srcdir)/data/config.prc.in Makefile
	$(substDataDir) $(top_srcdir)/data/$@.in > $@

game.xml : $(top_srcdir)/data/game.xml.in Makefile
	$(substDataDir) $(top_srcdir)/data/$@.in > $@

CLEANFILES = config.prc game.xml
		