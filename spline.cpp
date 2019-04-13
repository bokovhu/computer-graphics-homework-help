
struct Spline {
	
	// Source: https://en.wikipedia.org/wiki/Kochanek%E2%80%93Bartels_spline
	float tension = -0.5f;
	float bias = 0.0f;
	float continuity = 0.5f;
	
	int segmentsPerControlPoint = 50;
	unsigned int vao;
	unsigned int vbo;
	std::vector <vec2> controlPoints;
	std::vector <vec2> vertices;
	vec3 color;
	
	void Setup () {
		
		glGenVertexArrays(1, &this->vao);
		glBindVertexArray(this->vao);

		glGenBuffers(1, &this->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0, // attrib location
			2, // attrib element count, vec2 has 2 elements
			GL_FLOAT, // attrib element type, elements are of type float
			GL_FALSE, // please don't normalize OpenGL, thank you very much
			0, // no stride in the data, only using positions, tightly packed together
			0 // first element of the buffer is data already
		);
		
		glBindVertexArray (0);
		glBindBuffer (GL_ARRAY_BUFFER, 0);
		
	}

	float GetTangent (float x) {

		if (controlPoints.size () == 0) return 0.0f;
		
		vec2 p0 (controlPoints [0].x, controlPoints [0].y);
		vec2 p1 (controlPoints [0].x, controlPoints [0].y);
		vec2 p2 (controlPoints [0].x, controlPoints [0].y);
		vec2 p3 (controlPoints [0].x, controlPoints [0].y);
		bool foundControlPoints = false;
		
		for (int i = 0; i < controlPoints.size (); i++) {
			
			vec2 cp (controlPoints [i].x, controlPoints [i].y);
			if (cp.x < x) continue;
			
			if (i == 0) {
				
				p0.x = controlPoints [0].x;
				p0.y = controlPoints [0].y;
				
				p1.x = controlPoints [0].x;
				p1.y = controlPoints [0].y;
				
				p2.x = controlPoints [0].x;
				p2.y = controlPoints [0].y;
				
				p3.x = controlPoints [min (1, controlPoints.size ())].x;
				p3.y = controlPoints [min (1, controlPoints.size ())].y;
				
				foundControlPoints = true;
				break;
				
			} else if (i == 1) {
				
				p0.x = controlPoints [0].x;
				p0.y = controlPoints [0].y;
				
				p1.x = controlPoints [0].x;
				p1.y = controlPoints [0].y;
				
				p2.x = controlPoints [min (1, controlPoints.size ())].x;
				p2.y = controlPoints [min (1, controlPoints.size ())].y;
				
				p3.x = controlPoints [min (2, controlPoints.size ())].x;
				p3.y = controlPoints [min (2, controlPoints.size ())].y;
				
				foundControlPoints = true;
				break;
				
			} else {
				
				p0.x = controlPoints [i - 2].x;
				p0.y = controlPoints [i - 2].y;
				
				p1.x = controlPoints [i - 1].x;
				p1.y = controlPoints [i - 1].y;
				
				if (i == controlPoints.size () - 1) {
					
					p2.x = controlPoints [i].x;
					p2.y = controlPoints [i].y;
					
					p3.x = controlPoints [i].x;
					p3.y = controlPoints [i].y;
					
				} else {
					
					p2.x = controlPoints [i].x;
					p2.y = controlPoints [i].y;
					
					p3.x = controlPoints [i + 1].x;
					p3.y = controlPoints [i + 1].y;
					
				}
				
				foundControlPoints = true;
				break;
				
			}
			
		}
		
		if (!foundControlPoints) {
			return controlPoints [controlPoints.size () - 1].y;
		}
		
		// Calculate Kochanek-Bartels tangents
		vec2 d0;

		d0.x = ( (1.0f - tension) * (1.0f + bias) * (1.0f + continuity) ) / 2.0f * unzero ( p1.x - p0.x )
			+ ( (1.0f - tension) * (1.0f - bias) * (1.0f - continuity) ) / 2.0f * unzero (p2.x - p1.x);

		d0.y = ( (1.0f - tension) * (1.0f + bias) * (1.0f + continuity) ) / 2.0f * unzero ( p1.y - p0.y )
			+ ( (1.0f - tension) * (1.0f - bias) * (1.0f - continuity) ) / 2.0f * unzero (p2.y - p1.y);

		vec2 d1;

		d1.x = ( (1.0f - tension) * (1.0f + bias) * (1.0f - continuity) ) / 2.0f * unzero ( p2.x - p1.x )
			+ ( (1.0f - tension) * (1.0f - bias) * (1.0f + continuity) ) / 2.0f * unzero ( p3.x - p2.x );
		d1.y = ( (1.0f - tension) * (1.0f + bias) * (1.0f - continuity) ) / 2.0f * unzero ( p2.y - p1.y )
			+ ( (1.0f - tension) * (1.0f - bias) * (1.0f + continuity) ) / 2.0f * unzero ( p3.y - p2.y );
			
		// Alpha is the interpolation value between p1 and p2
		float t = (x - p1.x) / unzero (abs (p2.x - p1.x));

		vec2 tangentVector;

		tangentVector.x = (6.0f * t * t - 6.0f * t) * p1.x
			+ (3.0f * t * t - 4.0 * t + 1.0f) * d0.x
			+ (-6.0f * t * t + 6.0f * t) * p2.x
			+ (3.0f * t * t - 2.0f * t) * d1.x;

		tangentVector.y = (6.0f * t * t - 6.0f * t) * p1.y
			+ (3.0f * t * t - 4.0 * t + 1.0f) * d0.y
			+ (-6.0f * t * t + 6.0f * t) * p2.y
			+ (3.0f * t * t - 2.0f * t) * d1.y;

		return tangentVector.y / unzero (tangentVector.x);

	}

