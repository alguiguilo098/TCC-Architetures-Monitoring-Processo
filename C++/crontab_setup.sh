#!/bin/bash

CRON_FILE=$(mktemp)

# Mantém o crontab existente
crontab -l 2>/dev/null > "$CRON_FILE"

sudo usermod -aG wireshark $USER

mkdir ${HOME}/agente
# Adiciona o monitor_process
echo '@reboot /bin/bash -c '\''cd ${HOME}/TCC-Architetures-Monitoring-Processo/C++ && ./monitor_process confagent.conf >> ${HOME}/agente/agent.log 2>&1'\''' >> "$CRON_FILE"

# Adiciona o script_url
echo '@reboot /bin/bash -c '\''cd ${HOME}/TCC-Architetures-Monitoring-Processo/C++ && ./script_url >> ${HOME}/agente/agent_url.log 2>&1'\''' >> "$CRON_FILE"

 
# Instala o novo crontab
crontab "$CRON_FILE"

rm "$CRON_FILE"

echo "Crontab configurado:"
crontab -l

reboot
