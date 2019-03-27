# Grafika házifeladat segítség

Ezen dokumentum személyes tapasztalataimat méltatott megosztani annak érdekében, hogy
a gyakori, ám sok fejfájást okozó hibákra megoldást szolgáltasson, legyen szó az
OpenGL használatáról, vagy általános grafikai dolgokról. A dokumentumban megpróbálom
összegyűjteni azon gyakori hibajelenségeket, melyekkel már találkoztam, kereshető
formátumban, hogy az olvasó elkerülhesse a sok-sok debugolással töltött órát.

## Checklist

Amennyiben problémád akadt, ajánlom, hogy fuss végig ezen a listán, mert könnyen
elkövethető, de könnyen észlelhető és javítható problémákat tartalmaz.

* `glBufferData` **első** paramétere az **nem** a VBO ID-je, hanem az a buffer
target, ahová a VBO bindolva van (helyesen: `glBufferData (GL_ARRAY_BUFFER, ...)`)
* `glBufferData` **második** paramétere **nem** a feltöltött _elemszám_, hanem
a feltöltendő adat **mérete byte-okban**. Valószínüleg hiányzik egy `sizeof (float)`-tal való szorzás
* **Minden** használt shader esetén vizsgáld meg, hogy a shader lefordult-e, illetve,
hogy a programot lehetett-e linkelni! Amennyiben valami nem sikerült, a 
`glGetShaderInfoLog` és `glGetProgramInfoLog` függvényekkel nyerhető ki a hiba, 
a programodnak ebben az esetben valamilyen runtime hiba dobásával kell reagálnia!
* Vigyázni kell arra, hogy az OpenGL borzasztóan stateful, így ha egy rajzoláshoz,
vagy adatfeltöltéshez bind-oltál egy VAO-t, vagy VBO-t, azt a rajzolás, vagy egyéb
művelet után unbind-olni kell! Jellemzően ez úgy tehető meg, hogy ID-nek `0` értéket
adsz meg (`glBindVertexArray (0)`, `glBindBuffer (GL_ARRAY_BUFFER, 0)`, stb.)
* Minden `glVertexAttribPointer` függvényhívást meg kell, hogy előzzön egy 
`glEnableVertexAttribPointer` függvényhívás!
* Ha valami egészen furcsa képet kapsz, akkor nézd meg, hogy nem-e állítottad 
véletlenül a `glVertexAttribPointer` negyedik paraméterét `GL_TRUE`-ra!
* Minden `glGetUniformLocation` függvényhívás után meg kell nézni, hogy a visszaadott 
érték negatív-e! Amennyiben negatív, akkor 
	* elírtad az uniform nevét, VAGY 
	* a shader-ed nem fordult le, VAGY
	* a GLSL a nem használt uniform-okat kiszedi a shader-ből, így nézd meg, hogy
	használva van-e egyáltalán az adott uniform a shader-edben!
* Vertex shader-ben a `gl_Position` helyesen a következő kifejezéssel számítandó: 
`gl_Position = projMatrix * viewMatrix * modelMatrix * vertexPosition`, **ebben a 
sorrendben!**
* Ha a képed elmosódik amikor valami mozog, akkor nagy valószínűséggel lehagytad a 
`glClear (GL_COLOR_BUFFER_BIT)` függvényhívást, amely törli a képernyő tartalmát
* Amikor ortogonális vetítésű kamerát használsz, figyelj oda, hogy a near és far 
paraméterek is abban a nagyságrendben legyenek, mint a left, right, bottom és top.
Ha nagy az eltérés, akkor az objektumok ki fognak lógni a frustumból, és az OpenGL
nem rajzolja ki őket! (tehát ha pl így paraméterezel: 
`left=0, right=600, top=600, bottom=0`, akkor a near, far értékekre mondjuk 
`near=0, far=1000`-et állíts be)
* Amikor egy mátrix típusú uniform-ot állítasz be, figyelj oda **nagyon**, mert a 
framework-ben a mat4 `SetUniform` függvénye **transzponálja** a bemeneti mátrixot!
Ha ez nemkívánatos dolog, akkor kézzel kell meghívnod a `glUniformMatrix4fv` függvényt
* `glDrawArrays` harmadik paramétere **nem** a kirajzolt alakzatok száma, hanem a 
vertexek száma!
* Figyelj oda, hogy a vertex shader-ben amikor 4 elemű vektorrá alakítod a bemeneti
2, vagy 3 elemszámú vektort, a negyedik eleme **mindenképp `1.0` legyen**! Azaz
	* `vec2 vertex` esetén `vec4 (vertex.x, vertex.y, 0.0, 1.0)`
	* `vec3 vertex` esetén `vec4 (vertex, 1.0)`

