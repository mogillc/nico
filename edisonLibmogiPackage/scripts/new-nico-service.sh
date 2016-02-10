#!/bin/bash

SERVICE_FILE="servicetemp.sh"



#echo "--- Download template ---"
#wget -q -O "$SERVICE_FILE" 'https://raw.github.com/gist/4275302/service.sh'
#chmod +x "$SERVICE_FILE"
#echo ""

echo "--- Customize ---"
echo "I'll now ask you some information to customize script"
echo "Press Ctrl+C anytime to abort."
echo "Empty values are not accepted."
echo ""

replace_in_file() {
	#VAL=$1
	echo "$1 : $2"
	sed -i "s/<$1>/$(printf "$2")/g" $SERVICE_FILE
}

prompt_token() {
	local VAL=""
	while [ "$VAL" = "" ]; do
		echo -n "${2:-$1} : "
		read VAL
		if [ "$VAL" = "" ]; then
			echo "Please provide a value"
		fi
	done
	VAL=$(printf '%q' "$VAL")
	eval $1=$VAL
	sed -i "s/<$1>/$(printf '%q' "$VAL")/g" $SERVICE_FILE
}

#prompt_token 'NAME'        'Service name'
NAME=nico
if [ -f "/etc/init.d/$NAME" ]; then
	echo "Error: service '$NAME' already exists"
	exit 1
fi

cp service-bunting.sh $SERVICE_FILE
replace_in_file 'NAME' "${NAME}"

#prompt_token 'DESCRIPTION' ' Description'
DESCRIPTION="Runs the nico dance program"
replace_in_file 'DESCRIPTION' ${DESCRIPTION}
#prompt_token 'COMMAND'     '     Command'
COMMAND="runNico"
replace_in_file 'COMMAND' "${COMMAND}"
#prompt_token 'USERNAME'    '        User'
USERNAME="root"
replace_in_file 'USERNAME' "${USERNAME}"
if ! id -u "$USERNAME" &> /dev/null; then
	echo "Error: user '$USERNAME' not found"
	exit 1
fi

echo ""

echo "--- Installation ---"
if [ ! -w /etc/init.d ]; then
	echo "You don't gave me enough permissions to install service myself."
	echo "That's smart, always be really cautious with third-party shell scripts!"
	echo "You should now type those commands as superuser to install and run your service:"
	echo ""
	echo "   mv \"$SERVICE_FILE\" \"/etc/init.d/$NAME\""
	echo "   touch \"/var/log/$NAME.log\" && chown \"$USERNAME\" \"/var/log/$NAME.log\""
	echo "   update-rc.d \"$NAME\" defaults"
	echo "   service \"$NAME\" start"
else
	echo "1. mv \"$SERVICE_FILE\" \"/etc/init.d/$NAME\""
	mv -v "$SERVICE_FILE" "/etc/init.d/$NAME"
	echo "2. touch \"/var/log/$NAME.log\" && chown \"$USERNAME\" \"/var/log/$NAME.log\""
	touch "/var/log/$NAME.log" && chown "$USERNAME" "/var/log/$NAME.log"
	echo "3. update-rc.d \"$NAME\" defaults"
	update-rc.d "$NAME" defaults
	echo "4. service \"$NAME\" start"
	/etc/init.d/${NAME} start
fi

echo ""
echo "---Uninstall instructions ---"
echo "The service can uninstall itself:"
echo "    service \"$NAME\" uninstall"
echo "It will simply run update-rc.d -f \"$NAME\" remove && rm -f \"/etc/init.d/$NAME\""
echo ""
echo "--- Terminated ---"