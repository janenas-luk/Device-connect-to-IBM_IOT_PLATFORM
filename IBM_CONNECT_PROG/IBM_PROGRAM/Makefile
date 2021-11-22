include $(TOPDIR)/rules.mk

PKG_NAME:=IBM_PROGRAM
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/IBM_PROGRAM
	CATEGORY:=Base system
	TITLE:=IBM_PROGRAM
	DEPENDS:=+libubus +libubox +libblobmsg-json +libibm +luci-app-ibm
endef

define Package/IBM_PROGRAM/description
	This is a ibm app package
endef

define Package/IBM_PROGRAM/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/IBM_PROGRAM $(1)/usr/bin
	$(INSTALL_BIN) ./files/ibm_conf.init $(1)/etc/init.d/ibm_conf
	$(INSTALL_CONF) ./files/ibm_conf.config $(1)/etc/config/ibm_conf
endef

$(eval $(call BuildPackage,IBM_PROGRAM))
