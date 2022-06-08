# Language
## About
First, tokens are created by parsing, and then connected by recursive descent. BackEnd then creates an assembler file for my implementation [click](https://github.com/armanincredible/CPU). <br/> 
If you want to work with my language, you can also use my [compiler](https://github.com/armanincredible/Compiler)
## Grammar
This so easily, because it's similar to c language.
+ if     <=> VTURILAS
+ else   <=> VKRASHILAS
+ while  <=> VLYAPALAS
+ {}     <=> DORA DURA
+ return <=> POSHLU

Example of program that calculates factorial:
```
#x = scanf (#x);
#x = fact (#x);
fact (#x)
DORA
    VTURILAS (#x == 0)
    DORA
        POSHLU 1;
    DURA
    POSHLU #x * fact (#x - 1);
DURA;
printf (#x);$
```
!!! $ - it's the end of file !!!
