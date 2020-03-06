$CC -g -fPIC -c foo.c
$CC -shared foo.o -o libfoo.so
$CC -o loadso loadso.c -ldl
$CXX -o dhcpcsim dhcpcsim.cpp -ldl
