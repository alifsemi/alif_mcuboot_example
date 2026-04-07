# Script usage --> ./pack_setup.sh PACKS_PATH=directory_path_for_packs

# Number of Arguments passed
ARGUMENTS_PASSED=$#

#Total number of Arguments required
TOTAL_ARGUMENTS=2

curr_dir=$PWD

NC='\033[0m' # No Color
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
BROWN='\033[0;37m'
HIGH_CYAN='\033[38;5;45m'

# Checking for Number of Arguments passed
if [ "$TOTAL_ARGUMENTS" -gt "$ARGUMENTS_PASSED" ]
  then
    echo -e "\n\n${RED}<---      Arguments Missing       --->"
    echo -e "${CYAN}Script usage --> \n  ${GREEN} ./pack_setup.sh PACKS_PATH=directory_path_for_packs PACKS_INSTALL_DIR=output directory for packs PACK_NAME=packName ${NC}\n\n"
    exit 1
fi

# Assigning Arguments to their respective variables
for ARGUMENT in "$@"
do
    KEY=$(echo $ARGUMENT | cut -f1 -d=)

    KEY_LENGTH=${#KEY}
    VALUE="${ARGUMENT:$KEY_LENGTH+1}"

    export "$KEY"="$VALUE"
done

if [[ -z "$PACKS_PATH" ]]; then
    echo -e "\n\n${RED}Given input is incorrect instead of $1 , use PACKS_PATH=${NC}\n"
    exit 1
fi

if ! [ -d $PACKS_PATH ] ; then
    echo -e "\n\n${RED}Given Pack Path $PACKS_PATH is not a directory...please"
    echo -e "create a directory $PACKS_PATH and put all *.pack file inside that directory${NC}\n"
    exit 1
fi

if ! [ -d $PACKS_INSTALL_DIR ] ; then
    echo -e "\n\n${RED} Pack Installtion directory $PACKS_INSTALL_DIR is not available..."
    echo -e "please create a directory $PACKS_INSTALL_DIR${NC}\n"
    exit 1
fi

destDir="$PACKS_INSTALL_DIR"
currDir=$PACKS_PATH
custom_path=$currDir/.temp/.pack

if ! [ -f  $custom_path ] ; then
    rm -rf $custom_path
    mkdir -v $currDir/.temp
    touch $custom_path
    echo $custom_pat
else
    touch $custom_path
fi

# Creating a directory for Unzipped files
if [ -d $destDir ] ; then
    rm -rf $destDir
fi
mkdir -p $destDir
echo  -e "\n\n${CYAN}Pack Directory '$destDir' created ${NC}    "

cd $PACKS_PATH > /dev/null
PACKS_PATH=$PWD
echo $PWD
cd - > /dev/null

echo "PACK_NAME ${PACK_NAME}"

# Collecting all the Packs under one variable
if [[ $PACK_NAME = "" ]] ; then
    ALL_PACKS=$(find $PACKS_PATH  -maxdepth 1 -type f -name "*.pack")
    echo -e " Available Pack (To Extract):\n  $ALL_PACKS"
else
    ALL_PACKS=$(find $PACKS_PATH  -maxdepth 1 -type f -name "$PACK_NAME")
    echo -e " Pack to be exracted:\n  $ALL_PACKS"
fi

# Renaming the Packs to zip files
for file in $ALL_PACKS;
do
    FILENAME=$(basename "$file")
    DIRNAME=$(dirname   "$file")

    firstPart=${FILENAME#*.}
    firstArg=${FILENAME%%.*}
    secondPart=${firstPart#*.}
    secondArg=${firstPart%%.*}
    thirdArg=${secondPart%.pack}

    newDir="${destDir}/${firstArg}/${secondArg}/${thirdArg}"
    newDirName="${firstArg}_${secondArg}_${thirdArg}"

    echo "$newDir" >> $custom_path
    if [ $? -ne 0 ]; then
        echo -e "\n\n${RED} Info not able to generate, Run in root mode ${NC}\n"
        exit 1
    fi

    if [ -d $newDir ] ; then
        rm -rf $newDir
    fi

    mkdir -p $newDir
    echo -e "\n'${CYAN}$newDir' created${NC}"

    #cp -- "$file" "$PACKS_PATH/${newDirName}.zip"
    unzip "$file" -d $newDir

    echo -e "\n${GREEN} ************* ${newDirName} DONE *******************${NC}\n"
done
