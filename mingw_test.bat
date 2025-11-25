make clean
make -j8 config=debug
cd out
.\borealis-test.exe
cd ../