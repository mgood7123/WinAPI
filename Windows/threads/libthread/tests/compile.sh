set -v
g++ --shared -fPIC -I ../../libstack/include ../../libstack/src/*.cpp -o stack.so -g3 -O0 || exit
g++ --shared -fPIC ../../include/println.cpp -o println.so -g3 -O0 || exit
g++ --shared -fPIC -I ../../include -I ../../libstack/include -I ../../libthread/include ../../libthread/src/*.cpp stack.so println.so -o thread.so -Wl,-rpath . -g3 -O0 || exit
g++ -I ../../include -I ../../libstack/include -I ../../libthread/include -I ../../libthread/include/thread single_thread.cpp thread.so println.so -o single_thread -Wl,-rpath . -g3 -O0 || exit

./single_thread || gdb --quiet -ex "handle SIGCONT pass print nostop" -ex "handle SIGSTOP pass print nostop" -ex "set follow-fork-mode child" -ex r single_thread
