# homework 3

## 5-b

### usage

```bash
$ g++ 5-b-encode.c -o build/5-b-encode
$ g++ 5-b-decode.c -o build/5-b-decode
$ ./build/5-b-encode data.csv -o data.bin
$ ./build/5-b-decode data.bin -o data.csv
```

### output

```
data.csv:
Id,Name,Department,Age
2014310512,Nima Wang,E.E.,22
2014310513,John Smith,C.S.,23
2014310514,David Brown,E.E.,24
2014310515,John Doe,C.S.,25
2014310516,David White,E.E.,26
2014310517,John Black,C.S.,27
2014310518,David Green,E.E.,28
2014310519,John White,C.S.,29
2014310520,David Black,E.E.,30
2014310521,John Green,C.S.,31
2014310522,David White,E.E.,32
2014310523,John Black,C.S.,33
2014310524,David Green,E.E.,34
2014310525,John White,C.S.,35

data.bin:
p�x	Nima WangE.E.q�x
John SmithC.S.r�xDavid BrownE.E.s�xJohn DoeC.S.t�xDavid WhiteE.E.u�x
John BlackC.S.v�xDavid GreenE.E.w�x
John WhiteC.S.x�xDavid BlackE.E.y�x
John GreenC.S.z�xDavid WhiteE.E. {�x
John BlackC.S.!|�xDavid GreenE.E."}�x
John WhiteC.S.#
```