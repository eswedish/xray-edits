# xray

OBTAINING SOFTWARE:

For this program, used Geant4 10.7.2 so for max compatibility recommend using same version.

Linked source to download:
https://geant4.web.cern.ch/node/1914

Installation guide:
https://gentoo.c3sl.ufpr.br/distfiles/Geant4InstallationGuide-4.10.7.pdf

For the build options, for our purposes you can leave all the build options off besides:
GEANT4_BUILD_MULTITHREAD (note - having this means we have to make sure our program is thread safe)
GEANT4_INSTALL_DATA
GEANT4_QT (for interactive UI and visualization -- useful when not running in multithread mode)
GEANT4_USE_RAYTRACER_X11
GEANT4_USE_SYSTEM_EXPAT


Depending on experience level and just general bugs, building and installing from source (if this is the path you take, as I did) can can be a tedious process  -- if having troubles I highly recommend following along with this video:
https://www.youtube.com/watch?v=Lxb4WZyKeCE&list=PLLybgCU6QCGWgzNYOV0SKen9vqg4KXeVL


Confirm your installation was successfull (for example try running one of the examples)

To set all libraries and paths to use for our project, source our geant4 make file (recommend putting this in your bash profile so don't have to do it each time)

RUNNING PROGRAM (locally):

Go to our project directory and create a build folder within

Move into build folder and execute:
cmake ..

So long as no error message, can continue on to execute:
make -jN      (N = # of Cores)

Then given this brings no error (will probably be warnings for unused variables), execute the simulation:
./sim

After job is complete, should get output ROOT file.
To get info about the energy deposited in the different volumes, run:
root -l ../mt_combiner.c

This will result in a print out of the energy deposited in the volumes in MeV as well as a single combined root file that combined all the aforementioned ROOT files from the threads.
The various graphs will also pop up with this command.

To look through the graphs later, can simply open ROOT:
root

Then open up a TBrowser:
new TBrowser

Navigate to the name of the of the combined file (usually has 'allgraphs' within the name) then click on the TTree you want to see a plot of


NOTES:
For the current upload of the project, it is in multithread mode due to high volume of work to be done, and should be fine to keep that way unless trying to get the visualization.

For geometry, first see if desired material/element is within Geant4 Material Database:
https://geant4-userdoc.web.cern.ch/UsersGuides/ForApplicationDeveloper/html/Appendix/materialNames.html#g4matrdb

CLUSTER: !! NOT SURE WHETHER TO INCLUDE...
If using the cluster to execute these, you will similarly have to manually download the source code of Geant4 and do the same setup process (don't use the version on cluster, not entirely compatible)

Do both the cmake .. and make commands on the cluster interactively before submitting the job
