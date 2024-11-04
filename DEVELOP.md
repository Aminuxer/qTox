# Develop in eclipe-cdt
In this manual we will use Eclipse-CDT environment. All developments are done on Fedora Linux.

## Install dependencies
Cmake dependencies on Fedora are stored at `/usr/lib64/cmake/` directory, it is the place where `find_package(XXXX           REQUIRED)` directive will try to find it. The package providing it can be found using command

```bash
dnf provides /usr/lib64/cmake/XXXX
```

The next packages are required to develop qTox:

```bash
dnf install cmake qt5-qtbase-devel qt5-linguist qt5-qtsvg-devel toxcore-devel libasan libavcodec-free-devel libavdevice-free-devel libavformat-free-devel libavutil-free-devel libavif-devel qrencode-devel libsodium-devel libswscale-free-devel sqlcipher-devel libvpx-devel libexif-devel kf5-sonnet-devel openal-soft-devel libXScrnSaver-devel rpm-build rpm-sign
```

## Build toxext
The qTox project is dependfent on [toxext](https://github.com/toxext/toxext) and [tox_extension_messages](https://github.com/toxext/tox_extension_messages). We need to build it before building our project. This project was forked to [new location](https://github.com/nickolay168/toxext).

```bash
export ECLIPSE_VER="4.31"
cmake -G "Eclipse CDT4 - Unix Makefiles" ./ -DCMAKE_ECLIPSE_VERSION=${ECLIPSE_VER}
```

`toxext` can be build by either make or from Eclipse or using make.

```bash
make ToxExt
sudo make install
```

## Build tox_extension_messages
After `toxext` build the `tox_extension_messages`. [Original location](https://github.com/toxext/tox_extension_messages), [forked location](https://github.com/nickolay168/tox_extension_messages)

```bash
cmake -G "Eclipse CDT4 - Unix Makefiles" ./
make ToxExt
sudo make install
```

## Finally build qTox and create RPM package
This section is valid for Fedora Linux only.

```bash
export VERSION="1.17.6"
cmake -G "Eclipse CDT4 - Unix Makefiles" ./
make package
rpm --addsign qtox-${VERSION}-fc41.x86_64.rpm
rpm --checksig qtox-${VERSION}-fc41.x86_64.rpm
# Check the dependencies
rpm -qp qtox-${VERSION}-fc41.x86_64.rpm --requires
```
