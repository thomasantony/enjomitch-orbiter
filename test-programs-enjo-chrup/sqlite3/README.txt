For source, see:
http://sourceforge.net/projects/enjomitchsorbit

==============
1. Compiling:
==============

--------------
1.1 Windows
--------------

1) Download and install Tortoise SVN. Restart the computer to enable shell integration.
2) Download and install MinGW and MSYS.
3) Run MSYS and type the following commands to create a directory:

mkdir sql-programs

4) From Explorer, right click on the created directory (it's inside <path_where_you_installed_msys>/msys/1.0/home/<your_user_name>), and select SVN Checkout.
5) Paste the following URL into the "URL of repository" field:
https://enjomitchsorbit.svn.sourceforge.net/svnroot/enjomitchsorbit/test-programs-enjo-chrup
6) Click OK and wait until Tortoise SVN creates a "working copy" and populates it with the source.
7) Download SQLite3 source from http://sqlite.org/download.html (download the .tar.gz distribution)
8) Copy the sqlite source to MSYS home/<your_user_name> directory
9) From MSYS, execute the following commands:

tar -xzvf sqlite-amalgamation-3.6.4.tar.gz 
(NOTE: the file version and name may change. Modify appropriately or enter 
tar -xzvf sqlite<TAB>
for autocompletition)
cd sqlite-3.6.4
cp sqlite3.c sqlite3.h sqlite3ext.h ../sql-programs/sqlite3/common/

You now have SQLite3 sources where they are needed.

10) To compile a given program, enter the following commands:

cd
cd sql-programs/sqlite3/<program's_name>
make

11) To clean a project, enter:

make clean
(rarely needed)

12) To run a program, enter

./name_of_program
or
./<TAB>

13) To update your working copy with fresh sources, right click on sql-programs, and select SVN Update


NOTE: If you become a developer, upload only source and eventually Makefiles but don't upload SQLite3 sources, since they can be downloaded separately.
