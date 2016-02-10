#!/bin/sh

usage() {
	echo ""
	echo "Usage:  $0 <outputFilePath>"
	echo ""
}

center() {
	STRING=$1
	STRING_LENGTH=${#STRING}
	STRING_SPACE=$2

	let STRING_DIFF=$STRING_SPACE-$STRING_LENGTH
	let STRING_START=$STRING_DIFF/2

	STRING_CENTERED="$STRING"

	for var in $(seq 1 "$STRING_START")
	do
		STRING_CENTERED=" $STRING_CENTERED"
	done
	let STRING_START=${#STRING_CENTERED}+1

	for var in $(seq $STRING_START $STRING_SPACE)
	do
		STRING_CENTERED="$STRING_CENTERED "
	done

	echo "$STRING_CENTERED"
}

if [[ "$#" -gt 1 ]]; then
	usage
	exit 1
fi
if [[ "$#" -lt 1 ]]; then
	usage
	exit 1
fi

#echo "`uname -s` `uname -n` `uname -r` `uname -m` `uname -o`
#╔══════════════════════════════════════════════╗
#║                     $HEXAPOD_TYPE                   ║
#║  ███╗   ██╗██╗ ██████╗ ██████╗    /\\ __ /\\   ║
#║  ████╗  ██║╚═╝██╔════╝██╔═══██╗     /  \\     ║
#║  ██╔██╗ ██║██╗██║     ██║   ██║ /\\_/    \\_/\\ ║
#║  ██║╚██╗██║██║██║     ██║   ██║    \\    /    ║
#║  ██║ ╚████║██║╚██████╗╚██████╔╝  /\\_\\__/_/\\  ║
#║  ╚═╝  ╚═══╝╚═╝ ╚═════╝ ╚═════╝               ║
#║  github.com/blegas78/MBrobotics    v`mogi-version`    ║
#╚══════════════════════════════════════════════╝
#" > /etc/motd
#echo "`uname -s` `uname -n` `uname -r` `uname -m` `uname -o`
#╔════════════════════════════════════════════╗
#║                           ██╗              ║
#║   ███╗ ███╗  ████╗  ████╗ ╚═╝   /\\ __ /\\   ║
#║  ██╔████╔██╗██╔═██╗██╔═██╗██╗     /  \\     ║
#║  ██║╚██╔╝██║██║ ██║██║ ██║██║ /\\_/    \\_/\\ ║
#║  ██║ ╚═╝ ██║╚████╔╝╚████╔╝██║    \\    /    ║
#║  ╚═╝     ╚═╝ ╚═══╝  ╚══██╗╚═╝  /\\_\\__/_/\\  ║
#║                     ████╔╝                 ║
#║   http://mogi.io    ╚═══╝        v`mogi-version`    ║
#╚════════════════════════════════════════════╝
#" > /etc/motd

#LINUX_VERSION=$(center "`uname -s` `uname -r` `uname -m` `uname -o`" 49)
#HEXAPOD_VRSTR=$(center "v`mogi-version`" 14)
#WEBSITE__________STRING=$(center "http://mogi.io" 24)
#
#echo "$LINUX_VERSION
#╔═══════════════════════════════════════════════╗
#║  /\\        /\\                ██╗              ║
#║ //\\\\__  __//\\\\  ████╗  ████╗ ╚═╝   /\\ __ /\\   ║
#║//  \`¯\\\\//¯´  \\\\██╔═██╗██╔═██╗██╗     /  \\     ║
#║||     \\/     ||██║ ██║██║ ██║██║ /\\_/    \\_/\\ ║
#║\\\\            //╚████╔╝╚████╔╝██║    \\    /    ║
#║ \\\\          //  ╚═══╝  ╚══██╗╚═╝  /\\_\\__/_/\\  ║
#║  \\|        |/          ████╔╝                 ║
#║$WEBSITE__________STRING╚═══╝    $HEXAPOD_VRSTR║
#╚═══════════════════════════════════════════════╝
#" > $1

LINUX_VERSION=$(center "`uname -s` `uname -r` `uname -m` `uname -o`" 58)
HEXAPOD_VRS=$(center "v`mogi-version`" 11)
WEBSITE______________STRING=$(center "http://mogi.io" 29)
echo "$LINUX_VERSION
╔════════════════════════════════════════════════════════╗
║    ,        ,                                          ║
║  ,AM\\_,  ,_/MA,                             /\\ __ /\\   ║
║ ,M´ \`¯V\\/V¯´ \`M,    ,=O=,     ,=G=,    I      /  \\     ║
║ M´     \\/     \`M   O     O   G   ___   I  /\\_/    \\_/\\ ║
║ M              M   O.   .O   G.  ¯¯G   I     \\    /    ║
║ V              V    \`\"=\"´     \`\"=\"´    I   /\\_\\__/_/\\  ║
║  \            /                                        ║
║              $WEBSITE______________STRING $HEXAPOD_VRS ║
╚════════════════════════════════════════════════════════╝
" > $1

#echo "`uname -s` `uname -n` `uname -r` `uname -m` `uname -o`
#╔═════════════════════════════════════════════════════╗
#║    ___       ___       ___       ___                ║
#║   /\__\     /\  \     /\  \     /\  \     /\ __ /\  ║
#║  //| _|_   _\\\  \   //\\  \   //\\  \      /__\    ║
#║ //||/\__\ /\//\\__\ ///\\\__\ ///\\\__\ /\_//  \\_/\║
#║ \/||//  / \\//\/__/ \\\ \/__/ \\\///  /    \\__//   ║
#║   |//  /   \\\__\    \\\__\    \\//  /   /\_\__/_/\ ║
#║   \/__/     \/__/     \/__/     \/__/               ║
#║ github.com/blegas78/MBrobotics            v`mogi-version`    ║
#╚═════════════════════════════════════════════════════╝
#" > /etc/motd
