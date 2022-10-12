bingrep
=======

Find byte sequences in a file.

Info:
-----

A small command-line utility for searching byte signatures within a file.  
Should be somewhat script-friendly and can be for example used to find  
"hidden" files inside a file, as in some forensics/steganography CTF challenges.

I mainly made this for getting some more practice in `C`, though.

Usage:
------

`./bin/bingrep [-chsv] hexstring filename`, e. g.  
`./bin/bingrep -v deadbeef somefile.jpg`.

Options:
* `-c`: Count: Instead of normal output, only print the number of matches.
* `-h`: Help: Print a usage message (with options) and exit.
* `-s`: Silent: Do not output anything. Return code will (as in the other  
cases) be 0 iff at least one match was found and no error occured.
* `-v`: Verbose: Print actual text, not just numbers. Always recommended if  
your're not running this as part of a pipeline.

Build:
------

* `make` will compile everything and put the binary in `./bin/bingrep`.
* `make lib` will create a shared library in `lib/shared/bingrep.so`, although  
there's no use for it at this point.
* `make clean` will clean everything for a future clean build.

Test:
-----

* The script `run.sh` will run some tests on the examples in `./examples` with  
the verbose flag.

Contribute:
-----------

If you have any suggestions, notice any bugs etc., feel free to open an issue  
about it.  
Direct contributions via pull requests are also welcome.


