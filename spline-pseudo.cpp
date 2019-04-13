float spline (float x) {
	// Spline számolása ezen az X-en a kontrol pontokból
}

void UploadVertices () {
	glBindBuffer (GL_ARRAY_BUFFER, splineVbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof (float) * vertices.size () * 2, &vertices [0], GL_DYNAMIC_DRAW);
}

void RecalculateVertices () {
	
	vertices.clear ();
	float xStart = minControlPoint ().x;
	float xEnd = maxControlPoint ().x;
	
	float xStep = 1.0f;
	
	float x = xStart;
	while (x != xEnd) {
		
		float y = spline (x);
		vertices.push_back (vec2 (x, y));
		x += xStep;
		
	}
	
	UploadVertices ();
	
}