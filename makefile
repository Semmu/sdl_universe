#
#	VARIABLES AND SETTINGS
#

# compiler
CC := g++
# compilation flags
CFLAGS := -O -Wall -std=c++11 $(EXTRAFLAGS)
LFLAGS :=

# source files
SRCDIR := src
SRCEXT := cpp
SRCS := $(wildcard $(SRCDIR)/*.$(SRCEXT)) $(wildcard $(SRCDIR)/SU/*.$(SRCEXT))

# building
BUILDDIR := build
OBJS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRCS:.$(SRCEXT)=.o))
SDLVERSION := SDL

# output
OUTPUTDIR := bin
TARGET := SDL_Universe



#
#	SDL1 version
#

sdl1: SDLVERSION += 1
sdl1: CFLAGS += `sdl-config --cflags` -DUSING_SDL1
sdl1: LFLAGS += `sdl-config --libs` -lSDL_ttf
sdl1: run



#
#	SDL2 version
#

sdl2: SDLVERSION += 2
sdl2: CFLAGS += `sdl2-config --cflags` -DUSING_SDL2
sdl2: LFLAGS += `sdl2-config --libs` -lSDL2_ttf
sdl2: clean
sdl2: run



#
#	RULES
#

run: $(OUTPUTDIR)/$(TARGET)
	@echo ""
	@echo "### RUNNING VERSION" $(SDLVERSION)
	@echo -n "    "
	./$(OUTPUTDIR)/$(TARGET)

# linking rule
$(OUTPUTDIR)/$(TARGET): $(OBJS)
	@echo ""
	@echo "### LINKING" $@ "WITH" $(SDLVERSION)
	@echo -n "    "
	$(CC) $^ $(LFLAGS) -o $(OUTPUTDIR)/$(TARGET)

# compilation rule
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo ""
	@echo "### COMPILING" $@ "FOR" $(SDLVERSION)
	@echo -n "    "
	$(CC) $(CFLAGS) -I include -c -o $@ $<

# cleanup
clean:
	@echo ""
	@echo "### CLEANING"
	@echo -n "    "
	$(RM) -r $(BUILDDIR)/*.o
	@echo -n "    "
	$(RM) -r $(BUILDDIR)/SU/*.o
	@echo -n "    "
	$(RM) -r $(OUTPUTDIR)/$(TARGET)

# variable print for debug purposes
debug-%:
	@echo $* = $($*)

.PHONY: clean