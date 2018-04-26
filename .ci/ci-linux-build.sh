#!/bin/sh

set -e

PARALLEL_JOBS=10

CI_BUILD_TYPE=$1

DEFAULT_COPTS="--prefix=/usr/ --with-tests=regular --disable-cxx-bindings"

WAYLAND_COPTS="--with-tests=regular --enable-wayland --enable-elput --enable-drm \
--enable-wayland-ivi-shell --enable-gl-drm --with-opengl=es --enable-egl"

MISC_COPTS="--enable-harfbuzz --enable-liblz4 --enable-image-loader-webp --enable-xinput22 \
--enable-multisense --enable-lua-old --enable-xpresent --enable-hyphen \
--enable-pixman --enable-pixman-font --enable-pixman-rect --enable-pixman-line \
--enable-pixman-poly --enable-pixman-image --enable-pixman-image-scale-sample \
--enable-image-loader-generic --enable-libuv --enable-tile-rotate --enable-vnc-server \
--enable-sdl --enable-fb --enable-v4l2 --enable-cserve --enable-always-build-examples \
--enable-ecore-wayland --enable-ecore-drm --enable-cancel-ok --with-crypto=gnutls \
--enable-debug --disable-gstreamer1 --enable-gstreamer"

MISC_DISABLED_COPTS="--disable-neon --disable-libeeze --disable-systemd --disable-magic-debug \
--disable-valgrind --disable-cxx-bindings --disable-gstreamer1 \
--disable-fontconfig --disable-fribidi --disable-poppler --disable-spectre --disable-libraw \
--disable-librsvg --disable-xcf --disable-libmount --disable-tslib --disable-audio \
--disable-pulseaudio --disable-avahi --disable-xinput2 --disable-xim --disable-scim \
--disable-ibus --disable-physics --disable-quick-launch --disable-elua"

RELEASE_READY_COPTS="--with-profile=release"

if [ "$CI_BUILD_TYPE" = "" ]; then
  # Normal build test of all targets
  ./autogen.sh $DEFAULT_COPTS
  make -j $PARALLEL_JOBS
  make -j $PARALLEL_JOBS check-build
  make -j $PARALLEL_JOBS examples
  make -j $PARALLEL_JOBS benchmark
  make -j $PARALLEL_JOBS install
  ./.ci/build-efl-app.sh
fi

if [ "$CI_BUILD_TYPE" = "wayland" ]; then
  ./autogen.sh $WAYLAND_COPTS
  make -j $PARALLEL_JOBS
  make -j $PARALLEL_JOBS examples
fi

if [ "$CI_BUILD_TYPE" = "misc" ]; then
  ./autogen.sh $MISC_COPTS
  make -j $PARALLEL_JOBS
  make -j $PARALLEL_JOBS examples
fi

if [ "$CI_BUILD_TYPE" = "misc-disabled" ]; then
  ./autogen.sh $MISC_DISABLED_COPTS
  make -j $PARALLEL_JOBS
  make -j $PARALLEL_JOBS examples
fi

if [ "$CI_BUILD_TYPE" = "release-ready" ]; then
  ./autogen.sh $RELEASE_READY_COPTS
  make -j $PARALLEL_JOBS
  make -j $PARALLEL_JOBS distcheck
fi

# Known non-working options:
# --enable-g-main-loop fails to build
# --enable-ecore-buffer fails build in examples
# --enable-libvlc need rpmfusion repo
# --enable-xine needs rpmfusion repo
# --disable-threads configure: error: Threads are required.
# --enable-elua does not work in combination with --enable-lua-old??
# --enable-tizen needs some tizen system lib
# --enable-csharp-bindings work in progress
# Javacript bindings work in progress
# --enable-gesture unclear how to get Xgesture dep
# alsa is not detected
# --enable-debug-threads crashed build in examples with locked threads
# --enable-eglfs needs hwcomposer.h on an android build

# Still need to cover:
# --with-profile=debug AND release
# --with-crypto=none
