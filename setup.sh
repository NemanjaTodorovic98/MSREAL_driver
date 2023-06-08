#!/bin/sh

gcc -o calculate calculate.c 
sudo journalctl --vacuum-size=2M
sudo make
sudo rmmod driver.ko
sudo insmod driver.ko
sudo chmod 777 /dev/GetBlockIP
sudo chmod 777 /dev/bramA
sudo chmod 777 /dev/bramB
sudo chmod 777 /dev/bramC
sudo chmod 777 /dev/bramD
sudo chmod 777 /dev/bramF
sudo chmod 777 /dev/bramG
sudo chmod 777 /dev/bramH


