#pragma once

#define TOTAL_OBJECTS_IN_SCENE 10
struct Ray
{
	glm::vec3 RayOrigin;
	glm::vec3 RayDirection;

	Ray()
	{
		RayOrigin = glm::vec3(0.0f);
		RayDirection = glm::vec3(0.0f);
	}

};

struct BoundingBox
{
	glm::vec3 min_Bounding;
	glm::vec3 max_Bounding;

	BoundingBox()
	{
		min_Bounding = glm::vec3(0.0f);
		max_Bounding = glm::vec3(0.0f);
	}
};

GLuint gVao_bb, gVbo_bb_position, gVao_bbb_normal;

//create Bounding Box
//AABB => Axis aligned bounding box
void calculateBoundingVolumes(std::vector<float> vertices, BoundingBox *OOBBox)
{
	glm::vec3 min = glm::vec3(0.0f);
	glm::vec3 max = glm::vec3(0.0f);

	for (int i = 0; i < vertices.size(); i = i + 3)
	{
		// Expand the box as necessary to contain the point.
		if (vertices[i] < min.x) min.x = vertices[i];
		if (vertices[i] > max.x) max.x = vertices[i];
		if (vertices[i + 1] < min.y) min.y = vertices[i + 1];
		if (vertices[i + 1] > max.y) max.y = vertices[i + 1];
		if (vertices[i + 2] < min.z) min.z = vertices[i + 2];
		if (vertices[i + 2] > max.z) max.z = vertices[i + 2];

	}

	(OOBBox->min_Bounding) = min;
	(OOBBox->max_Bounding) = max;
}

void calculateBoundingVolumes(const float vertices[], int length, BoundingBox* OOBBox)
{
	glm::vec3 min = glm::vec3(0.0f);
	glm::vec3 max = glm::vec3(0.0f);

	for (int i = 0; i < length / sizeof(float); i = i + 3)
	{
		// Expand the box as necessary to contain the point.
		if (vertices[i] < min.x) min.x = vertices[i];
		if (vertices[i] > max.x) max.x = vertices[i];
		if (vertices[i + 1] < min.y) min.y = vertices[i + 1];
		if (vertices[i + 1] > max.y) max.y = vertices[i + 1];
		if (vertices[i + 2] < min.z) min.z = vertices[i + 2];
		if (vertices[i + 2] > max.z) max.z = vertices[i + 2];

	}

	(OOBBox->min_Bounding) = min;
	(OOBBox->max_Bounding) = max;
}
void updateBoundingVolumes(glm::mat4 modelMatrix, BoundingBox OOBBox, BoundingBox* AABBox)
{
	glm::mat4 model = modelMatrix;
	glm::mat4 absModel(glm::abs(model[0][0]), glm::abs(model[0][1]), glm::abs(model[0][2]), glm::abs(model[0][3]),
		glm::abs(model[1][0]), glm::abs(model[1][1]), glm::abs(model[1][2]), glm::abs(model[1][3]),
		glm::abs(model[2][0]), glm::abs(model[2][1]), glm::abs(model[2][2]), glm::abs(model[2][3]),
		glm::abs(model[3][0]), glm::abs(model[3][1]), glm::abs(model[3][2]), glm::abs(model[3][3]));

	glm::vec3 center = (OOBBox.min_Bounding+ OOBBox.max_Bounding) / 2.0f;
	glm::vec3 extent = (OOBBox.max_Bounding - OOBBox.min_Bounding) / 2.0f;

	glm::vec3 newCenter = glm::vec3(model * glm::vec4(center, 1.0f));
	glm::vec3 newExtent = glm::vec3(absModel * glm::vec4(extent, 0.0f));

	glm::vec3 min = newCenter - newExtent;
	glm::vec3 max = newCenter + newExtent;

	AABBox->min_Bounding = min;
	AABBox->max_Bounding = max;

}

void renderBoundingBox(BoundingBox bBox)
{
	GLfloat boxFaces[] = {
		/* First face */
		bBox.min_Bounding.x, bBox.min_Bounding.y, bBox.min_Bounding.z, bBox.max_Bounding.x, bBox.min_Bounding.y, bBox.min_Bounding.z, bBox.min_Bounding.x, bBox.min_Bounding.y,
		bBox.min_Bounding.z, bBox.min_Bounding.x, bBox.max_Bounding.y, bBox.min_Bounding.z, bBox.min_Bounding.x, bBox.min_Bounding.y, bBox.min_Bounding.z, bBox.min_Bounding.x,
		bBox.min_Bounding.y, bBox.max_Bounding.z,
		/*--*/
		bBox.max_Bounding.x, bBox.max_Bounding.y, bBox.max_Bounding.z, bBox.min_Bounding.x, bBox.max_Bounding.y, bBox.max_Bounding.z, bBox.max_Bounding.x, bBox.max_Bounding.y,
		bBox.max_Bounding.z, bBox.max_Bounding.x, bBox.min_Bounding.y, bBox.max_Bounding.z, bBox.max_Bounding.x, bBox.max_Bounding.y, bBox.max_Bounding.z, bBox.max_Bounding.x,
		bBox.max_Bounding.y, bBox.min_Bounding.z,
		/*--*/
		bBox.min_Bounding.x, bBox.max_Bounding.y, bBox.max_Bounding.z, bBox.min_Bounding.x, bBox.max_Bounding.y,bBox.min_Bounding.z, bBox.min_Bounding.x, bBox.max_Bounding.y,
		bBox.max_Bounding.z, bBox.min_Bounding.x, bBox.min_Bounding.y, bBox.max_Bounding.z,
		/*--*/
		bBox.max_Bounding.x, bBox.min_Bounding.y, bBox.max_Bounding.z, bBox.max_Bounding.x, bBox.min_Bounding.y, bBox.min_Bounding.z, bBox.max_Bounding.x, bBox.min_Bounding.y,
		bBox.max_Bounding.z, bBox.min_Bounding.x, bBox.min_Bounding.y, bBox.max_Bounding.z,
		/*--*/
		bBox.max_Bounding.x, bBox.max_Bounding.y, bBox.min_Bounding.z, bBox.min_Bounding.x, bBox.max_Bounding.y, bBox.min_Bounding.z, bBox.max_Bounding.x, bBox.max_Bounding.y,
		bBox.min_Bounding.z, bBox.max_Bounding.x, bBox.min_Bounding.y, bBox.min_Bounding.z,
	};

	
	glGenVertexArrays(1, &gVao_bb);
	glBindVertexArray(gVao_bb);

	//position
	glGenBuffers(1, &gVbo_bb_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_bb_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxFaces), boxFaces, GL_STATIC_DRAW);

	glVertexAttribPointer(MALATI_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(MALATI_ATTRIBUTE_VERTEX);
	
	fprintf(gpFile, "sizeof(boxFaces) = %d", sizeof(boxFaces));
	fflush(gpFile);
	glDrawArrays(GL_LINES, 0, sizeof(boxFaces) / (2 * sizeof (* boxFaces)));

	glBindVertexArray(0);
}

