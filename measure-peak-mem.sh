#!/bin/bash -e

for EXEC in $(find . -type f -name '*-bench'); do
  for CONFIG in $($EXEC 8MB 100000 own help | grep '    \[' | sed -e 's|^ *\[\([^]]*\)].*|\1|g'); do
    if [[ $CONFIG == "all" ]]; then
      continue
    fi
    echo -n "\"$CONFIG\" peak KB: "
    /usr/bin/time -f "%M" $EXEC 8MB 100000 own-max-threads-only 1 $CONFIG 1>&2 > /dev/null
    rm -f *.log nanolog*.txt spdl spdl.* compressedLog
  done
done
