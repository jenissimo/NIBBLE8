# NIBBLE8

## About

Welcome to NIBBLE8, the heart and soul of retro-style gaming encapsulated in a modern, accessible platform. Inspired by the monochrome beauty of amber CRT screens and the classic 4-color vibe of the original Game Boy, NIBBLE8 harks back to the golden age of 8-bit gaming while providing a comprehensive platform for creators and players alike.

Boasting a potent 2-bit graphics engine, and utilizing the easy-to-learn Lua scripting language, NIBBLE8 encourages everyone from seasoned developers to game dev novices to invent, share, and revel in their own game creations.

Immerse yourself in the irresistible charm of nostalgia while exploring the capabilities of contemporary game development with NIBBLE8. 

### Building NIBBLE8 SDL2 Version:

#### Dependencies:
- SDL2
- SDL2_image
- libpng
- pkg-config

#### Build Steps:
1. Install dependencies:
   - For macOS (using Homebrew):
     ```bash
     brew install sdl2 sdl2_image libpng pkg-config
     ```
   - For Ubuntu/Debian:
     ```bash
     sudo apt-get install libsdl2-dev libsdl2-image-dev libpng-dev pkg-config
     ```
   - For Windows (using MSYS2):
     - Install MSYS2 from [here](https://www.msys2.org/).
     - Open MSYS2 terminal and install dependencies:
       ```bash
       pacman -S zip mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-libpng mingw-w64-x86_64-pkg-config mingw-w64-x86_64-toolchain mingw-w64-x86_64-make
       ```
2. Navigate to the NIBBLE8 directory.
3. Build using make (mingw32-make for MSYS2):
   ```bash
   make VERSION=sdl2

### Building NIBBLE8 DOS Version:

#### Dependencies:
- DJGPP cross-compilation environment
- Additional libraries:
  - PNG 1.6.40: [png1640b.zip](https://www.delorie.com/pub/djgpp/current/v2tk/png1640b.zip)
  - Zlib 1.3: [zlb13b.zip](https://www.delorie.com/pub/djgpp/current/v2tk/zlb13b.zip)
  - Allegro 4.2.2: [all422ar2.zip](https://www.delorie.com/pub/djgpp/current/v2tk/allegro/all422ar2.zip)

#### Build Steps:
1. Set up DJGPP cross-compilation environment by following the instructions at [build-djgpp](https://github.com/andrewwutw/build-djgpp).
2. Unpack the additional libraries to the DJGPP folder.
3. Source the DJGPP environment setup script (replace /usr/local/djgpp with the actual path to your DJGPP installation if different):
   ```bash
   source /usr/local/djgpp/setenv
   ```
4. Navigate to the NIBBLE8 directory.
5. Run make with the DOS configuration Makefile:
   ```bash
   make -f config/Makefile.dos
   ```
## License

NIBBLE8 is licensed under the [MIT License](https://github.com/jenissimo/NIBBLE8/blob/main/LICENSE).

## Contact

For more information, support, or if you'd like to get involved, please email us at jenissimo@gmail.com.

Dive into the nostalgic charm of classic gaming and the excitement of modern game development with NIBBLE8!
