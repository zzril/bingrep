#! /bin/sh

command="./bin/bingrep"

ELF_MAGIC_NUMBER="7f454c46"

make

for example in examples/*; do
	printf "\nChecking for ELF magic number in \"$example\"...\n"
	$command "$ELF_MAGIC_NUMBER" "$example"
done


