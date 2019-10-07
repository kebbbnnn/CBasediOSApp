#!/bin/bash
printf "\e[32mDeleting infer-out/ directory.\e[m\n"
rm -rf infer-out/
xcodebuild -target CBasediOSApp  -configuration Debug -sdk iphonesimulator clean
infer run --reactive -- xcodebuild -target CBasediOSApp -configuration Debug -sdk iphonesimulator