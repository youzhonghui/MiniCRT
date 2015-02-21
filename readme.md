32bit是俞甲子的MiniCRT的原版代码，64bit是我修改过的。

在64位机上，32位的代码编译会出错。

## 使用方法：

** 按个人情况进入32bit或者64bit文件夹。 **

### window (手动编译链接)

```
>cl /c /DWIN32 /GS- entry.c malloc.c printf.c stdio.c string.c

>lib entry.obj malloc.obj printf.obj stdio.obj string.obj /OUT:minicrt.lib
>cl /c /DWIN32 test.c
>link test.obj minicrt.lib kernel32.lib /NODEFAULTLIB /entry:mini_crt_entry
```

### linux (手动编译链接)

```
# gcc -c -fno-builtin -nostdlib  entry.c malloc.c stdio.c string.c printf.c
# ar -rs minicrt.a malloc.o printf.o stdio.o string.o
# gcc -c -ggdb -fno-builtin -nostdlib test.c
# ld -static -e mini_crt_entry entry.o test.o minicrt.a -o test
```

#### linux (make)

感谢** Guo Lu **贡献的makefile。

```
make
./test
make clean
```
