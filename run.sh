#! /bin/sh -e

command="./bin/bingrep"

ELF_MAGIC_NUMBER="7f454c46"

make

for example in examples/*; do
	printf "\nChecking for ELF magic number in \"$example\"...\n"
	$command -v "$ELF_MAGIC_NUMBER" "$example" || true
done