	float GetHeight (float x) {
		
		if (controlPoints.size () == 0) return 0.0f;
		
		vec2 p0 (controlPoints [0].x, controlPoints [0].y);
		vec2 p1 (controlPoints [0].x, controlPoints [0].y);
		vec2 p2 (controlPoints [0].x, controlPoints [0].y);
		vec2 p3 (controlPoints [0].x, controlPoints [0].y);
		bool foundControlPoints = false;
		
		for (int i = 0; i < controlPoints.size (); i++) {
			
			vec2 cp (controlPoints [i].x, controlPoints [i].y);
			if (cp.x < x) continue;
			
			if (i == 0) {
				
				p0.x = controlPoints [0].x;
				p0.y = controlPoints [0].y;
				
				p1.x = controlPoints [0].x;
				p1.y = controlPoints [0].y;
				
				p2.x = controlPoints [0].x;
				p2.y = controlPoints [0].y;
				
				p3.x = controlPoints [min (1, controlPoints.size ())].x;
				p3.y = controlPoints [min (1, controlPoints.size ())].y;
				
				foundControlPoints = true;
				break;
				
			} else if (i == 1) {
				
				p0.x = controlPoints [0].x;
				p0.y = controlPoints [0].y;
				
				p1.x = controlPoints [0].x;
				p1.y = controlPoints [0].y;
				
				p2.x = controlPoints [min (1, controlPoints.size ())].x;
				p2.y = controlPoints [min (1, controlPoints.size ())].y;
				
				p3.x = controlPoints [min (2, controlPoints.size ())].x;
				p3.y = controlPoints [min (2, controlPoints.size ())].y;
				
				foundControlPoints = true;
				break;
				
			} else {
				
				p0.x = controlPoints [i - 2].x;
				p0.y = controlPoints [i - 2].y;
				
				p1.x = controlPoints [i - 1].x;
				p1.y = controlPoints [i - 1].y;
				
				if (i == controlPoints.size () - 1) {
					
					p2.x = controlPoints [i].x;
					p2.y = controlPoints [i].y;
					
					p3.x = controlPoints [i].x;
					p3.y = controlPoints [i].y;
					
				} else {
					
					p2.x = controlPoints [i].x;
					p2.y = controlPoints [i].y;
					
					p3.x = controlPoints [i + 1].x;
					p3.y = controlPoints [i + 1].y;
					
				}
				
				foundControlPoints = true;
				break;
				
			}
			
		}
		
		if (!foundControlPoints) {
			return controlPoints [controlPoints.size () - 1].y;
		}
		
		// Calculate Kochanek-Bartels tangents
		float d0 = ( (1.0f - tension) * (1.0f + bias) * (1.0f + continuity) ) / 2.0f * unzero ( p1.y - p0.y )
			+ ( (1.0f - tension) * (1.0f - bias) * (1.0f - continuity) ) / 2.0f * unzero (p2.y - p1.y);
		float d1 = ( (1.0f - tension) * (1.0f + bias) * (1.0f - continuity) ) / 2.0f * unzero ( p2.y - p1.y )
			+ ( (1.0f - tension) * (1.0f - bias) * (1.0f + continuity) ) / 2.0f * unzero ( p3.y - p2.y );
			
		// Alpha is the interpolation value between p1 and p2
		float t = (x - p1.x) / unzero (abs (p2.x - p1.x));
		
		// Calculate the spline interpolation value
		return (2.0f * t * t * t - 3.0f * t * t + 1.0f) * p1.y
			+ (t * t * t - 2.0 * t * t + t) * d0 
			+ (-2.0f * t * t * t + 3.0f * t * t) * p2.y
			+ (t * t * t - t * t) * d1;
		
	}
	
	void RecalculateVertices () {
		
		vertices.clear ();
		
		if (controlPoints.size () >= 2) {
			
			float fromX = controlPoints [0].x;
			float toX = controlPoints [controlPoints.size () - 1].x;
			float step = 1.0f;
			
			float x = fromX;
			while (x < toX) {

				float y = GetHeight (x);
				vertices.push_back (vec2 (x, 0.0f));
				vertices.push_back (vec2 (x, y));

				x += step;
			}
			
		}
		
	}
	
	void UploadVertices () {
		
		glBindBuffer (GL_ARRAY_BUFFER, this->vbo);
		glBufferData (GL_ARRAY_BUFFER, sizeof (float) * vertices.size () * 2, &vertices [0], GL_DYNAMIC_DRAW);
		
	}
	
	void AddControlPoint (vec2 cp) {
		
		bool inserted = false;
		for (auto it = controlPoints.begin (); it != controlPoints.end (); it++) {
			
			auto p = *it;
			if (p.x > cp.x) {
				controlPoints.insert (it, cp);
				inserted = true;
				break;
			}
			
		}
		
		if (!inserted) {
			controlPoints.push_back (cp);
		}
		
		this->RecalculateVertices ();
		this->UploadVertices ();
		
	}
	
	void Draw () {
		
		int colorUniformLocation = glGetUniformLocation(gpuProgram.getId(), "u_color");
		glUniform3f (colorUniformLocation, color.x, color.y, color.z);
		
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, vertices.size ());
		glBindVertexArray (0);
		
	}
	
};
