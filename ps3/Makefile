.SUFFIXES:
ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment: export PSL1GHT=<path>")
endif
include $(PSL1GHT)/ppu_rules

TARGET := poop_game
BUILD := build
SOURCES := source ../src
INCLUDES := include ../src
TITLE := Poop Game
APPID := POOP00001
CONTENTID := UP0001-$(APPID)_00-0000000000000000
CFLAGS = -Wall -mcpu=cell $(MACHDEP) $(INCLUDE)
CXXFLAGS = $(CFLAGS)
LDFLAGS = $(MACHDEP) -Wl,-Map,$(notdir $@).map
LIBS := -lrsx -lgcm_sys -lio -lsysutil -lrt -llv2 -lm

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)
export BUILDDIR := $(CURDIR)/$(BUILD)
CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
export LD := $(CXX)
export OFILES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) -I$(CURDIR)/$(BUILD) $(LIBPSL1GHT_INC)
export LIBPATHS := $(LIBPSL1GHT_LIB)
.PHONY: $(BUILD) clean pkg
$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
clean:
	rm -rf $(BUILD) $(OUTPUT).elf $(OUTPUT).self $(OUTPUT).pkg
pkg: $(BUILD) $(OUTPUT).pkg
else
DEPENDS := $(OFILES:.o=.d)
$(OUTPUT).self: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
