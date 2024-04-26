# Todd

## initrd.img
To run the operating system, you now need an initial ramdisk. You can
create it like this:

```bash
qemu-img create initrd.img <SIZE>
```

For the size you can use something like *64M* for 64 megabytes.

## Some notes
For some reason, and I don't know why, the version of GNU ld on Fedora 39 (2.42.50) doesn't work to build this. 2.34 works, and also the latest version from git, so I don't know. If you have issues with running anything, try compiling binutils yourself.
