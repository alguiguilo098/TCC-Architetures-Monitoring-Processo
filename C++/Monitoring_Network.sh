#!/bin/bash

file="network_benchmark_$(date +%Y%m%d_%H%M%S).log"
INTERFACE=${1:-lo}
DURATION=${2:-60}

if ! ip link show $INTERFACE &>/dev/null; then
    echo "Interface $INTERFACE não encontrada"
    echo "Interfaces disponíveis:"
    ls /sys/class/net/
    exit 1
fi

echo "===============================" | tee -a $file
echo " BENCHMARK DE REDE" | tee -a $file
echo " Interface : $INTERFACE" | tee -a $file
echo " Duração   : ${DURATION}s" | tee -a $file
echo "===============================" | tee -a $file

MAX_RX=0
MAX_TX=0
TOTAL_RX=0
TOTAL_TX=0
SAMPLES=0
SUM_RX=0
SUM_TX=0

RX1=$(cat /sys/class/net/$INTERFACE/statistics/rx_bytes)
TX1=$(cat /sys/class/net/$INTERFACE/statistics/tx_bytes)
START_RX=$RX1
START_TX=$TX1

for i in $(seq 1 $DURATION); do
    sleep 1

    RX2=$(cat /sys/class/net/$INTERFACE/statistics/rx_bytes)
    TX2=$(cat /sys/class/net/$INTERFACE/statistics/tx_bytes)

    RX_RATE=$((RX2 - RX1))
    TX_RATE=$((TX2 - TX1))

    # máximo
    [ $RX_RATE -gt $MAX_RX ] && MAX_RX=$RX_RATE
    [ $TX_RATE -gt $MAX_TX ] && MAX_TX=$TX_RATE

    # acumulado para média
    SUM_RX=$((SUM_RX + RX_RATE))
    SUM_TX=$((SUM_TX + TX_RATE))
    SAMPLES=$((SAMPLES + 1))

    # converte para KB/s
    RX_KB=$(echo "scale=2; $RX_RATE/1024" | bc)
    TX_KB=$(echo "scale=2; $TX_RATE/1024" | bc)

    echo "[${i}s] RX: ${RX_KB} KB/s | TX: ${TX_KB} KB/s" | tee -a $file

    RX1=$RX2
    TX1=$TX2
done

# totais
TOTAL_RX=$((RX1 - START_RX))
TOTAL_TX=$((TX1 - START_TX))

AVG_RX=$((SUM_RX / SAMPLES))
AVG_TX=$((SUM_TX / SAMPLES))

echo "" | tee -a $file
echo "========= RESULTADO FINAL =========" | tee -a $file

echo "Total RX: $(echo "scale=2; $TOTAL_RX/1024/1024" | bc) MB" | tee -a $file
echo "Total TX: $(echo "scale=2; $TOTAL_TX/1024/1024" | bc) MB" | tee -a $file

echo "Média RX: $(echo "scale=2; $AVG_RX/1024" | bc) KB/s" | tee -a $file
echo "Média TX: $(echo "scale=2; $AVG_TX/1024" | bc) KB/s" | tee -a $file

echo "Pico RX : $(echo "scale=2; $MAX_RX/1024" | bc) KB/s" | tee -a $file
echo "Pico TX : $(echo "scale=2; $MAX_TX/1024" | bc) KB/s" | tee -a $file

echo "Arquivo salvo em: $file"