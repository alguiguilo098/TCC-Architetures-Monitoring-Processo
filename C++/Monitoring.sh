#!/bin/bash

PROCESS_NAME="./monitor_process"
LOG_FILE="agent_cost_log.csv"
INTERVAL=2  # segundos entre medições

# Cabeçalho do arquivo CSV
if [ ! -f "$LOG_FILE" ]; then
    echo "timestamp,pid,cpu_percent,mem_percent,mem_mb,uptime_sec" > "$LOG_FILE"
fi

echo "🔍 Monitorando processo: $PROCESS_NAME"
echo "Logs serão salvos em: $LOG_FILE"
echo "Pressione Ctrl+C para parar."

while true; do
    # Pega o PID do processo
    PID=$(pgrep -f "$PROCESS_NAME")

    TIMESTAMP=$(date '+%Y-%m-%d %H:%M:%S')

    if [ -z "$PID" ]; then
        echo "[$TIMESTAMP] ❌ Processo '$PROCESS_NAME' não encontrado."
    else
        for p in $PID; do
            # Coleta dados do processo
            CPU=$(ps -p "$p" -o %cpu= | awk '{print $1}')
            MEM_PERCENT=$(ps -p "$p" -o %mem= | awk '{print $1}')
            MEM_MB=$(ps -p "$p" -o rss= | awk '{printf "%.1f", $1/1024}')
            START_TIME=$(ps -p "$p" -o lstart=)
            UPTIME_SEC=$(ps -p "$p" -o etimes=)

            echo "[$TIMESTAMP] PID $p | CPU: ${CPU}% | MEM: ${MEM_MB}MB (${MEM_PERCENT}%) | Uptime: ${UPTIME_SEC}s"

            # Salva no CSV
            echo "$TIMESTAMP,$p,$CPU,$MEM_PERCENT,$MEM_MB,$UPTIME_SEC" >> "$LOG_FILE"
        done
    fi

    sleep "$INTERVAL"
done
