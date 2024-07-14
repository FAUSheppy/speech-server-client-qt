# Intro
This is the GUI Interface to a proprietary server that interfaces with speech recognition backends.

# Dependencies

    apt-get install libqt5gui5

![GUI Main-Window](https://media.atlantishq.de/pictures/qt_client_dm.png)

# Build on Linux
This is a qt-creator project and can be imported as such. To build for Windows (on Linux) you need [MXE](https://github.com/mxe/mxe.git).

To download and compile MXE and dependencies:

    git clone https://github.com/mxe/mxe.git
    cd mxe && make qtbase
    make -j 4 qt qt5 MXE_TARGETS='x86_64-w64-mingw32.shared i686-w64-mingw32.shared x86_64-w64-mingw32.static i686-w64-mingw32.static'
    make qtbase

Setup the actual compilation enviroment:

    MXE_ROOT=/path/to/mxe/root/
    export PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:$MXE_ROOT/usr/bin
    $MXE_ROOT/usr/bin/i686-w64-mingw32.static-qmake-qt5
    make

Also check the ``cross_compile.sh`` file in this project.

# Mac Support?
I do not own a Mac, so I haven't tried, but there is no reason why it wouldn't run on a Mac.

# Stylesheet
The stylesheet used is "Qt dark orange" by [LoneWolf](https://discourse.techart.online/t/release-qt-dark-orange-stylesheet/2287#post14381).
