Dingoo SDK




CONTENTS

1 Installation
	1.1 Windows Installation
	1.2 Unix Installation
	1.3 Updating
	1.4 Building the documentation

2 Compiling the sample programs






1 - INSTALLATION

Need:
    compiler tool chain, available from http://code.google.com/p/dingoo-sdk/downloads/
    dingoo native sdk, either from svn checkout or prebuilt from http://code.google.com/p/dingoo-sdk/downloads/

1.1 - WINDOWS INSTALLATION

Windows users will need to download the following files:
	The latest version of cygwin: http://www.cygwin.com/setup.exe
	A copy of the MIPS compiler tools: 
        mipsel-gcc4.1-cygwin-nopic.tar.bz2

First install cygwin by executing setup.exe.
	- Click "Next".
	- Select "Install from Internet", Click "Next".
	- Set the install path to what you want, Select "All Users", Click "Next".
	- Set the "Local Package Directory" to a subdirectory in your cygwin install path (e.g. c:\cygwin\downloads), Click "Next".
	- Let the installer create the directory if it asks.
	- Select the correct proxy settings (probably "Direct Connection") and click "Next".
	- Wait for the mirror list to download.
	- Select any mirror, if one doesn't work then select another, Click "Next".
	- Wait for the package list to download.
	- Select "Devel" in the package tree
		- Toggle the package "make" (not "automake") to "install"
		- Toggle the package "doxygen" to "install" (optional if you want documentation)
		- Click "Next".
	- Wait for the packages to download and install.
	- Click "Finish", cygwin is now installed.

Now you need to install those MIPS compiler tools you downloaded:
	- Unzip the compiler tools into the directory you installed cygwin to (e.g. If cygwin was installed to "c:\cygwin" then there will be a "c:\cygwin\mipsel-gcc4.1-nopic" directory).

Now you need to setup your cygwin environment:
	- Go to the directory in which you installed cygwin and open "cygwin.bat"
	- At in the middle of the file before the line "bash --login -i" add the following lines:
		SET MIPSTOOLS=/mipsel-gcc4.1-nopic
		SET DINGOO_SDK=<dingoo_sdk install path>/dingoo_sdk
		SET PATH=%PATH%;%MIPSTOOLS%/bin;%DINGOO_SDK%
NB: Your dingoo_sdk path has to be posix style (e.g "c:\dingoo_sdk" would be "/cygdrive/c/dingoo_sdk").

Now for the final step, open up cygwin from the icon in your start menu or desktop:
	- Type "cd $DINGOO_SDK"
	- Type "sh ./install"

The dingoo SDK is now installed.



1.2 - UNIX INSTALLATION

Linux users will need to do the following:
	- Download mipsel-4.1.2-nopic.tar.bz2
	- Install these compiler tools to an appropriate place.
	- Set environment variables:
		- Set the environment variable MIPSTOOLS to the location of your mips compiler tools.
		- Set the environment variable DINGOO_SDK to this directory.
		- Set the environment variable PATH to "$PATH:$MIPSTOOLS/bin"

	  Example (assuming compiler and SDK in current directory):

		MIPSTOOLS=`pwd`/mipsel-4.1.2-nopic
		export MIPSTOOLS

		DINGOO_SDK=`pwd`/dingoo-sdk/dingoo_sdk
		export DINGOO_SDK

		PATH="$PATH:$MIPSTOOLS/bin"

	- Type "cd $DINGOO_SDK && sh ./install"

The dingoo SDK is now installed.



1.3 - Updating

To update your current SDK, download the newest version and overwrite your current install,
then type:
	make install

This can take a while, if you only want to install core libraries or just ignore samples then the following options are quicker.
	make install_core_libs # Just installs the core libraries
	make install_core      # Just installs the core libraries and samples

On the other hand if you want a complete installation including compiled samples and tests then run:
	make install_full



1.4 - Building the documentation

To build the documentation, you need to run:
	make -C $DINGOO_SDK documentation
HTML and XML documentation is saved to $DINGOO_SDK/doc/html and $DINGOO_SDK/doc/xml, respectively. You can easily modify the Doxyfile to include other formats as you need them, but the HTML documentation is the most user-friendly, just open $DINGOO_SDK/doc/html/index.html in your web browser.




2 - COMPILING THE SAMPLE PROGRAMS

Open up a bash/cygwin shell.
Type the following commands:
	cd $DINGOO_SDK
	cd samples
	cd <sample dir>
	make
Now copy the .app file to your dingoo along with any required data files (.tga files usually).

To compile all the samples at once type:
	make -C $DINGOO_SDK samples
