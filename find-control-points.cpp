struct FindControlPointsResult {
	vec2 p0;
	vec2 p1;
	vec2 p2;
	vec2 p3;
	bool found;
};


FindControlPointsResult FindControlPoints (float x) {

	vec2 p0 = controlPoints [0];
	vec2 p1 = controlPoints [0];
	vec2 p2 = controlPoints [0];
	vec2 p3 = controlPoints [0];
	bool foundControlPoints = false;
	
	for (int i = 0; i < controlPoints.size (); i++) {
		
		vec2 cp (controlPoints [i].x, controlPoints [i].y);
		if (cp.x < x) continue;
		
		if (i == 0) {
			
			p0 = controlPoints [0];
			p1 = controlPoints [0];
			p2 = controlPoints [0];
			
			p3 = controlPoints [min (1, controlPoints.size () - 1)];
			
			foundControlPoints = true;
			break;
			
		} else if (i == 1) {
			
			p0 = controlPoints [0];
			p1 = controlPoints [0];
			p2 = controlPoints [min (1, controlPoints.size ())];
			p3 = controlPoints [min (2, controlPoints.size ())];

			foundControlPoints = true;
			break;
			
		} else {
			
			p0 = controlPoints [i - 2];
			p1 = controlPoints [i - 1];
			p2 = controlPoints [i];
			
			if (i == controlPoints.size () - 1) {
				
				p3 = controlPoints [i];
				
			} else {
				
				p3 = controlPoints [i + 1];
				
			}
			
			foundControlPoints = true;
			break;
			
		}
		
	}

	return { p0, p1, p2, p3, foundControlPoints };

}