#define RIGHT_MOUSE	0
#define LEFT_MOUSE	1
#define MIDDLE_MOUSE	2

bool HitBoundingBox(Ray ray, BoundingBox bBox)
{
	glm::vec3 coord;
	bool inside = true;
	double maxT[3];
	glm::vec3 quadrant = glm::vec3(0.0); //x, y, z
	glm::vec3 candidatePlane;
	int whichPlane;

	/* Find candidate planes; this loop can be avoided if
	rays cast all from the eye(assume perpsective view) */
	for (int i = 0; i < 3; i++)
	{
		if (ray.RayOrigin[i] < bBox.min_Bounding[i])
		{
			quadrant[i] = LEFT_MOUSE;
			candidatePlane[i] = bBox.min_Bounding[i];
			inside = FALSE;
		}
		else if (ray.RayOrigin[i] > bBox.max_Bounding[i])
		{
			quadrant[i] = RIGHT_MOUSE;
			candidatePlane[i] = bBox.max_Bounding[i];
			inside = FALSE;
		}
		else
		{
			quadrant[i] = MIDDLE_MOUSE;
		}
	}

	/* Ray origin inside bounding box */
	if (inside) {
		coord = ray.RayOrigin;
		return (TRUE);
	}

	/* Calculate T distances to candidate planes */
	for (int i = 0; i < 3; i++)
		if (quadrant[i] != MIDDLE_MOUSE && ray.RayDirection[i] != 0.0)
			maxT[i] = (candidatePlane[i] - ray.RayOrigin[i]) / ray.RayDirection[i];
		else
			maxT[i] = -1.;

	/* Get largest of the maxT's for final choice of intersection */
	whichPlane = 0;
	for (int i = 1; i < 3; i++)
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;

	/* Check final candidate actually inside box */
	if (maxT[whichPlane] < 0.0) return (FALSE);
	for (int i = 0; i < 3; i++)
		if (whichPlane != i) {
			coord[i] = ray.RayOrigin[i] + maxT[whichPlane] * ray.RayDirection[i];
			if (coord[i] < bBox.min_Bounding[i] || coord[i] > bBox.max_Bounding[i])
				return (FALSE);
		}
		else {
			coord[i] = candidatePlane[i];
		}
	return (TRUE);/* ray hits box */
}


///////////////////////////////////////////////////////////////////////////////////
glm::vec2 getNormalisedDeviceCoordinates(float mouseX, float mouseY)
{
	//ndc conversion
	float x = (2.0f * mouseX) / gWidth - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / gHeight;

	return (glm::vec2(x, y));
}

glm::vec4 toEyeCoords(glm::vec4 clipCoords)
{
	glm::mat4 invertedProjection = glm::inverse(gPerspectiveProjectionMatrix);
	glm::vec4 eyeCoords = invertedProjection * clipCoords;
	return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

glm::vec3 toWorldCoords(glm::vec4 eyeCoords)
{
	glm::mat4 viewM = scene_camera.GetViewMatrix();
	glm::mat4 invertedView = glm::inverse(viewM);

	glm::vec4 rayWorld = invertedView * eyeCoords;
	glm::vec3 mouseRay = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);
	mouseRay = glm::normalize(mouseRay);
	return mouseRay;
}

Ray computeRay(float x, float y)
{
	//viewport space
	float mouseX = x;
	float mouseY = y;
	glm::vec3 rayOrig = glm::vec3(0.0);
	glm::vec2 normalizedCoords = getNormalisedDeviceCoordinates(mouseX, mouseY);
	glm::vec4 clipCoords = glm::vec4(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);
	glm::vec4 eyeCoords = toEyeCoords(clipCoords);
	glm::vec3 worldRay = toWorldCoords(eyeCoords);
	//*rayOrigin = cam_Pos;

	Ray ray;
	ray.RayOrigin = scene_camera.Position;
	ray.RayDirection = worldRay;
	return (ray);
}