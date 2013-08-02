//this file is autogenerated using stringify.bat (premake --stringify) in the build folder of this project
static const char* rayCastKernelCL= \
"#define SHAPE_CONVEX_HULL 3\n"
"#define SHAPE_PLANE 4\n"
"#define SHAPE_CONCAVE_TRIMESH 5\n"
"#define SHAPE_COMPOUND_OF_CONVEX_HULLS 6\n"
"#define SHAPE_SPHERE 7\n"
"\n"
"\n"
"typedef struct\n"
"{\n"
"	float4 m_from;\n"
"	float4 m_to;\n"
"} b3RayInfo;\n"
"\n"
"typedef struct\n"
"{\n"
"	float m_hitFraction;\n"
"	int	m_hitResult0;\n"
"	int	m_hitResult1;\n"
"	int	m_hitResult2;\n"
"	float4	m_hitPoint;\n"
"	float4	m_hitNormal;\n"
"} b3RayHit;\n"
"\n"
"typedef struct\n"
"{\n"
"	float4 m_pos;\n"
"	float4 m_quat;\n"
"	float4 m_linVel;\n"
"	float4 m_angVel;\n"
"\n"
"	unsigned int m_collidableIdx;\n"
"	float m_invMass;\n"
"	float m_restituitionCoeff;\n"
"	float m_frictionCoeff;\n"
"} Body;\n"
"\n"
"typedef struct Collidable\n"
"{\n"
"	union {\n"
"		int m_numChildShapes;\n"
"		int m_bvhIndex;\n"
"	};\n"
"	float m_radius;\n"
"	int m_shapeType;\n"
"	int m_shapeIndex;\n"
"} Collidable;\n"
"\n"
"\n"
"typedef struct  \n"
"{\n"
"	float4		m_localCenter;\n"
"	float4		m_extents;\n"
"	float4		mC;\n"
"	float4		mE;\n"
"\n"
"	float			m_radius;\n"
"	int	m_faceOffset;\n"
"	int m_numFaces;\n"
"	int	m_numVertices;\n"
"\n"
"	int m_vertexOffset;\n"
"	int	m_uniqueEdgesOffset;\n"
"	int	m_numUniqueEdges;\n"
"	int m_unused;\n"
"\n"
"} ConvexPolyhedronCL;\n"
"\n"
"typedef struct\n"
"{\n"
"	float4 m_plane;\n"
"	int m_indexOffset;\n"
"	int m_numIndices;\n"
"} b3GpuFace;\n"
"\n"
"\n"
"\n"
"///////////////////////////////////////\n"
"//	Quaternion\n"
"///////////////////////////////////////\n"
"\n"
"typedef float4 Quaternion;\n"
"\n"
"__inline\n"
"	Quaternion qtMul(Quaternion a, Quaternion b);\n"
"\n"
"__inline\n"
"	Quaternion qtNormalize(Quaternion in);\n"
"\n"
"\n"
"__inline\n"
"	Quaternion qtInvert(Quaternion q);\n"
"\n"
"\n"
"__inline\n"
"	float dot3F4(float4 a, float4 b)\n"
"{\n"
"	float4 a1 = (float4)(a.xyz,0.f);\n"
"	float4 b1 = (float4)(b.xyz,0.f);\n"
"	return dot(a1, b1);\n"
"}\n"
"\n"
"\n"
"__inline\n"
"	Quaternion qtMul(Quaternion a, Quaternion b)\n"
"{\n"
"	Quaternion ans;\n"
"	ans = cross( a, b );\n"
"	ans += a.w*b+b.w*a;\n"
"	//	ans.w = a.w*b.w - (a.x*b.x+a.y*b.y+a.z*b.z);\n"
"	ans.w = a.w*b.w - dot3F4(a, b);\n"
"	return ans;\n"
"}\n"
"\n"
"__inline\n"
"	Quaternion qtNormalize(Quaternion in)\n"
"{\n"
"	return fast_normalize(in);\n"
"	//	in /= length( in );\n"
"	//	return in;\n"
"}\n"
"__inline\n"
"	float4 qtRotate(Quaternion q, float4 vec)\n"
"{\n"
"	Quaternion qInv = qtInvert( q );\n"
"	float4 vcpy = vec;\n"
"	vcpy.w = 0.f;\n"
"	float4 out = qtMul(q,vcpy);\n"
"	out = qtMul(out,qInv);\n"
"	return out;\n"
"}\n"
"\n"
"__inline\n"
"	Quaternion qtInvert(Quaternion q)\n"
"{\n"
"	return (Quaternion)(-q.xyz, q.w);\n"
"}\n"
"\n"
"__inline\n"
"	float4 qtInvRotate(const Quaternion q, float4 vec)\n"
"{\n"
"	return qtRotate( qtInvert( q ), vec );\n"
"}\n"
"\n"
"\n"
"\n"
"void	trInverse(float4 translationIn, Quaternion orientationIn,\n"
"	float4* translationOut, Quaternion* orientationOut)\n"
"{\n"
"	*orientationOut = qtInvert(orientationIn);\n"
"	*translationOut = qtRotate(*orientationOut, -translationIn);\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"bool rayConvex(float4 rayFromLocal, float4 rayToLocal, int numFaces, int faceOffset,\n"
"	__global const b3GpuFace* faces, float* hitFraction, float4* hitNormal)\n"
"{\n"
"	rayFromLocal.w = 0.f;\n"
"	rayToLocal.w = 0.f;\n"
"	bool result = true;\n"
"\n"
"	float exitFraction = hitFraction[0];\n"
"	float enterFraction = -0.3f;\n"
"	float4 curHitNormal = (float4)(0,0,0,0);\n"
"	for (int i=0;i<numFaces && result;i++)\n"
"	{\n"
"		b3GpuFace face = faces[faceOffset+i];\n"
"		float fromPlaneDist = dot(rayFromLocal,face.m_plane)+face.m_plane.w;\n"
"		float toPlaneDist = dot(rayToLocal,face.m_plane)+face.m_plane.w;\n"
"		if (fromPlaneDist<0.f)\n"
"		{\n"
"			if (toPlaneDist >= 0.f)\n"
"			{\n"
"				float fraction = fromPlaneDist / (fromPlaneDist-toPlaneDist);\n"
"				if (exitFraction>fraction)\n"
"				{\n"
"					exitFraction = fraction;\n"
"				}\n"
"			} 			\n"
"		} else\n"
"		{\n"
"			if (toPlaneDist<0.f)\n"
"			{\n"
"				float fraction = fromPlaneDist / (fromPlaneDist-toPlaneDist);\n"
"				if (enterFraction <= fraction)\n"
"				{\n"
"					enterFraction = fraction;\n"
"					curHitNormal = face.m_plane;\n"
"					curHitNormal.w = 0.f;\n"
"				}\n"
"			} else\n"
"			{\n"
"				result = false;\n"
"			}\n"
"		}\n"
"		if (exitFraction <= enterFraction)\n"
"			result = false;\n"
"	}\n"
"\n"
"	if (enterFraction < 0.f)\n"
"	{\n"
"		result = false;\n"
"	}\n"
"\n"
"	if (result)\n"
"	{	\n"
"		hitFraction[0] = enterFraction;\n"
"		hitNormal[0] = curHitNormal;\n"
"	}\n"
"	return result;\n"
"}\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"bool sphere_intersect(float4 spherePos,  float radius, float4 rayFrom, float4 rayTo, float* hitFraction)\n"
"{\n"
"	float4 rs = rayFrom - spherePos;\n"
"	rs.w = 0.f;\n"
"	float4 rayDir = rayTo-rayFrom;\n"
"	rayDir.w = 0.f;\n"
"	float A = dot(rayDir,rayDir);\n"
"	float B = dot(rs, rayDir);\n"
"	float C = dot(rs, rs) - (radius * radius);\n"
"\n"
"	float D = B * B - A*C;\n"
"\n"
"	if (D > 0.0f)\n"
"	{\n"
"		float t = (-B - sqrt(D))/A;\n"
"\n"
"		if ( (t >= 0.0f) && (t < (*hitFraction)) )\n"
"		{\n"
"			*hitFraction = t;\n"
"			return true;\n"
"		}\n"
"	}\n"
"	return false;\n"
"}\n"
"\n"
"float4 setInterpolate3(float4 from, float4 to, float t)\n"
"{\n"
"	float s = 1.0f - t;\n"
"	float4 result;\n"
"	result = s * from + t * to;\n"
"	result.w = 0.f;	\n"
"	return result;	\n"
"}\n"
"\n"
"__kernel void rayCastKernel(  \n"
"	int numRays, \n"
"	const __global b3RayInfo* rays, \n"
"	__global b3RayHit* hitResults, \n"
"	const int numBodies, \n"
"	__global Body* bodies,\n"
"	__global Collidable* collidables,\n"
"	__global const b3GpuFace* faces,\n"
"	__global const ConvexPolyhedronCL* convexShapes	)\n"
"{\n"
"\n"
"	int i = get_global_id(0);\n"
"	if (i>=numRays)\n"
"		return;\n"
"\n"
"	hitResults[i].m_hitFraction = 1.f;\n"
"\n"
"	float4 rayFrom = rays[i].m_from;\n"
"	float4 rayTo = rays[i].m_to;\n"
"	float hitFraction = 1.f;\n"
"	float4 hitPoint;\n"
"	float4 hitNormal;\n"
"	int hitBodyIndex= -1;\n"
"\n"
"	int cachedCollidableIndex = -1;\n"
"	Collidable cachedCollidable;\n"
"\n"
"	for (int b=0;b<numBodies;b++)\n"
"	{\n"
"		if (hitResults[i].m_hitResult2==b)\n"
"			continue;\n"
"		Body body = bodies[b];\n"
"		float4 pos = body.m_pos;\n"
"		float4 orn = body.m_quat;\n"
"		if (cachedCollidableIndex != body.m_collidableIdx)\n"
"		{\n"
"			cachedCollidableIndex = body.m_collidableIdx;\n"
"			cachedCollidable = collidables[cachedCollidableIndex];\n"
"		}\n"
"		if (cachedCollidable.m_shapeType == SHAPE_CONVEX_HULL)\n"
"		{\n"
"\n"
"			float4 invPos = (float4)(0,0,0,0);\n"
"			float4 invOrn = (float4)(0,0,0,0);\n"
"			float4 rayFromLocal = (float4)(0,0,0,0);\n"
"			float4 rayToLocal = (float4)(0,0,0,0);\n"
"			invOrn = qtInvert(orn);\n"
"			invPos = qtRotate(invOrn, -pos);\n"
"			rayFromLocal = qtRotate( invOrn, rayFrom ) + invPos;\n"
"			rayToLocal = qtRotate( invOrn, rayTo) + invPos;\n"
"			rayFromLocal.w = 0.f;\n"
"			rayToLocal.w = 0.f;\n"
"			int numFaces = convexShapes[cachedCollidable.m_shapeIndex].m_numFaces;\n"
"			int faceOffset = convexShapes[cachedCollidable.m_shapeIndex].m_faceOffset;\n"
"			if (numFaces)\n"
"			{\n"
"				if (rayConvex(rayFromLocal, rayToLocal, numFaces, faceOffset,faces, &hitFraction, &hitNormal))\n"
"				{\n"
"					hitBodyIndex = b;\n"
"					\n"
"				}\n"
"			}\n"
"		}\n"
"		if (cachedCollidable.m_shapeType == SHAPE_SPHERE)\n"
"		{\n"
"			float radius = cachedCollidable.m_radius;\n"
"		\n"
"			if (sphere_intersect(pos,  radius, rayFrom, rayTo, &hitFraction))\n"
"			{\n"
"				hitBodyIndex = b;\n"
"				hitNormal = (float4) (hitPoint-bodies[b].m_pos);\n"
"			}\n"
"		}\n"
"	}\n"
"\n"
"	if (hitBodyIndex>=0)\n"
"	{\n"
"		hitPoint = setInterpolate3(rayFrom, rayTo,hitFraction);\n"
"		hitResults[i].m_hitFraction = hitFraction;\n"
"		hitResults[i].m_hitPoint = hitPoint;\n"
"		hitResults[i].m_hitNormal = normalize(hitNormal);\n"
"		hitResults[i].m_hitResult0 = hitBodyIndex;\n"
"	}\n"
"\n"
"}\n"
;
