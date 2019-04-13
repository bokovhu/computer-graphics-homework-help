# Ray tracing - GLSL kiegészítés

## `in`, `out` függvényparaméterek

A GLSL-ben módunk van paramétereket referenciaként átvenni, de csak
úgy, hogy meghatározzuk, mit csinálunk a paraméterrel. Ha csak olvasható
módon szeretnénk egy paramétert átvenni, akkor az `in` kulcsszót elérakhatjuk,
ekkor már referenciaként fogjuk átvenni, de nem módosíthatjuk (ez olyan, mint
C#-ban az `in` kulcsszó, illetve mint C++-ban a `const Type&` referenciatípus).
Ha csak írható paramétert veszünk át, az `out` kulcsszót rakhatjuk oda elé,
ekkor írhatjuk a változó értékét. Ha írni **és** olvasni is szeretnénk a
referenciát, az `inout` kulcsszót használhatjuk.

Így gyakorlatilag bármilyen GLSL függvényt megírhatunk kétféleképpen. Az
alábbi példában két ekvivalens kód található.

```glsl
vec2 someFunction (vec2 param) {
	return param * 0.5;
}
```

```glsl

void someFunction (in vec2 param, out vec2 result) {
	result = param * 0.5;
}

```

Igazából csak tervezési kérdés, de optimalizációs szempontból az `in`-eket
mindenképp érdemes megjelölni, olyan függvényekben pedig, ahol `struct`-okat
adunk vissza **ÉN SZEMÉLYSZERINT** (és nem feltétlen követendő dolog ez) jobban
szeretem `out` változóként megadni, mint függvény visszatérési értékként. Ha pedig
egy függvénynek egyszerre több visszatérési értéke van, akkor érdemes több `out`
paramétert csinálni, mint egy külön structot a visszatérési értéknek.

## `struct`

A GLSL-ben vannak `struct`-ok. VIGYÁZAT! Ez közel nem OOP!! Ezek teljesen olyanok,
mint az ANSI C structok!