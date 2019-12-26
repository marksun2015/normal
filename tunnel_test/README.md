$ gcc -o tuntest tuntest.c  

$ ./tuntest  
TUN name is tun0  

切換到令一個terminal 
$ ifconfig tun0 0.0.0.0 up
$ route add 10.10.10.1 dev tun0
$ ping 10.10.10.1 

切換回原來的terminal，可以看到read tun0的數據   
root@cMT-9A0F:/home# ./tuntest 
TUN name is tun0
read 84 bytes
write 84 bytes
read 84 bytes
write 84 bytes
read 84 bytes
write 84 bytes
read 84 bytes

