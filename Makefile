#
# Copyright (C) 2021 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=jsonmg
PKG_RELEASE:=1

PKG_SOURCE_URL=https://github.com/TheRootED24/jsonmg.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2024-07-27
			
PKG_SOURCE_VERSION:=357f9f7daf33b459c0c773d6439bdb92b0a56ea0

PKG_MIRROR_HASH:=2379169fe055ab67c61bdf74f8d46287a7b6405d0c90746f8b8272ebfaa67d0f

PKG_MAINTAINER:=hostle <TheRootED24@gmail.com>
PKG_LICENSE:=MIT License

PKG_BUILD_FLAGS:=lto

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/jsonmg
  SECTION:=net
  CATEGORY:=Network
  TITLE:=OpenWrt jsonmg Library
  DEPENDS:=+libmongoose +liblua
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include

define Package/jsonmg/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/jsonmg
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/lib/lua/jsonmg/jsonmg.so $(1)/usr/lib/lua/jsonmg
endef

$(eval $(call BuildPackage,jsonmg))
