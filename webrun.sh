make -f Makefile.emscripten clean
make -f Makefile.emscripten
cp out/nibble8.html out/index.html
emrun --port 8080 --browser chrome out
