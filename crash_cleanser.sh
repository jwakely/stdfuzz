#!/bin/sh

me=$(basename $0)
usage() {
    echo "usage: $me fuzzer crashcase"
    exit 0
}


if [ $# -ne 2 ] ; then
    usage
fi

fuzzer=$1
crashcase=$2

set -e

cp $crashcase minimizedcrash
$fuzzer -minimize_crash=1 -exact_artifact_path=minimizedcrash -max_total_time=10 $crashcase
cp minimizedcrash cleansedcrash
$fuzzer -cleanse_crash=1 -exact_artifact_path=cleansedcrash -max_total_time=10 minimizedcrash

echo $me: all is good
