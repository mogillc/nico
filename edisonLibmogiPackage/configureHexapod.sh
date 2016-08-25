#!/bin/sh
usage() {
	echo ""
	echo "Usage:  $0 [-d|-n|-a] [-o]"
	echo "  -d:  Set the daemon to work with Dmitri"
	echo "  -n:  Set the daemon to work with Nico"
  echo "  -a:  Set the daemon to work with Anouk length legs"
  echo "  -o:  Set the daemon to work with Osc Dancing"

	echo ""
}

cmake_and_build() {
	BUILD_FOLDER=build-$1

	if [ ! -d $BUILD_FOLDER ]; then
		printf " - Creating: $BUILD_FOLDER ............ "
		mkdir -p $BUILD_FOLDER
		echo "Done."
	fi

	if [ ! -f $BUILD_FOLDER/Makefile ]; then
		printf " - Running cmake for $1 .......... "
		cmake -H$1 -B$BUILD_FOLDER >> $2 2>&1
		rc=$?
		if [ $rc != 0 ]; then
			echo "FAILED"
			return $rc
		else
			echo "Done."
		fi
	fi

	ORIGINAL_DIR=`pwd`
	cd $BUILD_FOLDER
	printf " - Running make for $1 ........... "
	make -j4 >> $2 2>&1
	rc=$?
	if [ $rc != 0 ]; then
		echo "FAILED"
		return $rc
	else
		echo "Done."
	fi

	printf " - Running make install for $1 ... "
	make install -j4 >> $2 2>&1
	rc=$?
	if [ $rc != 0 ]; then
		echo "FAILED"
		return $rc
	else
		echo "Done."
	fi
	cd ${ORIGINAL_DIR}
	return 0
}

LOGFILE="`pwd`/installation.log"
echo "Hexapod installation log:" > ${LOGFILE}

HEXAPOD_EXECUTABLE="danceNicoCurses"
HEXAPOD_EXECUTABLE_PARAMETERS=""
HEXAPOD_TYPE=" Nico "

if [[ "$#" -gt 2 ]]; then
	usage
	exit 1
fi

if [[ "$#" -lt 1 ]]; then
	usage
	exit 1
fi

HEXAPOD_EXECUTABLE="danceNicoCurses"
for var in "$@"
do
	if [[ "$var" == "-d" ]]; then
		HEXAPOD_EXECUTABLE="danceDmitriCurses"
	fi
done

for var in "$@"
do
	if [[ "$var" == "-o" ]]; then
		HEXAPOD_EXECUTABLE="Dancing"
		HEXAPOD_EXECUTABLE_PARAMETERS="$HEXAPOD_EXECUTABLE_PARAMETERS -o -t `cat /home/root/type.txt`"
	elif [[ "$var" == "-d" ]]; then
		HEXAPOD_EXECUTABLE_PARAMETERS="$HEXAPOD_EXECUTABLE_PARAMETERS -j dmitri.json"
		HEXAPOD_TYPE="Dmitri"
	elif [[ "$var" == "-n" ]]; then
		HEXAPOD_EXECUTABLE_PARAMETERS="$HEXAPOD_EXECUTABLE_PARAMETERS -j nico.json"
		HEXAPOD_TYPE=" Nico "
	elif [[ "$var" == "-a" ]]; then
		HEXAPOD_EXECUTABLE_PARAMETERS="$HEXAPOD_EXECUTABLE_PARAMETERS -j anouk.json"
		HEXAPOD_TYPE="Anouk "
	else
		usage
		exit 1
	fi
done

# cmake:
printf "Installing cmake ............... "
if hash cmake 2>/dev/null; then
	echo "already installed."
else
	echo "############### CMAKE ###############" >> ${LOGFILE}
	sh dependencies/cmake-3.0.1-Linux-i386.sh --prefix=/usr/ --exclude-subdir >> ${LOGFILE}
	echo "Done."
fi

# libusb:
printf "Installing libusb .............. "
if [ -f /usr/include/libusb-1.0/libusb.h ]; then
	echo "already installed."
else
	echo "############### LIBUSB ###############" >> ${LOGFILE}
