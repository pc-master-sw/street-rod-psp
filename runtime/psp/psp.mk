TARGET = street_rod
PSP_EBOOT_TITLE = Street Rod
SR_SRC = engine/srport/src
SR_SOURCES = $(wildcard $(SR_SRC)/*.c $(SR_SRC)/*/*.c)
SR_OBJS = $(addprefix sr_,$(notdir $(SR_SOURCES:.c=.o)))
OBJS = main.o port_common.o port_psp.o engine_host.o port_keyboard.o $(SR_OBJS)
INCDIR = $(PROJECT_ROOT)/runtime/include $(SR_SRC) $(PROJECT_ROOT)/games/street_rod
CFLAGS = -std=gnu11 -O2 -G0 -Wall -Wextra -Werror -Wno-misleading-indentation -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections
LIBS = -lpspgu -lpspaudio -lpsppower -lm
BUILD_PRX = 1
PSP_LARGE_MEMORY = 0
EXTRA_TARGETS = EBOOT.PBP
PSPSDK := $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

main.o: $(PROJECT_ROOT)/games/street_rod/engine_main.c
	$(CC) $(CFLAGS) -c $< -o $@
engine_host.o: $(PROJECT_ROOT)/games/street_rod/engine_host.c
	$(CC) $(CFLAGS) -c $< -o $@
port_common.o: $(PROJECT_ROOT)/runtime/common/port_common.c
	$(CC) $(CFLAGS) -c $< -o $@
port_keyboard.o: $(PROJECT_ROOT)/runtime/common/port_keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@
port_psp.o: $(PROJECT_ROOT)/runtime/psp/port_psp.c
	$(CC) $(CFLAGS) -c $< -o $@

define build_engine_object
sr_$(notdir $(1:.c=.o)): $(1)
	$$(CC) $$(filter-out -Werror,$$(CFLAGS)) -fno-strict-aliasing -Wno-unused-parameter -Wno-format-truncation -c $$< -o $$@
endef
$(foreach source,$(SR_SOURCES),$(eval $(call build_engine_object,$(source))))
