# $Id: Makefile,v 1.3 2004/02/02 09:08:15 mathie Exp $
#
# $Log: Makefile,v $
# Revision 1.3  2004/02/02 09:08:15  mathie
# * Add options to control build process (switch on warnings, optional
#   debugging symbols/optimisation).
# * Factor addition of targets out to the start of the Makefile.
# * Target to clean everything up.
#
# Revision 1.2  2004/02/02 08:32:04  mathie
# * Add trailing CR, RCS keywords.
#

TARGETS = kata2
DEBUG = yes

BOOST_HOME = $(HOME)/src/not-mine/tarballs/boost-1.30.2

# Default to linking C++ instead of C
LINK.o = $(CXX) $(LDFLAGS) $(TARGET_ARCH)

CXXFLAGS = -Wall

ifeq ($(DEBUG),)
CXXFLAGS += -O2
else
CXXFLAGS += -O0 -g
endif

CPPFLAGS = -I$(BOOST_HOME)

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.o *~

# Dependencies
kata2: kata2.o
