CC := g++ 
SRCDIR := ./src
BUILDDIR := ./build

BUILD_MODELS := models
RUN_CORRECTOR := corrector
 
SRCEXT := cc
INCEXT := h

BUILD_MODELS_SOURCES := build_models.cc language_model.cc utils.cc edit_cost_model.cc noisy_channel.cc 
RUN_CORRECTOR_SOURCES := run_corrector.cc corrector.cc utils.cc edit_cost_model.cc noisy_channel.cc language_model.cc scorer.cc

BUILD_MODELS_S := $(addprefix $(SRCDIR)/,$(BUILD_MODELS_SOURCES))
RUN_CORRECTOR_S := $(addprefix $(SRCDIR)/,$(RUN_CORRECTOR_SOURCES))

BUILD_MODELS_OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(BUILD_MODELS_S:.$(SRCEXT)=.o))
RUN_CORRECTOR_OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(RUN_CORRECTOR_S:.$(SRCEXT)=.o))

# CFLAGS := -O1 -Wall -Wextra -pedantic -ansi -g
CFLAGS := -O1 -Wall -pg -g -std=c++11
LIB := -L lib -lboost_thread -lboost_system -lboost_program_options
INC := -I ./include

all: $(BUILD_MODELS) $(RUN_CORRECTOR)

$(BUILD_MODELS): $(BUILD_MODELS_OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(BUILD_MODELS) $(LIB)"; $(CC) $^ -o $(BUILD_MODELS) $(LIB)

$(RUN_CORRECTOR): $(RUN_CORRECTOR_OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(RUN_CORRECTOR) $(LIB)"; $(CC) $^ -o $(RUN_CORRECTOR) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
