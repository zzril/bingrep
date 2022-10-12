#! /bin/sh -e

ELF_MAGIC_NUMBER="7f454c46"

make && echo && ./bingrep "$ELF_MAGIC_NUMBER" ./bingrep