#cd libusb
	echo ""
	printf " - Running opkg install ........ "
	opkg install dependencies/libusb/* >> ${LOGFILE} 2>&1
	rc=$?
	if [ $rc != 0 ]; then
		echo "FAILED"
		exit $rc
	else
		echo "Done."
	fi
#	cd ..
#	echo "Done."
fi

# libftdi:
printf "Installing libftdi ............. "
if [ -f /usr/local/include/libftdi1/ftdi.h ]; then
	echo "already installed."
else
	echo "############### LIBFTDI ###############" >> ${LOGFILE}
	echo ""
	cmake_and_build dependencies/libftdi1-1.2 ${LOGFILE}
	if [ $? != 0 ]; then
		echo "Failed to compile and install, see log file: ${LOGFILE}"
		exit 2
	fi
#echo "Done."
fi

# libftdi:
printf "Installing libjsoncpp .......... "
if [ -f /usr/local/include/json/json.h ]; then
	echo "already installed."
else
	echo "############### LIBJSONCPP ###############" >> ${LOGFILE}
	echo ""
	cmake_and_build dependencies/jsoncpp ${LOGFILE}
	if [ $? != 0 ]; then
		echo "Failed to compile and install, see log file: ${LOGFILE}"
		exit 2
	fi
#echo "Done."
fi

# libmogi:
echo "Installing libmogi .......... "
echo "############### LIBMOGI ###############" >> ${LOGFILE}

cmake_and_build libmogi ${LOGFILE}
if [ $? != 0 ]; then
	echo "Failed to compile and install, see log file: ${LOGFILE}"
	exit 2
fi

# mogid:
#echo "Installing mogid .......... "
#echo "############### MOGID ###############" >> ${LOGFILE}
#
#cmake_and_build mogid ${LOGFILE}
#if [ $? != 0 ]; then
#echo "Failed to compile and install, see log file: ${LOGFILE}"
#exit 2
#fi


# the final example programs
echo "Installing the dance program ... "
echo "############### EXAMPLES ###############" >> ${LOGFILE}

cmake_and_build examples/dance ${LOGFILE}
if [ $? != 0 ]; then
	echo "Failed to compile and install, see log file: ${LOGFILE}"
	exit 2
fi

# HARD CODING HOLY CRAP!
if [ -f /bin/danceNicoCurses ]; then
	rm /bin/danceNicoCurses
fi
ln -s /usr/local/bin/danceNicoCurses /bin/danceNicoCurses

if [ -f /bin/danceDmitriCurses ]; then
rm /bin/danceDmitriCurses
fi
ln -s /usr/local/bin/danceDmitriCurses /bin/danceDmitriCurses
#cd ..
#echo "Done."




#printf " - creating ~/run.sh ..."
##cd ~
#echo "#!/bin/sh" > run.sh
#echo "# This script runs the danceNicoCurses program in the daemon mode so that" >> run.sh
#echo "#   outputs from another program may be piped for keyboard control" >> run.sh
#echo "" >> run.sh
#echo "python `pwd`/serial-filter.py /dev/ttyMFD1 | danceNicoCurses -d" >> run.sh
#echo "#danceNicoCurses" >> run.sh
#echo "Done."
##cp run.sh /sbin/danceNicosh

#chmod +x run.sh
#cp run.sh ~/run.sh
#cp run.sh /sbin/runNico

# make a run script:
echo "Generating scripts:"
echo "############### SCRIPTS ###############" >> ${LOGFILE}
if hash systemctl 2>/dev/null; then
	# Due to multicasting, we need to modify the start up script for udhcpd to set routing information so multicast packets may be sent:
	#
	echo "Checking hostapd:"
	echo "############### HOSTAPD ###############" >> ${LOGFILE}
	printf " - Checking udhcpd configuration ........ "
	UDHCPD_SERVICE=/lib/systemd/system/udhcpd-for-hostapd.service
	ROUTE_COMMAND="ExecStartPost=`which route` add default gw 192.168.42.1 wlan0"
	ROUTE_COMMAND_ESCAPED="$(echo $ROUTE_COMMAND | sed 's/\//\\\//g')"
	grep -Fxq "$ROUTE_COMMAND" $UDHCPD_SERVICE
	if [ $? -eq 0 ]; then
		echo "already done."
	else
		sed -i "s/.*ExecStart=.*/&\n$ROUTE_COMMAND_ESCAPED/" $UDHCPD_SERVICE
		systemctl daemon-reload
		echo "modification complete."
		if [ `systemctl status hostapd | grep 'active (running)' | wc -l` -eq 1 ]; then
			printf " - hostapd running, restarting udhcpd ... "
			systemctl restart udhcpd-for-hostapd.service
			echo "Done."
		fi
	fi

	printf " - Creating systemd service ........ "

	SYSTEMD_SERVICE=/etc/systemd/system/nico.service
	echo "[Unit]" > $SYSTEMD_SERVICE
	echo "Description=nico" >> $SYSTEMD_SERVICE
	echo "After=network.target" >> $SYSTEMD_SERVICE
	echo "Requires=network.target" >> $SYSTEMD_SERVICE
	echo "" >> $SYSTEMD_SERVICE
	echo "[Service]" >> $SYSTEMD_SERVICE
	echo "TimeoutStartSec=0" >> $SYSTEMD_SERVICE
	echo "ExecStart=`which $HEXAPOD_EXECUTABLE` $HEXAPOD_EXECUTABLE_PARAMETERS -d" >> $SYSTEMD_SERVICE
	echo "" >> $SYSTEMD_SERVICE
	echo "[Install]" >> $SYSTEMD_SERVICE
	echo "WantedBy=multi-user.target" >> $SYSTEMD_SERVICE

	echo "Done."

	printf " - Reloading systemd services ...... "
	systemctl daemon-reload >> ${LOGFILE} 2>&1
	if [ $? != 0 ]; then
		echo "FAILED!  See log file: ${LOGFILE}"
		exit 2
	fi
	echo "Done."

	printf " - Enabling nico service on boot ... "
	systemctl enable nico >> ${LOGFILE} 2>&1
	if [ $? != 0 ]; then
		echo "FAILED!  See log file: ${LOGFILE}"
		exit 2
	fi
	echo "Done."

#printf " - Starting the nico service ....... "
#	systemctl start nico >> ${LOGFILE} 2>&1
#	if [ $? != 0 ]; then
#		echo "FAILED!  See log file: ${LOGFILE}"
#		exit 2
#	fi
#	echo "Done."

else
	printf " - Checking for /etc/init.d/ ....... "
	if [ ! -d /etc/init.d ]; then
		echo "doesn't exist, creating."
		mkdir /etc/init.d
	else
		echo "already exists."
	fi

	printf " - Checking for /etc/init.d/nico ... "
	if [ -f /etc/init.d/nico ]; then
		echo "already installed."
	else
		echo "doesn't exist, creating."
		cd scripts
		./new-nico-service.sh >> ${LOGFILE} 2>&1
		cd ..
	#echo "Done."
	fi
fi


./scripts/splash.sh /etc/motd
