droidguardian
=============

DroidGuardian is an application firewall for Android.

DroidGuardian allows users to receive alert dialogs when an outgoing internet connection is requested by any app (or Unix process). This tool intercepts all internet socket connection requests at kernel level. Based on rules created by a native JNI library, connection attempts may be displayed to the user that has the power to either accept or reject such connections.

This is a first protoptype of a security feature that help users to be aware of what internet connections are their apps doing.
