#!/bin/bash
FILES=./lib/private/PrivateConfig*.hpp
for f in $FILES
do
  echo "Processing $f file..."
  # take action on each file. $f store current file name
  ORIGNAME=$f
  NEWNAME=$(printf '%s' "$f" | sed 's/[0-9]//g')
  LEDSRVNR=$(printf '%s' "$f" | sed 's/[^0-9]*//g')
  ORIGKEYNAME=./src/SSLCert${LEDSRVNR}.key.h
  ORIGCRTNAME=./src/SSLCert${LEDSRVNR}.crt.h
  mv $ORIGNAME $NEWNAME
  mv $ORIGKEYNAME ./src/SSLCert.key.h
  mv $ORIGCRTNAME ./src/SSLCert.crt.h
  platformio run
 ret=$?
  if [ $ret -ne 0 ]; then
        mv $NEWNAME $ORIGNAME
        mv ./src/SSLCert.key.h $ORIGKEYNAME
        mv ./src/SSLCert.crt.h $ORIGCRTNAME
        exit 99
  fi
  mv $NEWNAME $ORIGNAME
  mv ./src/SSLCert.key.h $ORIGKEYNAME
  mv ./src/SSLCert.crt.h $ORIGCRTNAME
  #dir $FILES
  #echo $ORIGKEYNAME
  #echo $ORIGCRTNAME
done
ls /tmp/*.bin
rm -rf ./.piolibdeps
rm -rf ./.pioenvs
echo "finished!"

