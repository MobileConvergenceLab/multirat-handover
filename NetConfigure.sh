#!/bin/bash

# Configure mobile node network
# Connect 2 WLAN interface(wlan0, wlan1) to virtual bridge(br0)
sudo ovs-vsctl del-br br0
sudo ovs-vsctl add-br br0
sudo ovs-vsctl add-port br0 wlan0
sudo ovs-vsctl add-port br0 wlan1

# Give IP on virtual bridge(br0)
sudo ifconfig br0 192.168.0.200

# MAC Address table
BR0=64:e5:99:f0:8d:7c
WLAN0=64:e5:99:f0:8d:7c
WLAN1=64:e5:99:f0:8d:db

# Setting flow table of virtual bridge
ovs-ofctl del-flows br0

# udp port setting
ovs-ofctl add-flow br0 priority=65534,udp,in_port=65534,tp_dst=10000,actions=output:1
ovs-ofctl add-flow br0 priority=65534,udp,in_port=1,tp_src=10000,actions=LOCAL

ovs-ofctl add-flow br0 priority=65534,udp,in_port=65534,tp_dst=10001,actions=mod_dl_src=$WLAN1,output:2
ovs-ofctl add-flow br0 priority=65534,udp,in_port=2,tp_src=10001,actions=mod_dl_dst=$BR0,LOCAL

## (default) use wlan0's own mac
ovs-ofctl add-flow br0 priority=35534,in_port=65534,actions=output:1
ovs-ofctl add-flow br0 priority=35534,in_port=1,actions=LOCAL

ovs-ofctl dump-flows br0
