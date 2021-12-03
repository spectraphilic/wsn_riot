Howto LittleFS
==============

To read an SD card with a LittleFS filesystem you need fuse, including headers.
On Debian systems:

    # apt install fuse libfuse-dev

The checkout the program and build:

    $ git clone https://github.com/littlefs-project/littlefs-fuse
    [...]
    $ cd littlefs-fuse
    $ make
    [...]

To format the SD card (replace sdX by your device name):

    $ ./lfs --format /dev/sdX

You can also format the SD card with the wsn-shell program.

To mount the filesystem:

    $ mkdir mount
    $ ./lfs /dev/sdX mount

When finished unmount:

    $ unmount mount


Troubleshooting
---------------

- ``error: Permission denied``

  Your user many not have access to the device. In my (Gentoo Linux) system I
  had to add my user to the ``disk`` group.


Links
-----

- https://github.com/littlefs-project/littlefs-fuse
