#!/bin/bash

projectrDir=$(cd `dirname $0`; pwd)

arduino-cli --config-file ${projectrDir}/arduino-cli.yaml compile -v \
  --fqbn esp8266:esp8266:nodemcuv2 ${projectrDir} --build-path ${projectrDir}/dist

arduino-cli --config-file ./arduino-cli.yaml upload --port /dev/cu.SLAB_USBtoUART \
  --fqbn esp8266:esp8266:nodemcuv2 ${projectrDir} --input-dir ${projectrDir}/dist