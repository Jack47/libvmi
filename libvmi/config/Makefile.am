h_sources = config_parser.h
c_sources = grammar.y lexicon.l

CLEANFILES = lexicon.c grammar.c grammar.h
AM_CPPFLAGS = -I$(top_srcdir)
# csd 2013-4-9 AM_YFLAGS = -y -d
AM_YFLAGS = -d

noinst_LTLIBRARIES= libconfig.la
libconfig_la_SOURCES= $(h_sources) $(c_sources)
