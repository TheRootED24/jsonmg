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
PKG-BRANCH=openwrt
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2024-07-29
			
PKG_SOURCE_VERSION:=aa85a8a28cb8d48cb7b2f1846b44b0ec7e0d1ca1

PKG_MIRROR_HASH:=23cc4cbaf579a2ba9872605f809b8e380d09e78fe927a655be09cb4c9e7e29a9

PKG_MAINTAINER:=hostle <TheRootED24@gmail.com>
PKG_LICENSE:=MIT License

PKG_BUILD_FLAGS:=lto

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/jsonmg
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=OpenWrt jsonmg Library
  DEPENDS:=+mongoose +liblua
endef

TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/jsonmg/

define Package/jsonmg/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/jsonmg
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ipkg-install/usr/lib/lua/jsonmg/jsonmg.so $(1)/usr/lib/lua/jsonmg
endef

$(eval $(call BuildPackage,jsonmg))
