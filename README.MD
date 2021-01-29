# RND Libraries

This directory holds a collection of my self-written C libraries.
Some of these may be data structure implementations, wrappers,
helper scripts and so on. The list may grow longer in size.
All libraries are prefixed "RND" for "Randoragon" to avoid name
collisions.

The libraries are not necessarily "finished". I will never push
incomplete or buggy features on purpose, but the number of individual
features of each library may increase (adding new functions, etc.).
Also feel free to request new features.

## Contact

- Email: [<randoragongamedev@gmail.com>](mailto:randoragongamedev@gmail.com)
- Discord: Randoragon#1958
- Discord Server: https://discord.gg/YqJYzMS

## Documentation

[Read the online documentation here!](https://codedocs.xyz/Randoragon/rnd-libs)

All header files are thoroughly documented with [Doxygen](https://www.doxygen.nl).
The documentation is not included in the repository though,
you can generate it yourself with the following `make` command:

    make docs

To open the documentation, open the newly created `docs/html/index.html`
file in your internet browser of choice.

## Building

You can go into each directory individually and compile only what you need,
or you can use the makefile that resides in this directory and it will
automatically invoke the makefiles in each of the subdirectories.

Therefore, to install all libraries at once, run from the top directory
(if necessary, as root):

    make install

By default, header files will be installed to `/usr/local/include`
and shared libraries to `/usr/local/lib`.  If you want it installed elsewhere,
you can overwrite the PREFIX variable like so:

    make install PREFIX=/usr

The above command will install headers to `/usr/include` and libraries
to `/usr/lib` (PREFIX is set to `/usr/local` by default).
