# Forsaken

This repo holds the community port of Forsaken!

Check out the [Wiki](https://github.com/ForsakenX/forsaken/wiki) for more information.

# PM fork

This fork is an attempt at building the forsaken engine for [PortMaster](https://portmaster.games/). It is a Work In Progress. At the time of writing this the OpenGL v1 + SDL v1 build works partially:
- No sound
- Rendering works only with OpenGL support with the Panfrost GPU driver on rocknix.

# How to build

This procedure works on ubuntu 20.04 aarch64

## Install deps with the package manager

- build-essential
- libgl-dev
- liblua5.1-0-dev
- libopenal-dev
- libpng-dev
- libsdl2-dev (for SDL2)
- libsdl2-dev (for SDL1)
- zlib1g-dev

## Install manually old version of deps

### Libenet 1.2.5

```
wget http://sauerbraten.org/enet/download/enet-1.2.5.tar.gz
tar -xzf enet-1.2.5.tar.gz
cd enet-1.2.5
mv config.guess config.guess.original
mv config.sub config.sub.original
wget "http://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.guess;hb=HEAD" -O config.guess
wget "http://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=config.sub;hb=HEAD" -O config.sub
./configure
make -j4
make install
```

When it's done it should look like this
```
root@ebf47c554692:~# cat /usr/local/lib/pkgconfig/libenet.pc
prefix=/usr/local
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include

Name: libenet
Description: Low-latency UDP networking library supporting optional reliability
Version: 1.2.5
Cflags: -I${includedir}
Libs: -L${libdir} -lenet
root@ebf47c554692:~#
root@ebf47c554692:~#
root@ebf47c554692:~# ls -l /usr/local/lib/libenet.*
-rw-r--r-- 1 root root 290794 Aug 13 12:05 /usr/local/lib/libenet.a
-rwxr-xr-x 1 root root    936 Aug 13 12:05 /usr/local/lib/libenet.la
lrwxrwxrwx 1 root root     16 Aug 13 12:05 /usr/local/lib/libenet.so -> libenet.so.0.0.3
lrwxrwxrwx 1 root root     16 Aug 13 12:05 /usr/local/lib/libenet.so.0 -> libenet.so.0.0.3
-rwxr-xr-x 1 root root 171648 Aug 13 12:05 /usr/local/lib/libenet.so.0.0.3
root@ebf47c554692:~#
root@ebf47c554692:~#
root@ebf47c554692:~# pkg-config libenet --cflags --libs
-I/usr/local/include -L/usr/local/lib -lenet
```


### Luasocket 2.0

```
git clone https://github.com/lunarmodules/luasocket.git
cd luasocket
git checkout v2.0.2

echo "LUAINC=-I/usr/include/lua5.1" >> config
make -j4
make install

mkdir /usr/local/include/luasocket
cp src/luasocket.h /usr/local/include/luasocket
cp src/mime.h /usr/local/include/luasocket
cp src/unix.h /usr/local/include/luasocket

wget https://github.com/ForsakenX/forsaken-libs/raw/master/src/pkgconfig/lua-socket.pc
sed -i 's/prefix= \.\/libs/prefix= \/usr\/local/g' lua-socket.pc
sed -i 's/\/include/\/include\/luasocket/g' lua-socket.pc
sed -i 's/lua-mime/lua5\.1-mime/g' lua-socket.pc
sed -i 's/lua-socket/lua5\.1-socket/g' lua-socket.pc
sed -i 's/lua-mime/lua5\.1-mime/g' lua-socket.pc
sed -i 's/lua-socket/lua5\.1-socket/g' lua-socket.pc
cp lua-socket.pc /usr/local/lib/pkgconfig/lua5.1-socket.pc

cd /usr/local/lib
ln -s lua/5.1/mime/core.so liblua5.1-mime.so.2.0.2
ln -s lua/5.1/socket/core.so liblua5.1-socket.so.2.0.2
ln -s liblua5.1-mime.so.2.0.2 liblua5.1-mime.so
ln -s liblua5.1-socket.so.2.0.2 liblua5.1-socket.so
```

When it's done it should look like this
```
root@ebf47c554692:~# cat /usr/local/lib/pkgconfig/lua5.1-socket.pc
# lua.pc -- pkg-config data for Lua

V= 2.0
R= 2.0.2

# relative path from forsaken src folder
prefix= /usr/local
#prefix= /usr/local

# grep '^INSTALL_.*=' ../Makefile | sed 's/INSTALL_TOP/prefix/'
INSTALL_BIN= ${prefix}/bin
INSTALL_INC= ${prefix}/include/luasocket
INSTALL_LIB= ${prefix}/lib
INSTALL_MAN= ${prefix}/man/man1
INSTALL_LMOD= ${prefix}/share/lua/${V}
INSTALL_CMOD= ${prefix}/lib/lua/${V}

# canonical vars
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include/luasocket

Name: LuaSocket
Description: Network libraries for lua
Version: ${R}
Requires:
Libs: -L${libdir} -llua5.1-mime -llua5.1-socket
Cflags: -I${includedir}

# (end of lua.pc)
root@ebf47c554692:~#
root@ebf47c554692:~#
root@ebf47c554692:~# ls -l /usr/local/lib/liblua5.1-*
lrwxrwxrwx 1 root root 23 Aug 13 12:30 /usr/local/lib/liblua5.1-mime.so -> liblua5.1-mime.so.2.0.2
lrwxrwxrwx 1 root root 20 Aug 13 12:30 /usr/local/lib/liblua5.1-mime.so.2.0.2 -> lua/5.1/mime/core.so
lrwxrwxrwx 1 root root 25 Aug 13 12:57 /usr/local/lib/liblua5.1-socket.so -> liblua5.1-socket.so.2.0.2
lrwxrwxrwx 1 root root 22 Aug 13 12:57 /usr/local/lib/liblua5.1-socket.so.2.0.2 -> lua/5.1/socket/core.so
root@ebf47c554692:~#
root@ebf47c554692:~#
root@ebf47c554692:~# ls /usr/local/include/luasocket
luasocket.h  mime.h  unix.h
root@ebf47c554692:~#
root@ebf47c554692:~#
root@ebf47c554692:~# pkg-config lua5.1-socket --cflags --libs
-I/usr/local/include/luasocket -L/usr/local/lib -llua5.1-mime -llua5.1-socket
```

## Build the engine

### OpenGL v1 + SDL v1 build

```
git clone https://github.com/cdeletre/forsaken.git
cd forsaken
git checkout portmaster
make -j4
```

Only work with Panfrost. I haven't had success to get it work with GL4ES at the moment.

### OpenGL v2 + SDL v2 build

WIP

```
git clone https://github.com/cdeletre/forsaken.git
cd forsaken
git checkout portmaster
```

Edit `Makefile`:
- line 71: `SDL=2`
- line 79: `GL=2`

```
make -j4
```

Current status: The game crashes with "Thread 1 "projectx.aarch6" received signal SIGSEGV, Segmentation fault.". According to gdb it is coming from line 266 in `src/mxload.c`: `lpLVERTEX[i].x = old->x;`

More details about this here: [opengl2sdl2-run_001.txt](https://github.com/cdeletre/forsaken/blob/master/gdb/opengl2sdl2-run_001.txt)

### OpenGL ES v1 + SDL v2 build

in progress

- Use the `portmaster-gles` branch
- Set `GL=1` `SDL=2` `HAVE_GLES=1` in the Makefile

At run it generates GL error 1280 and 1282 on rocknix with MALI

## Run the game

```
cd forsaken
git clone https://github.com/ForsakenX/forsaken-data.git data
mkdir -p savegame logs configs pilots
./projectx.aarch64 -Debug
```

## Pack the game

```
mkdir -p pack/forsaken/forsaken
cp forsaken/projectx.aarch64 pack/forsaken/forsaken/
cp -R forsaken/PortMaster/libs.aarch64 pack/forsaken/forsaken/
cp -R forsaken/data pack/forsaken/forsaken/
mkdir -p pack/forsaken/forsaken/savegame pack/forsaken/forsaken/logs pack/forsaken/forsaken/configs pack/forsaken/forsaken/pilots
```

Also add `libSDL-1.2.so.0` from SDLCompat in `pack/forsaken/forsaken/libs.aarch64`.

If needed add gl4es libs in `pack/forsaken/forsaken/gl4es.aarch64`

## Debug with gdb

To get more debug information from gdb put `src` in `forsaken` if you're not running it from the build folder.

Run `gdb --args ./projectx.aarch64 -Debug`. In gdb prompt type `run` and press `enter`. if it crashed you can type `backtrace` and press `enter` to get more details about the function call history.

# TODO

- get an OpenGL v2 or OpenGL ES v2 working build with SDL2
- get the sound working
- pack it