#!/bin/bash

file="disk_benchmark_$(date +%Y%m%d_%H%M%S).log"
DURATION=${1:-60}
DISK=${2:-nvme0n1}

if [ ! -f /sys/block/$DISK/stat ]; then
    echo "Disco $DISK não encontrado"
    echo "Discos disponíveis:"
    ls /sys/block/
    exit 1
fi

echo "===============================" | tee -a $file
echo " BENCHMARK DE DISCO (BYTES)" | tee -a $file
echo " Disco     : $DISK" | tee -a $file
echo " Duração   : ${DURATION}s" | tee -a $file
echo "===============================" | tee -a $file

MAX_READ=0
MAX_WRITE=0
SUM_READ=0
SUM_WRITE=0
SAMPLES=0

# setores lidos/escritos no início
READ1=$(cat /sys/block/$DISK/stat | awk '{print $3}')
WRITE1=$(cat /sys/block/$DISK/stat | awk '{print $7}')

for i in $(seq 1 $DURATION); do
    sleep 1

    READ2=$(cat /sys/block/$DISK/stat | awk '{print $3}')
    WRITE2=$(cat /sys/block/$DISK/stat | awk '{print $7}')

    # diferença em setores
    READ_SECT=$((READ2 - READ1))
    WRITE_SECT=$((WRITE2 - WRITE1))

    # converter para bytes (1 setor = 512 bytes)
    READ_BYTES=$((READ_SECT * 512))
    WRITE_BYTES=$((WRITE_SECT * 512))

    # máximo
    [ $READ_BYTES -gt $MAX_READ ] && MAX_READ=$READ_BYTES
    [ $WRITE_BYTES -gt $MAX_WRITE ] && MAX_WRITE=$WRITE_BYTES

    # soma para média
    SUM_READ=$((SUM_READ + READ_BYTES))
    SUM_WRITE=$((SUM_WRITE + WRITE_BYTES))
    SAMPLES=$((SAMPLES + 1))

    # conversões
    READ_KB=$(echo "scale=2; $READ_BYTES/1024" | bc)
    WRITE_KB=$(echo "scale=2; $WRITE_BYTES/1024" | bc)

    READ_MB=$(echo "scale=2; $READ_BYTES/1024/1024" | bc)
    WRITE_MB=$(echo "scale=2; $WRITE_BYTES/1024/1024" | bc)

    echo "[${i}s] READ: ${READ_BYTES} B | ${READ_KB} KB/s | ${READ_MB} MB/s  |  WRITE: ${WRITE_BYTES} B | ${WRITE_KB} KB/s | ${WRITE_MB} MB/s" | tee -a $file

    READ1=$READ2
    WRITE1=$WRITE2
done

AVG_READ=$((SUM_READ / SAMPLES))
AVG_WRITE=$((SUM_WRITE / SAMPLES))

echo "" | tee -a $file
echo "========= RESULTADO FINAL =========" | tee -a $file

echo "Média READ : ${AVG_READ} B/s | $(echo "scale=2; $AVG_READ/1024" | bc) KB/s" | tee -a $file
echo "Média WRITE: ${AVG_WRITE} B/s | $(echo "scale=2; $AVG_WRITE/1024" | bc) KB/s" | tee -a $file

echo "Pico READ  : ${MAX_READ} B/s | $(echo "scale=2; $MAX_READ/1024" | bc) KB/s" | tee -a $file
echo "Pico WRITE : ${MAX_WRITE} B/s | $(echo "scale=2; $MAX_WRITE/1024" | bc) KB/s" | tee -a $file

echo "Arquivo salvo em: $file"