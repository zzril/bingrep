#! /bin/sh -e

program="./bin/bingrep"
library="./lib/bin/bingrep.so"

command=$program

ELF_MAGIC_NUMBER="7f454c46"

make
printf "\nChecking \"$program\"...\n"
$command "$ELF_MAGIC_NUMBER" "$program"
printf "\nChecking \"$library\"...\n"
$command "$ELF_MAGIC_NUMBER" "$library"


