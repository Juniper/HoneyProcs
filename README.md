HoneyProcs is a Deception Tool for Endpoints.

* Development
** Checking out the source code

   While holding the code/source/repo on the filesystem, make sure the word
   HoneyProcs is the name of the root folder only, and the word isn't
   there anywhere else in the path.

   For example,

   Correct:
   /home/user/development/repos/HoneyProcs
   /home/user/development/HoneyProcs

   Wrong:
   /home/user/HoneyProcs/repos/HoneyProcs - HoneyProcs seen twice here

** Build environment Setup

   For development and building the tool you need Cygwin installed, with
   MS Visual Studio environment loaded into Cygwin's environment/bash.

   Set the environment variable HONEYPROCS_BUILD_ROOT to a linux style
   path inside cygwin bash so that all the compiled and built binaries
   go to this directory.

** Build Command

   From the root directory run "make".
