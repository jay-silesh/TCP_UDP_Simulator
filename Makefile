rm -rf cli1 ser cli2 cli3 cli4	
gcc -o warehouse warehouse.c -lsocket -lnsl -lresolv
gcc -o store1 store1.c -lsocket -lnsl -lresolv
gcc -o store2 store2.c -lsocket -lnsl -lresolv
gcc -o store3 store3.c -lsocket -lnsl -lresolv
gcc -o store4 store4.c -lsocket -lnsl -lresolv