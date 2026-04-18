#!/bin/sh

LINE_SEPARATOR="***********************************************************"

# check there is no case-sensitive file
check_case_files() {
    casefiles=$(find ./ -name '*[A-Z]*' | grep -v '.svn' | grep -v 'README' | grep -v 'Makefile' | grep -v 'WarMUX-CommunityMaps')

    if [ -z "${casefiles}" ]; then
        return 0
    fi

    nbcasefiles=$(echo -e "$casefiles" | wc -l)

    if [ $nbcasefiles -ne 0 ]; then
        echo -e ${LINE_SEPARATOR} >&2
        echo -e "ERROR: there are $nbcasefiles case sensitive files. Please " >&2
        echo -e "       rename them and check config.xml files" >&2
        echo -e "$casefiles" >&2
        echo -e ${LINE_SEPARATOR} >&2
        return 1
    fi

    return 0
}

# check there is no space in filenames!
check_space_files() {
    spacefiles=$(find ./ -name '* *' | grep -v '.svn' | grep -v 'README' | grep -v 'Makefile' | grep -v 'WarMUX-CommunityMaps')

    if [ -z "${spacefiles}" ]; then
        return 0
    fi

    nbspacefiles=$(echo -e "${spacefiles}" | wc -l)

    if [ $nbspacefiles -ne 0 ]; then
        echo -e ${LINE_SEPARATOR} >&2
        echo -e "ERROR: there are $nbspacefiles files with space in their names." >&2
        echo -e "Please rename them and check config.xml files" >&2
        echo -e "$spacefiles" >&2
        echo -e ${LINE_SEPARATOR} >&2
        return 1
    fi

    return 0
}

check_files_exist() {
    map=$1
    filenames=$(grep ' file=' $map/config.xml | sed 's/.*file=\"\(.*\)".*/\1/')
    for f in $filenames; do
        if [ ! -f $map/$f ]; then
            echo -e "*** ERROR: file $map/$f does not exist" >&2
            return 1
        fi
    done
}

check_water_type() {
    map=$1
    VALID_WATER_TYPE="no chocolate dirtywater lava radioactive water wine"
    water=$(grep '<water>' $map/config.xml | sed 's%.*<water>\(.*\)</water>.*%\1%')
    if [[ ${VALID_WATER_TYPE} == *"$water"* ]]; then
        return 0
    fi

    echo "*** ERROR: Invalid water type $water for map $map" >&2
    return 1
}

check_one_map_config() {
    map=$1
    check_files_exist ${map}

    if [ $? -ne 0 ]; then
        return 1
    fi

    check_water_type ${map}
}

check_maps_config() {
    for d in *; do
        if [ -d "$d" ]; then
            check_one_map_config "$d"
            if [ $? -ne 0 ]; then
                echo -e "*** ERROR: invalid configuration file for map $d" >&2
                return 1
            fi
        fi
    done
}

check_space_files || exit 1

check_case_files || exit 1

check_maps_config || exit 1

DATE=$(date +%Y%m%d)
tar --exclude=.svn --exclude=src --exclude=compress.sh --exclude=*~ -czf WarMUX-CommunityMaps-${DATE}.tar.gz * 
