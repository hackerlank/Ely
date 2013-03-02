ACLOCAL_AMFLAGS = -I m4

if BOOST_TEST
BOOSTTESTDIR = src/test
endif

if TRAINING_TEST
TRAININGTESTDIR = training
endif

SUBDIRS = $(BOOSTTESTDIR) include callbacks transitions initializations tinyxml2 src doc $(TRAININGTESTDIR) 

pkgdata_DATA = game.xml config.prc
