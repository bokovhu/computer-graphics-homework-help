# Grafika checklist

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