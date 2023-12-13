#
# Makefile for the input misc drivers.
#

# Each configuration option enables a list of files.
include $(srctree)/drivers/misc/mediatek/Makefile.custom
ccflags-y += -I$(MTK_PROJECT_PATH_ROOT)/jksensorhub/apollo_mcu

obj-y += apollo_sensorhub.o apollo_sensorhub_spi.o apollo_sensorhub_sync.o apollo_sensorhub_test.o apollo_sensorhub_version.o crc.o
