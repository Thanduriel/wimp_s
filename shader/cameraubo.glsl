layout(std140) uniform Camera
{
	vec4 c_vProjection;
	vec4 c_vInverseProjection;
	vec4 c_vNearPlaneSize;
	mat4x4 c_mCameraRotation;
};