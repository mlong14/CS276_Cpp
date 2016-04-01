CC := g++ 
SRCDIR := ./src
BUILDDIR := ./build

INDEX := index
QUERY := query
 
SRCEXT := cc
INCEXT := h

INDEX_SOURCES := index.cc basic_index.cc pa1.cc utils.cc
QUERY_SOURCES := query.cc basic_index.cc pa1.cc utils.cc

INDEX_S := $(addprefix $(SRCDIR)/,$(INDEX_SOURCES))
QUERY_S := $(addprefix $(SRCDIR)/,$(QUERY_SOURCES))

INDEX_OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(INDEX_S:.$(SRCEXT)=.o))
QUERY_OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(QUERY_S:.$(SRCEXT)=.o))

CFLAGS := -O0 -Wall -g
LIB := -L lib
INC := -I ./include

all: $(INDEX) $(QUERY)

$(INDEX): $(INDEX_OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(INDEX) $(LIB)"; $(CC) $^ -o $(INDEX) $(LIB)

$(QUERY): $(QUERY_OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(QUERY) $(LIB)"; $(CC) $^ -o $(QUERY) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
