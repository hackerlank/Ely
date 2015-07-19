#!/bin/bash

CMD=$1
BUILDMODE=$2
THREADS=$3
WORKDIR=$4
MYWORKSPACE=$5
#
CMDS="install update uninstall clean distclean"
BUILDMODES="release debug"
MYPKGS="my_bullet3 my_librocket my_opensteer my_recastnavigation"
DISTPKGS="autoconf automake build-essential checkinstall freeglut3-dev gdebi git gnulib libboost-python-dev libeigen3-dev libfreetype6-dev libgl1-mesa-dev libglu1-mesa-dev libode-dev libopenal-dev libopencv-dev libsdl1.2-dev libssl-dev libswscale-dev libtool libvorbis-dev libx11-dev libxxf86dga-dev libxxf86vm-dev nvidia-cg-dev pkg-config python-dev"
BUILDDIR="build-elydeps"
CONFARGS=
PANDA3DALLEXT="-all"
PANDA3DINSTCMD="sudo gdebi --n "

#clean
cleanElyDeps () {
	echo "clean autoconf archive"
	WD=${WORKDIR}/autoconf-archive
	[ -d ${WD} ] && cd ${WD} && \
		sudo make uninstall && \
		make clean && \
		make distclean
	for PKG in ${MYPKGS}
	do
		echo "clean ${PKG}"
		WD=${WORKDIR}/${MYWORKSPACE}/${PKG}/${BUILDDIR}
		[ -d ${WD} ] && cd ${WD} && \
			sudo make uninstall && \
			make clean && \
			make distclean
	done
	echo "clean Panda3d"
	sudo apt-get --purge --yes remove panda3d1.9 
	rm -rfv ${WORKDIR}/panda3d/built
	#
	cd ${WORKDIR}
}

#install sources
installElyDeps () {
	#install distribution packages
	sudo apt-get install --yes ${DISTPKGS}
	#install sources
	mkdir -p ${WORKDIR}/${MYWORKSPACE}
	cd ${WORKDIR}
	echo "install autoconf archive source"
	WD=${WORKDIR}/autoconf-archive
	! ( [ -d ${WD} ] || [ -h ${WD} ] ) && 
		git clone git://git.sv.gnu.org/autoconf-archive.git
	for PKG in ${MYPKGS}
	do
		cd ${WORKDIR}/${MYWORKSPACE}
		echo "install ${PKG} source"
		WD=${WORKDIR}/${MYWORKSPACE}/${PKG}
		! ( [ -d ${WD} ] || [ -h ${WD} ] ) && git clone https://github.com/consultit/${PKG}.git
	done
	#
	cd ${WORKDIR}
	echo "install Panda3d source"
	WD=${WORKDIR}/panda3d
	! ( [ -d ${WD} ] || [ -h ${WD} ] ) && git clone https://github.com/panda3d/panda3d.git
}

#uninstall
uninstallElyDeps () {
	cleanElyDeps
	#remove all working directories
	echo "distclean autoconf archive"
	rm -rf ${WORKDIR}/autoconf-archive 
	for PKG in ${MYPKGS}
	do
		echo "distclean ${PKG}"
		rm -rf ${WORKDIR}/${MYWORKSPACE}/${PKG}
	done
	echo "distclean Panda3d"
	rm -rfv ${WORKDIR}/panda3d
	#uninstall distribution packages
	sudo apt-get --purge --yes remove ${DISTPKGS}
	sudo apt-get --purge --yes autoremove
	#
	cd ${WORKDIR}
}

#update
updateElyDeps () {	
	echo "update autoconf archive"
	WD=${WORKDIR}/autoconf-archive
	[ -d "${WD}" ] && cd ${WD} && \
		git pull && \
		./bootstrap.sh && \
		./configure --prefix=/usr && \
		make maintainer-all && \
		sudo make install
	#
	for PKG in ${MYPKGS}
	do
		echo "update ${PKG}"
		WD=${WORKDIR}/${MYWORKSPACE}/${PKG}
		[ -d "${WD}" ] && cd ${WD} && \
			git pull && \
			autoreconf -i -v && \
			mkdir -p ${BUILDDIR} && \
			cd ${BUILDDIR} && \
			if [ "${PKG}" = "my_librocket" ] ; then \
				CONFARGS=${CONFARGS}" --enable-python" ; \
			fi && \
			../configure ${CONFARGS} && \
			make -j${THREADS} && \
			sudo make install
	done
	echo "update Panda3d"
	OPTIMIZE=3
	[ "${BUILDMODE}" = "debug" ] && OPTIMIZE=1
	WD=${WORKDIR}/panda3d
	[ -d "${WD}" ] && cd ${WD} && \
		git pull && \
		makepanda/makepanda.py --verbose --everything --no-fftw --no-gles --no-gles2 \
	 		--threads ${THREADS} --optimize ${OPTIMIZE} --installer && \
	 	PKG=panda3d1.10_1.10.0_amd64-$(date +'%Y%m%d')-${BUILDMODE}${PANDA3DALLEXT}.deb && \
	 	mv panda3d1.10_1.10.0_amd64.deb ${PKG} && \
	 	${PANDA3DINSTCMD} ${PKG}
	#
	cd ${WORKDIR}
}

#print usage
printUsage () {
	echo "Usage: $0 install|uninstall|update|clean [build-mode=release|debug] [threads=half cores] [work-dir=$(pwd)] [my-workspace=WORKSPACE]"
}

#MAIN
#checks
#trim spaces from CMD and checks if it's in CMDS else exit
CMD="$(echo -e "${CMD}" | tr -d '[[:space:]]')"
[ "${CMDS/$CMD}" = "${CMDS}" ] && printUsage && exit 1
#trim spaces from BUILDMODE and checks if it's in BUILDMODES else =release
BUILDMODE="$(echo -e "${BUILDMODE}" | tr -d '[[:space:]]')"
[ "${BUILDMODES/$BUILDMODE}" = "${BUILDMODES}" ] && BUILDMODE=release
#checks if THREADS is integer else =half than num of cores (>=1)
integer='^[0-9]+$'
T=$(($(cat /proc/cpuinfo | grep -i processor | wc -l) / 2))
if [ -z "$THREADS" ] 
then
	THREADS=${T}
else
	! [[ ${THREADS} =~ ${integer} ]] && THREADS=${T}
fi
[ "$((${THREADS} >= 1))" = "0" ] && THREADS=1
#checks if WORKDIR exists else =PWD
! [ -d "${WORKDIR}" ] && WORKDIR=$(pwd)
#checks if MYWORKSPACE specified and WORKDIR}/MYWORKSPACE exists else =WORKSPACE
if [ -z ${MYWORKSPACE} ] 
then
	MYWORKSPACE=WORKSPACE 
else
	! [ -d "${WORKDIR}/${MYWORKSPACE}" ] && MYWORKSPACE=WORKSPACE
fi

#check if debug build mode
[ "${BUILDMODE}" == "debug" ] && CONFARGS="--enable-debug"

#start
case ${CMD} in
	install)
		installElyDeps
		updateElyDeps
	;;
	uninstall)
		uninstallElyDeps
	;;
	update)
		updateElyDeps
	;;
	clean)
		cleanElyDeps
	;;
	*)
		printUsage
	;;
esac

exit 0

