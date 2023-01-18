#!/bin/sh
# 
# Changelog:
# - 2021-12-22 use two block with block size 131072 each (by Mark, Mao)
# - 2021-12-21 first draft (Mao)

# Declare inputs
# Path for original block
ORIG_BLOCK=/tmp/mtdblock7_orig.img
# Path for preparation block
PREP_BLOCK=/tmp/mtdblock7_prep.img
# Path for double check block
CHK_BLOCK=/tmp/mtdblock7_double_confirm.img
# Path for a flag of successful run to avoid repeated run
SUCCESS_FLAG=/tmp/mtdblock7_updated

MAC=$(echo -n $1 | sed -e 'y/ABCDEF/abcdef/')
MAC_FOR_PRINTF=$(echo -n "\\x"${MAC:0:2}"\\x"${MAC:3:2}"\\x"${MAC:6:2}"\\x"${MAC:9:2}"\\x"${MAC:12:2}"\\x"${MAC:15:2})
MAC_REVERSED_FOR_PRINTF=$(echo -n "\\x"${MAC:15:2}"\\x"${MAC:12:2}"\\x"${MAC:9:2}"\\x"${MAC:6:2}"\\x"${MAC:3:2}"\\x"${MAC:0:2})

# step 1. Checking input
echo -n $MAC | grep -E '^00:0c:26:[0-9a-f]{2}:[0-9a-f]{2}:[0-9a-f]{2}$' > /dev/null
if [ $? -ne 0 ]; then
    echo "ERROR: $MAC is not in correct format: 00:0c:26:xx:yy:zz"
    exit 1
fi
echo "Input: $MAC"

if [ -e $SUCCESS_FLAG ]; then
    printf "SUCCESS_FLAG ($SUCCESS_FLAG) means a successful update was run. \nTo continue anyway, please remove the file.\n"
    exit 1
fi

# step 2. Reading original block
echo "Reading target block... "
mtd_debug read /dev/mtdblock7 0 262144 $ORIG_BLOCK
if [ $? -ne 0 ]; then
    echo "ERROR: failed to read target block"
    exit 1
fi

# Checking block file size
size=$(wc -c $ORIG_BLOCK | awk '{print $1}')
if [ $size != "262144" ]; then
    echo "ERROR: block file size should be 262144"
    exit 1
fi

echo "Dumping original content... "
hexdump -C $ORIG_BLOCK

# step 3. prepare the modified block file
echo "Prepare the modified block..."
cp $ORIG_BLOCK $PREP_BLOCK
printf $MAC_FOR_PRINTF | dd of=$PREP_BLOCK bs=1 count=6 conv=notrunc
printf $MAC_REVERSED_FOR_PRINTF | dd of=$PREP_BLOCK seek=20 bs=1 count=6 conv=notrunc
printf $MAC_FOR_PRINTF | dd of=$PREP_BLOCK seek=131072 bs=1 count=6 conv=notrunc
printf $MAC_REVERSED_FOR_PRINTF | dd of=$PREP_BLOCK seek=131092 bs=1 count=6 conv=notrunc

echo "Erasing target block... "
flash_eraseall /dev/mtd7
if [ $? -ne 0 ]; then
    echo "ERROR: unable to erase target block"
    exit 1
fi

# step 4. write the block file
echo "Overwriting target block... "
nandwrite -p -s 0 /dev/mtd7 $PREP_BLOCK
if [ $? -ne 0 ]; then
    echo "ERROR: unable to overwrite target block"
    exit 1
fi

echo "Dumping current modified content... "
mtd_debug read /dev/mtdblock7 0 262144 $CHK_BLOCK
hexdump -C $CHK_BLOCK

# step 5. double check
cmp -s $PREP_BLOCK $CHK_BLOCK
if [ $? -ne 0 ]; then
    echo "ERROR: $PREP_BLOCK is different from $CHK_BLOCK. Unsuccessful write operation!" 
    exit 1
fi 

touch $SUCCESS_FLAG

echo "*********************************************"
echo "*                                           *"
echo "*  Please reboot to apply the modification  *"
echo "*                                           *"
echo "*********************************************"
