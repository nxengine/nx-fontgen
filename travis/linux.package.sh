mkdir deploy
cp bin/fontbm deploy/fontbm.bin
cp travis/fontbm.sh deploy/fontbm
cd deploy
mkdir lib
../travis/sobundler.sh ./fontbm.bin ./lib/
tar -czf fontbm.tar.gz ./*
mv fontbm.tar.gz ..
cd ..