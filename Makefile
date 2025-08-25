build ?= build

c ?= gcc
cflags += -c -Wall -Werror -MMD -Iinclude -g
lflags += -lvulkan

src := \
	src/entry.c \
	src/window.c \
	src/shaders.c \
	src/vulkan.c \
	src/vkapp.c \
	\
	src/backend/wayland/loader.c \
	src/backend/wayland/backend.c \
	src/backend/wayland/registry.c \
	src/backend/wayland/surface.c \
	src/backend/wayland/wayland.c \
	\
	src/vulkan/instance.c \
	src/vulkan/physical.c \
	src/vulkan/device.c \
	src/vulkan/surface.c \
	src/vulkan/swapchain.c \
	src/vulkan/shaders.c \
	src/vulkan/pipeline_layouts.c \
	src/vulkan/render_passes.c \
	src/vulkan/framebuffers.c \
	src/vulkan/pipelines.c \
	src/vulkan/command_pools.c \
	src/vulkan/command_buffers.c \
	src/vulkan/semaphores.c \
	src/vulkan/fences.c \
	src/vulkan/commands.c \
	src/vulkan/buffers.c \
	src/vulkan/memories.c \
	src/vulkan/maps.c \


obj = $(addprefix $(build)/,$(src:.c=.o))
dep = $(obj:.o=.d)

sc ?= glslc
scflags += -MD
slflags +=

ssrc := \
	shaders/base_ui.vert \
	shaders/base_ui.frag \


sobj = $(addprefix $(build)/,$(addsuffix .spv,$(ssrc)))
sdep = $(addsuffix .d,$(sobj))

.PHONY: src
src: $(build)/vkapp

.PHONY: watch
watch:
	@watchexec -w . -e .c,.h -- 'make -Bnwk src | compiledb'

$(build)/vkapp: $(obj)
	@mkdir -p $(dir $@)
	$(c) -o $@ $^ $(lflags)

$(build)/%.o: %.c
	@mkdir -p $(dir $@)
	$(c) -o $@ $< $(cflags)

src/shaders.c: $(sobj) include/core/shaders.h
	@echo -e "#include \"core/shaders.h\"\n\nconst char *const *embed_get_shaders() {\n    static const char *const list[] = {" > $@;
	@for spv in $(sobj); do \
		content=$$(xxd -p $$spv | tr -d '\n' | sed 's/../\\\\x&/g'); \
		echo -e "        \"$${content}\"," >> $@; \
	done
	@echo -e "    };\n\n    return list;\n}\n\nconst unsigned *embed_get_shader_sizes() {\n    static const unsigned sizes[] = {" >> $@;
	@for spv in $(sobj); do \
		size=$$(stat -c '%s' $$spv); \
		echo -e "        $${size}," >> $@; \
	done
	@echo -e "    };\n\n    return sizes;\n}\n" >> $@;

include/core/shaders.h: $(sobj)
	@echo -e "#pragma once\n\nenum {" > $@;
	@for spv in $(ssrc); do \
		name=$$(echo EMBED_$${spv} | tr '[:lower:]' '[:upper:]' | tr '/' '_' | tr '.' '_'); \
		echo -e "    $${name}," >> $@; \
	done
	@echo -e "    EMBED_SHADERS_COUNT,\n};\n\nconst char *const *embed_get_shaders();\nconst unsigned *embed_get_shader_sizes();\n" >> $@;

$(build)/%.spv: %
	@mkdir -p $(dir $@)
	$(sc) -o $@ $< $(scflags) $(slflags)

-include $(dep)
-include $(sdep)
