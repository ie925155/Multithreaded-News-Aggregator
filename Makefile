#
# Based on Makefiles previously written by Julie Zelenski.
#

CXX = g++

# The CXXFLAGS variable sets compile flags for gcc:
#  -g                         compile with debug information
#  -Wall                      give all diagnostic warnings
#  -pedantic                  require compliance with ANSI standard
#  -O0                        do not optimize generated code
#  -std=c++0x                 go with the c++0x extensions for thread support, unordered maps, etc
#  -D_GLIBCXX_USE_NANOSLEEP   included for this_thread::sleep_for and this_thread::sleep_until support
#  -D_GLIBCXX_USE_SCHED_YIELD included for this_thread::yield support
#CXXFLAGS = -g -Wall -pedantic -O0 -std=c++0x -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD -I/usr/class/cs110/include/libxml2 -I/usr/class/cs110/local/include
CXXFLAGS = -g -Wall -pedantic -O0 -std=c++0x -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD -I/usr/include/libxml2/ -I/usr/include

# The LDFLAGS variable sets flags for linker
#  -lm       link in libm (math library)
#  -lpthread link in libpthread (thread library) to back C++11 extensions
#  -lrand    link to simple random number generator
#  -lthreads link to convenience functions layering over C++11 threads
#  -lxml2    link in libxml2 (XML processing library)
#LDFLAGS = -lm -lpthread -L/usr/class/cs110/local/lib -lrand -lthreads -L/usr/class/cs110/lib/libxml2 -lxml2
#LDFLAGS = -lm -lpthread -L/usr/lib -lrand -lthreads -L/usr/include/libxml2/ -lxml2
LDFLAGS = -lm -lpthread -L/usr/lib -L/usr/include/libxml2/ -lxml2

# In this section, you list the files that are part of the project.
# If you add/change names of header/source files, here is where you
# edit the Makefile.
SOURCES = \
	news-aggregator.cc \
	news-aggregator-utils.cc \
	stream-tokenizer.cc \
	rss-feed.cc \
	rss-feed-list.cc \
	html-document.cc \
	rss-index.cc \
	html-test.cc \
	thread-pool.cc \
	thread-pool-test.cc \
	ostreamlock.cc \

HEADERS = \
	article.h \
	news-aggregator-utils.h \
	stream-tokenizer.h \
	rss-feed.h \
	rss-feed-exception.h \
	rss-feed-list.h \
	rss-feed-list-exception.h \
	html-document.h \
	html-document-exception.h \
	rss-index.h \
	thread-pool.h \
	ostreamlock.h \

OBJECTS = $(SOURCES:.cc=.o)
TARGETS = news-aggregator thread-pool-test

default: $(TARGETS)

thread-pool-test: thread-pool-test.o thread-pool.o ostreamlock.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

news-aggregator: news-aggregator.o rss-feed.o rss-feed-list.o rss-index.o html-document.o stream-tokenizer.o news-aggregator-utils.o \
	thread-pool.o ostreamlock.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# In make's default rules, a .o automatically depends on its .c file
# (so editing the .c will cause recompilation into its .o file).
# The line below creates additional dependencies, most notably that it
# will cause the .c to reocmpiled if any included .h file changes.

Makefile.dependencies:: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -MM $(SOURCES) > Makefile.dependencies

-include Makefile.dependencies

# Phony means not a "real" target, it doesn't build anything
# The phony target "clean" that is used to remove all compiled object files.

.PHONY: clean spartan

clean:
	@rm -f $(TARGETS) $(OBJECTS) core Makefile.dependencies news-aggregator_soln

spartan: clean
	@rm -f *~ .*~
