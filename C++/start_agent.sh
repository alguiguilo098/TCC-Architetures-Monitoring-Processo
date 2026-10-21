#!/bin/bash

cd /usr/local/bin || return

/usr/local/bin/monitor_process /usr/local/bin/confagent.conf >> /tmp/monitor_process.log 2>&1 &
/usr/local/bin/script_url >> /tmp/script_url.log 2>&1 &

