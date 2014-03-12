droidguardian
=============

DroidGuardian is an application firewall for Android.

DroidGuardian allows users to receive alert dialogs when an outgoing Internet connection is requested by any app (or Unix process). This tool intercepts all Internet socket connection requests at kernel level. DroidGuardian uses a rule-based model to filter connection requests. Whenever a new connection is launched, DroidGuardian displays a dialog window so that users can either allow or deny such request. The dialog presents the IP address and port of the remote server, and the name and identifier of the process that is trying to get connected.

This is a first protoptype of a security feature that help users to be aware of what is going on under the hood regarding Internet traffic.

------

Tested on Android emulator:
 - Android 4.2
 - Kernel Goldfish 2.6.29

-----

Files:
 - DroidGuardian: Eclipse project files of the DroidGuardian app
 - droidg: custom Linux Kernel Module files (should be placed inside the /security folder)
 - Kconfig: patch to the file /security/Kconfig
 - Makefile: patch to the file /security/Makefile
 - zImage: Android kernel image already compiled (start emulator with "-kernel zImage")
