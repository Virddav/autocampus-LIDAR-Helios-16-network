################################################################################
# Build config
################################################################################

APP_DIR = /home/duser/mk6/stack/apps/exampleETSI
APP_BIN = $(APP_DIR)/exampleETSI

################################################################################
# Build Rules
################################################################################

all: $(APP_BIN)

$(APP_BIN):
	echo "duser" | sudo -S pwd; $(MAKE) -C $(APP_DIR) mk6-tarball

clean: 
	echo "duser" | sudo -S pwd; $(MAKE) -C $(APP_DIR) mk6-clean