## Kamera

OpenGL-ben a "kamera", mint fogalom tökre másképp értelmezendő, mint egy boltban 
kapható kamera. Ugyanis amikor a kamerát mozgatjuk, akkor igazából nem a kamera
mozog, hanem az egész világot mozgatjuk ellenkező irányba. A kamerának pedig 
az a szerepe, hogy egy "ablakot" adjon a virtuális világra. Két fontos mátrix-ot
kell egy kamerában nyilvántartani:

* Projekciós mátrix: Ez definiálja, hogy "mik történnek az egyenesekkel"
	* Ortogonális vetítés esetén a merőleges egyenesek merőlegesek maradnak
	* Perspektív vetítés esetén a merőleges egyenesek nem maradnak merőlegesek
* View mátrix: Ez határozza meg a kamera transzformációját
	* Hol van
	* Merre néz
	* Nagyítás

Az ortogonális és perspektív kamerák közti különbség az alábbi képeken látható:

![Perspektív projekció](perspective.png)

![Ortogonális projekció](orthographic.png)

A két kép **ugyanarról a jelenetről** készült, a különbség annyi, hogy az elsőben
perspektív, a másodikban ortogonális vetítést használ a kamera.

A kameráknál még van egy fogalom (aminek a magyar nevét nem tudom), a frustum.
A frustum az a 3D test, amiben a benne lévő objektumokat látjuk, és a belőle kívül
eső objektumokat nem. Szemléletesen a perspektív vetítés esetén ez a frustum egy
csonka gúla, ortogonális esetben egy egyszerű téglatest.

### Ortogonális vetítési mátrix

```cpp
/* Ez csinál egy egységmátrixot a referenciaként átvett mat4-ben */
inline void IdentityMatrix (mat4 &matrix) {
	
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix.m [i][j] = 0.0f;
		}
		matrix.m [i][i] = 1.0f;
	}
	
}

/* Ez megcsinálja az ortogonális vetítési mátrixot a referenciaként átvett mat4-ben */
inline void OrthographicProjection (
	float left, 
	float right, 
	float top, 
	float bottom,
	float n,
	float f,
	mat4 &matrix
) {
	
	IdentityMatrix (matrix);
	
	matrix.m [0][0] = 2.0f / (right - left);
	matrix.m [1][1] = 2.0f / (top - bottom);
	matrix.m [2][2] = -2.0f / (f - n);
	matrix.m [3][3] = 1.0f;
	
	matrix.m [0][3] = -1.0f * ( (right + left) / (right - left) );
	matrix.m [1][3] = -1.0f * ( (top + bottom) / (top - bottom) );
	matrix.m [2][3] = -1.0f * ( (f + n) / (f - n) );
	
}
```

* A `left` paraméterrel kontrolláljuk, hogy a "képernyő bal széle melyik X koordinátának felel meg"
* A `right` paraméterrel kontrolláljuk, hogy a "képernyő jobb széle melyik X koordinátának felel meg"
* A `top` paraméterrel kontrolláljuk, hogy a "képernyő teteje melyik Y koordinátának felel meg"
* A `bottom` paraméterrel kontrolláljuk, hogy a "képernyő alja melyik Y koordinátának felel meg"
* A `near`-nél közelebb lévő dolgok, vagy a `far`-nal messzebb lévő dolgok pedig kívül esnek a frustumon, ezeket nem fogjuk kirajzolni

Kvázi a 6 paraméterrel a frustum téglatest 3 oldalát adjuk meg.

