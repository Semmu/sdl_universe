#
#	VARIABLES AND SETTINGS
#

# compiler
CC := g++
# compilation flags
CFLAGS := -O -Wall -std=c++11 `sdl-config --cflags` -DUSING_SDL1
LFLAGS := `sdl-config --libs` -lSDL_ttf

# source files
SRCDIR := src
SRCEXT := cpp
SRCS := $(wildcard $(SRCDIR)/*.$(SRCEXT)) $(wildcard $(SRCDIR)/SU/*.$(SRCEXT))

# building
BUILDDIR := build
OBJS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SRCS:.$(SRCEXT)=.o))

# output
OUTPUTDIR := bin
TARGET := SDL_Universe




#
#	RULES
#

run: $(TARGET)
	@echo ""
	@echo "### RUNNING"
	@echo -n "    "
	./$(TARGET)

# linking rule
$(TARGET): $(OBJS)
	@echo ""
	@echo "### LINKING" $@
	@echo -n "    "
	$(CC) $^ $(LFLAGS) -o $(TARGET)

# compilation rule
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo ""
	@echo "### COMPILING" $@
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
	$(RM) -r $(TARGET)

# variable print for debug purposes
debug-%:
	@echo $* = $($*)

.PHONY: clean