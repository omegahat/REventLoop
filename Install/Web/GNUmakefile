OMEGA_HOME=../../../..


include $(OMEGA_HOME)/Config/GNUmakefile.SWebPackageConfig
include $(OMEGA_HOME)/R/Config/RPackageWebConfig


CSS_FILES=$(OMEGA_HOME)/Docs/OmegaTech.css

CHANGES=../Changes.html
AUX_DOCS=$(BUGS) $(TODO) $(FAQ) $(CHANGES)

OTHER_DOCS=../INSTALL.html ../README.html pkg/man.pdf

PKG_TAR_FILE=../../$(TAR_FILE)

pkg/man: $(PKG_TAR_FILE)
	if [ ! -d pkg ] ; then mkdir pkg  ; fi
	(cd  pkg ; tar zxvf ../$(PKG_TAR_FILE) )


../../Docs/html/struct__R__EventLoop.html: ../../Reventloop.h
	(cd ../.. ; doxygen)

install: pkg/$(PKG_NAME)/inst/Docs index.html examples pkg/man.pdf ../../Docs/html/struct__R__EventLoop.html
	scp -r index.html examples  $(OTHER_DOCS) pkg/$(PKG_NAME)/inst/Docs $(CSS_FILES) $(AUX_DOCS) $(PKG_TAR_FILE) $(TARGET_DIR)
	scp -r ../../Docs/html $(TARGET_DIR)
	$(update-r-mirror)

foo: ../../Docs/html/struct__R__EventLoop.html
		scp -r ../../Docs/html $(TARGET_DIR)

include $(OMEGA_HOME)/Config/GNUmakefile.SWebPackageRules

