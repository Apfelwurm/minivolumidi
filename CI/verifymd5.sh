#!/bin/bash
FILES=/tmp/*.bin
for f in $FILES
do
  echo "Processing $f file..."
  ORIGNAME=$f
  md5=($(md5sum $ORIGNAME))
  filename=$(basename -- "$ORIGNAME")
  filename="${filename%.*}"

  otamd5=$(curl -k -sS -u $ESPOTA_USER:$ESPOTA_PASSWD -F "firmware=$filename" https://sv-smrthm01:1880/esp8266-ota/checkmd5)

  echo $otamd5
  if [ $md5 != $otamd5 ]; then
        echo "md5 missmatch!"
        exit 99
  fi

done

echo "finished!"

