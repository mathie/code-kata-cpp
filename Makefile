# $Id: Makefile,v 1.2 2004/02/02 08:32:04 mathie Exp $
#
# $Log: Makefile,v $
# Revision 1.2  2004/02/02 08:32:04  mathie
# * Add trailing CR, RCS keywords.
#

BOOST_HOME = $(HOME)/src/not-mine/tarballs/boost-1.30.2

CPPFLAGS = -I$(BOOST_HOME)

all: kata2
