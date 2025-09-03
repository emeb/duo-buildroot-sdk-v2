#!/bin/sh

#LED_PIN=440
# changed to XGPIOC[3] - pkg pin 57 for dspod_cv1800b
LED_PIN=419

LED_GPIO=/sys/class/gpio/gpio${LED_PIN}

if test -d ${LED_GPIO}; then
    echo "PIN ${LED_PIN} already exported"
else
    echo ${LED_PIN} > /sys/class/gpio/export
fi

echo out > ${LED_GPIO}/direction

while true; do
    echo 0 > ${LED_GPIO}/value
    sleep 0.5
    echo 1 > ${LED_GPIO}/value
    sleep 0.5
done

