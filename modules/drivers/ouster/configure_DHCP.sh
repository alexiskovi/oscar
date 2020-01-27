read -p "Check that Ouster isn't plugged" -n1 -s
ip addr flush dev $1
echo $1
ip addr show dev $1
sudo ip addr add 10.5.5.1/24 dev $1
read -p "Plug in Ouster" -n1 -s
sudo ip link set $1 up
ip addr show dev $1
sudo dnsmasq -C /dev/null -kd -F 10.5.5.77,10.5.5.77  -i $1 --bind-dynamic
ping -c1 10.5.5.77