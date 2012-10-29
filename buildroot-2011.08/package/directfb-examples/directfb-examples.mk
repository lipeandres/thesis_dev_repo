#############################################################
#
# directfb examples
#
#############################################################
DIRECTFB_EXAMPLES_VERSION:=1.2.0
DIRECTFB_EXAMPLES_SITE:=http://www.directfb.org/downloads/Extras
DIRECTFB_EXAMPLES_SOURCE = DirectFB-examples-$(DIRECTFB_EXAMPLES_VERSION).tar.gz
DIRECTFB_EXAMPLES_INSTALL_STAGING = YES
DIRECTFB_EXAMPLES_INSTALL_TARGET = YES
DIRECTFB_EXAMPLES_DEPENDENCIES = directfb

DIRECTFB_EXAMPLES_TARGETS_ :=
DIRECTFB_EXAMPLES_TARGETS_y :=

DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_ANDI) += usr/bin/df_andi
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_BLTLOAD) += usr/bin/df_bltload
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_CPULOAD) += usr/bin/df_cpuload
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DATABUFFER) += usr/bin/df_databuffer
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DIOLOAD) += usr/bin/df_dioload
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/bin/df_dok
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DRIVERTEST) += usr/bin/df_drivertest
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_FIRE) += usr/bin/df_fire
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_FLIP) += usr/bin/df_flip
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_FONTS) += usr/bin/df_fonts
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_INPUT) += usr/bin/df_input
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_JOYSTICK) += usr/bin/df_joystick
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_KNUCKLES) += usr/bin/df_knuckles
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_LAYER) += usr/bin/df_layer
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_NEO) += usr/bin/df_neo
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_NETLOAD) += usr/bin/df_netload
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_PALETTE) += usr/bin/df_palette
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_PARTICLE) += usr/bin/df_particle
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_PORTER) += usr/bin/df_porter
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_STRESS) += usr/bin/df_stress
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_TEXTURE) += usr/bin/df_texture
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_VIDEO) += usr/bin/df_video
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_VIDEO_PARTICLE) += usr/bin/df_video_particle
DIRECTFB_EXAMPLES_TARGETS_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_WINDOW) += usr/bin/df_window

DIRECTFB_EXAMPLES_DATA_ :=
DIRECTFB_EXAMPLES_DATA_y :=

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_ANDI) += usr/share/directfb-examples/destination_mask.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_ANDI) += usr/share/directfb-examples/tux.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_ANDI) += usr/share/directfb-examples/wood_andi.jpg

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/share/directfb-examples/biglogo.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/share/directfb-examples/card.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/share/directfb-examples/melted.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/share/directfb-examples/meter.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/share/directfb-examples/pngtest*.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/share/directfb-examples/colorkeyed.gif
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/share/directfb-examples/intro.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/share/directfb-examples/fish.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DOK) += usr/share/directfb-examples/swirl.png

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DRIVERTEST) += usr/share/directfb-examples/pngtest.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_DRIVERTEST) += usr/share/directfb-examples/testmask.png

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_INPUT) += usr/share/directfb-examples/joystick.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_INPUT) += usr/share/directfb-examples/gnu-keys.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_INPUT) += usr/share/directfb-examples/gnome-mouse.png

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_NEO) += usr/share/directfb-examples/apple-red.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_NEO) += usr/share/directfb-examples/gnome-*.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_NEO) += usr/share/directfb-examples/gnu-*.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_NEO) += usr/share/directfb-examples/background*.jpg

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_STRESS) += usr/share/directfb-examples/melted.png

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_TEXTURE) += usr/share/directfb-examples/texture.png

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_VIDEO) += usr/share/directfb-examples/shot.png

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_VIDEO_PARTICLE) += usr/share/directfb-examples/convergence.png

DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_WINDOW) += usr/share/directfb-examples/desktop.png
DIRECTFB_EXAMPLES_DATA_$(BR2_PACKAGE_DIRECTFB_EXAMPLES_WINDOW) += usr/share/directfb-examples/dfblogo.png

DIRECTFB_EXAMPLES_FONT_y :=
ifeq ($(BR2_PACKAGE_DIRECTFB_EXAMPLES_FONTS),y)
DIRECTFB_EXAMPLES_FONT_y = usr/share/directfb-examples/fonts/*
endif

define DIRECTFB_EXAMPLES_INSTALL_TARGET_CMDS
	$(Q)mkdir -p $(TARGET_DIR)/usr/bin
	$(Q)mkdir -p $(TARGET_DIR)/usr/share/directfb-examples/
	$(Q)mkdir -p $(TARGET_DIR)/usr/share/directfb-examples/fonts/
	$(Q)for file in $(DIRECTFB_EXAMPLES_TARGETS_y); do \
	        cp -dpf $(STAGING_DIR)/$$file $(TARGET_DIR)/$$file; \
		$(STRIPCMD) $(STRIP_STRIP_UNNEEDED) $(TARGET_DIR)/$$file; \
	done
	$(Q)cp -rdpf $(STAGING_DIR)/usr/share/directfb-examples/fonts/decker.ttf $(TARGET_DIR)/usr/share/directfb-examples/fonts/
	$(Q)for file in $(DIRECTFB_EXAMPLES_FONT_y); do \
		$(INSTALL) -m 0644 $(STAGING_DIR)/$$file $(TARGET_DIR)/usr/share/directfb-examples/fonts/; \
	done
	$(Q)$(INSTALL) -m 0755 -d $(TARGET_DIR)/usr/share/directfb-examples
	$(Q)for file in $(DIRECTFB_EXAMPLES_DATA_y); do \
		$(INSTALL) -m 0644 $(STAGING_DIR)/$$file $(TARGET_DIR)/usr/share/directfb-examples; \
	done
endef

$(eval $(call AUTOTARGETS,package,directfb-examples))

