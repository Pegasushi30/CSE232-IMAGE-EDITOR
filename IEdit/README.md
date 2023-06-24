projeyi linux üzerinde açmak için:

```bash
gcc iedit.c gfx.c -o iedit -lX11 -lm
```

projeyi macos üzerinde açmak için:

```bash
gcc iedit.c gfx.c -o iedit -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lm

```


windows üzerinde çalıştırılamaz!