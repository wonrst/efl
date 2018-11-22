#!/bin/sh

set -e
. .ci/travis.sh
if [ "$1" = "release-ready" ] ; then
  exit 0
fi
travis_fold make make
if [ "$BUILDSYSTEM" = "ninja" ] ; then
  if [ "$DISTRO" != "" ] ; then
    docker exec --env MAKEFLAGS="-j5 -rR" --env EIO_MONITOR_POLL=1 $(cat $HOME/cid) ninja -C build
  else
    export PATH="$(brew --prefix gettext)/bin:$PATH"
    ninja -C build
  fi
else
  if [ "$DISTRO" != "" ] ; then
    if [ "$1" = "coverity" ] ; then
      docker exec --env MAKEFLAGS="-j5 -rR" --env EIO_MONITOR_POLL=1 $(cat $HOME/cid) sh -c "rm -rf cov-int/"
      docker exec --env MAKEFLAGS="-j5 -rR" --env EIO_MONITOR_POLL=1 --env PATH="/cov/cov-analysis-linux64-2017.07/bin:$PATH" $(cat $HOME/cid) sh -c "cov-configure --comptype gcc --compiler /usr/bin/gcc"
      docker exec --env MAKEFLAGS="-j5 -rR" --env EIO_MONITOR_POLL=1 --env PATH="/cov/cov-analysis-linux64-2017.07/bin:$PATH" $(cat $HOME/cid) sh -c "cov-build --dir cov-int make"
      docker exec --env MAKEFLAGS="-j5 -rR" --env EIO_MONITOR_POLL=1 $(cat $HOME/cid) sh -c "tar caf efl-$(git describe).xz cov-int"
    else
      docker exec --env MAKEFLAGS="-j5 -rR" --env EIO_MONITOR_POLL=1 $(cat $HOME/cid) make
    fi
  else
    export PATH="$(brew --prefix gettext)/bin:$PATH"
    make
  fi
fi
travis_endfold make
