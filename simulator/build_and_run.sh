cd code

rm *.o simulator

for i in *.c; do g++ -O3 -c $i; done
g++ -O3 -o simulator `ls *.o`

./simulator

