#!/bin/bash -e

BUILD_DIR=./build-all-$RANDOM
CMAKEFILEDIR=$(dirname "$(readlink -f "$0")")


if [[ -d $BUILD_DIR ]]; then
  echo "try again"
  exit 1
fi

CMAKECMD="cmake $CMAKEFILEDIR"
BUILDCMD="make -j"

mkdir $BUILD_DIR && cd $BUILD_DIR

$CMAKECMD -DMALC=on
$BUILDCMD
mv logger-bench ../malc-bench

$CMAKECMD -DMALC=off -DMAL=on
$BUILDCMD
mv logger-bench ../mal-bench

$CMAKECMD -DMALC=off -DMAL=off -DSPDLOG=on
$BUILDCMD
mv logger-bench ../spdlog-bench

$CMAKECMD -DMALC=off -DMAL=off -DSPDLOG=off -DGLOG=on
$BUILDCMD
mv logger-bench ../glog-bench

$CMAKECMD -DMALC=off -DMAL=off -DSPDLOG=off -DGLOG=off -DNANOLOG=on
$BUILDCMD
mv logger-bench ../nanolog-bench

$CMAKECMD -DMALC=off -DMAL=off -DSPDLOG=off -DGLOG=off -DNANOLOG=off -DG3LOG=on
$BUILDCMD
mv logger-bench ../g3log-bench

$CMAKECMD -DMALC=off -DMAL=off -DSPDLOG=off -DGLOG=off -DNANOLOG=off -DG3LOG=off -DNANOLOG_IYENGAR=on
$BUILDCMD
mv logger-bench ../nanolog-iyengar-bench

cd -
rm -rf $BUILD_DIR
