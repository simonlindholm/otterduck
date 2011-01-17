The Cracking Good Adventures of Otter and Duck
==============================================

A cooperative 2D platformer featuring Otter and Duck.
See http://piratepad.net/otterduck for more information about the project.

Building/running
----------------

Like with [NieXS's game](https://github.com/NieXS/Otter--n-Duck), the building is done using waf, which is quite a lot nicer than handmade Makefiles. This means you have to have python installed. The compiler options included are for g++, so if you use any other compiler you might have to change wscript to work or translate the flags to your compiler's language. The game uses C++0x in places; if this doesn't work, just change the source code. The build instructions are as follows:

* Install Clanlib 2.2. Compiling it from source, as is needed on Linux, takes quite a long time, so you may want to <s>grab a cup of coffee</s> port the game to SDL while it compiles.
* Run `./waf configure`.
* Run `./waf` to compile the game.
* Go to the bin/ directory and run `../build/otterduck` to run the game, or run `./a.out` which does exactly this.
