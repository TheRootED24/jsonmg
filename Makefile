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
			
PKG_SOURCE_VERSION:=5b15820b0b43c7d10a1686702bc31f07f9e09621

PKG_MIRROR_HASH:=f7415123260d6c2f735dddba2fb3dfc283a0c50dd81a8b6f538a133b6f964879

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
