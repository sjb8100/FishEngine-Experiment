#include <FishEngine/Render/Material.hpp>
#include <FishEngine/Render/Shader.hpp>

const char* error_shader_str = R"(
#ifdef VERTEX
	#define PositionIndex 0
	layout (location = PositionIndex)   in vec3 InputPositon;
	uniform mat4 MATRIX_MVP;

	void main()
	{
		gl_Position = MATRIX_MVP * vec4(InputPositon, 1);
	}
#endif

#ifdef FRAGMENT
	out vec4 fragColor;
	void main()
	{
		fragColor = vec4(1, 0, 1, 1);
	}
#endif
)";

const char* default_shader_str = R"(
layout(std140, row_major) uniform PerCameraUniforms
{
	mat4 MATRIX_P;			// CameraProjection;
	mat4 MATRIX_V;			// WorldToCamera;
	mat4 MATRIX_I_V;		// CameraToWorld;
	mat4 MATRIX_VP;			// CameraProjection * WorldToCamera;

	// Time (t = time since current level load) values from Unity
	vec4 Time; 	// (t/20, t, t*2, t*3)
	//vec4 _SinTime; // sin(t/8), sin(t/4), sin(t/2), sin(t)
	//vec4 _CosTime; // cos(t/8), cos(t/4), cos(t/2), cos(t)
	//vec4 unity_DeltaTime; // dt, 1/dt, smoothdt, 1/smoothdt

	// x = 1 or -1 (-1 if projection is flipped)
	// y = near plane
	// z = far plane
	// w = 1/far plane
	vec4 ProjectionParams;

	// x = width
	// y = height
	// z = 1 + 1.0/width
	// w = 1 + 1.0/height
	vec4 ScreenParams;

	// Values used to linearize the Z buffer (http://www.humus.name/temp/Linearize%20depth.txt)
	// x = 1-far/near
	// y = far/near
	// z = x/far
	// w = y/far
	vec4 ZBufferParams;

	// x = orthographic camera's width
	// y = orthographic camera's height
	// z = unused
	// w = 1.0 if camera is ortho, 0.0 if perspective
	//vec4 unity_OrthoParams;

	vec4 WorldSpaceCameraPos;		// .w = 1, not used
	vec4 WorldSpaceCameraDir;		// .w = 0, not used, forward direction of the camera in world space
};


layout(std140, row_major) uniform PerDrawUniforms
{
	mat4 MATRIX_MVP;
	mat4 MATRIX_MV;
	mat4 MATRIX_IT_MV;
	mat4 MATRIX_M;		// ObjectToWorld
	mat4 MATRIX_IT_M;	// WorldToObject
};

layout(std140, row_major) uniform LightingUniforms
{
	vec4 LightColor;
	vec4 WorldSpaceLightPos;

	vec4 CascadesNear;
	vec4 CascadesFar;
	vec4 CascadesSplitPlaneNear;
	vec4 CascadesSplitPlaneFar;
	vec4 _LightShadowData;
	vec4 unity_LightShadowBias;
	//mat4 LightMatrix; // World-to-light matrix. Used to sample cookie & attenuation textures.
	// macOS bug
	layout(column_major) mat4 LightMatrix[4]; // world -> clip (VP)
};

#ifdef VERTEX
	#define PositionIndex 0
	#define NormalIndex 1
	#define TangentIndex 2
	#define UVIndex 3
	#define BoneIndexIndex 4
	#define BoneWeightIndex 5
	layout(location = PositionIndex)   in vec3 InputPositon;
	layout(location = NormalIndex)     in vec3 InputNormal;
	layout(location = TangentIndex)    in vec3 InputTangent;
	layout(location = UVIndex)         in vec2 InputUV;
//	uniform mat4 MATRIX_MVP;
//	uniform mat4 MATRIX_M;

	out vec3 normal;

	void main()
	{
		gl_Position = MATRIX_MVP * vec4(InputPositon, 1);
		//normal = InputNormal*0.5+0.5;
		normal = mat3(MATRIX_M) * InputNormal;
		//normal = normalize(normal);
		//normal = normalize(InputNormal);
		//color = vec3(gl_Position.z);
	}
#endif

#ifdef FRAGMENT
	in vec3 normal;
	//const vec3 L = normalize(vec3(1, 1, 0));
//	uniform vec3 LightDir;
	out vec4 fragColor;
	void main()
	{
//		vec3 LightDir = normalize(WorldSpaceLightPos.xyz);
		vec3 LightDir = -WorldSpaceLightPos.xyz;
		vec3 N = normalize(normal);
		//fragColor = vec4(normal, 1);
		float ndotl = clamp(dot(N, -LightDir), 0, 1);
		fragColor = vec4(vec3(ndotl), 1);
		//fragColor = vec4(N*0.5+0.5, 1);
		//fragColor = vec4(vec3(gl_FragCoord.z*5), 1);
	}
#endif
)";
namespace FishEngine
{
	Material* Material::s_ErrorMaterial = nullptr;
	Material* Material::s_DefaultMaterial = nullptr;
	
	void Material::StaticInit()
	{
		s_ErrorMaterial = new Material();
		s_ErrorMaterial->SetName("Default-Error");
		s_ErrorMaterial->m_Shader = Shader::FromString(error_shader_str);

		s_DefaultMaterial = new Material();
		s_DefaultMaterial->SetName("Default-Material");
		s_DefaultMaterial->m_Shader = Shader::FromString(default_shader_str);
	}
	
	void Material::StaticClean()
	{
		// TODO: AssetDatabase;
//		delete s_ErrorMaterial->m_Shader;
//		delete s_ErrorMaterial;

//		delete s_DefaultMaterial->m_Shader;
//		delete s_DefaultMaterial;
	}
}
