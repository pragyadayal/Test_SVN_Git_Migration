
#!/bin/sh
#
# Start application
#
# Include config file
. /etc/init.d/platform_debug

start() {
	platform_debug "Starting application : "


	if [ -x /matrix/scripts/nvr-x.sh ]
	then
	      chmod 755 /matrix/scripts/nvr-x.sh
	      /matrix/scripts/nvr-x.sh
	fi
	echo "OK"
}

stop() {
	:
}

case "$1" in
  start)
	start
	;;
  stop)
	stop
	;;
  restart|reload)
	stop
	start
	;;
  *)
	echo "Usage: $0 {start|stop|restart}"
	exit 1
esac

exit $?


