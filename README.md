# Todd

## initrd.img
To run the operating system, you now need an initial ramdisk. You can
create it like this:

```bash
qemu-img create initrd.img <SIZE>
```

For the size you can use something like *64M* for 64 megabytes.
