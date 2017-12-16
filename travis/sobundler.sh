#!/bin/bash

SKIP='
	libc.so*
	libdl.so*
	libdrm.so*
	libGL.so*
	libm.so*
	libpthread.so*
	libresolv.so*
	librt.so*
	libstdc++.so*
	libgcc_s.so*
'

exe=$1
dir=$2


#
# Returns 0 if the specified library should be skipped, 1 otherwise.
#
function skip_lib() {
	
	soname=$(basename "${1}")
	
	for skip in ${SKIP}; do
		if [[ ${soname} = ${skip} ]]; then
			return 0
		fi
	done

	return 1
}

libs=$(ldd $exe | cut -d ' ' -f 3)

skipped=""

for lib in ${libs}; do
	if skip_lib "${lib}"; then
		skipped="${skipped} ${lib}"
		continue
	fi
	
	echo "Installing ${lib} in ${dir}.."
	install "${lib}" -t "${dir}" -D
done

echo
echo "The following libraries were skipped:"

for skip in ${skipped}; do
	echo " ${skip}"
done

echo

#for exe in $@; do
#	echo "Rewriting RPATH for ${exe}.."
#	chrpath -r '$ORIGIN/lib' ${exe}
#done
