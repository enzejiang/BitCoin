rm *.dat

valgrind --tool=memcheck --leak-check=full --read-var-info=yes --show-leak-kinds=definite --track-origins=yes --log-file=./valgrind.log ./BitCoin
