#!/bin/sh
GLOBAL_MACRO_FLAG+=" -D_GNU_SOURCE -DNDEBUG"
TARGET+=
pack_apdm()
{
	SYS_DEVICE_TYPE=100
    TARGET+=vr_apd
	SYS_DEV_NAME=apdm
	GLOBAL_MACRO_FLAG+=" -DSYS_DEVICE_TYPE=$SYS_DEVICE_TYPE"
}
pack_acsm()
{
	SYS_DEVICE_TYPE=120
    TARGET+=vr_acs
    SYS_DEV_NAME=acsm
	GLOBAL_MACRO_FLAG+=" -DSYS_DEVICE_TYPE=$SYS_DEVICE_TYPE"
}

#echo -e "1: apdm    2: acsm
echo -e "1: vr_apd   2:vr_acs"
echo -n "Please choose dev type:"
read DEV_TYPE
if [ "$DEV_TYPE" == "1" ]; then
pack_apdm
elif [ "$DEV_TYPE" == "2" ]; then
pack_acsm
else
echo "invalid mainboard type!"
exit
fi

#CFLAGS=" -O2 -Wall -funsigned-char"

CFLAGS=${GLOBAL_MACRO_FLAG}

export CFLAGS
export TARGET
make
make clean
