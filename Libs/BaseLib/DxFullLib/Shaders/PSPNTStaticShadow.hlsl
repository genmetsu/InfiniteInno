//--------------------------------------------------------------------------------------
// File: PSPVTStaticShadow.hlsl
//
//--------------------------------------------------------------------------------------

#include "INCStructs.hlsli"
#include "INCParameters.hlsli"

Texture2D<float4> g_texture : register(t0);
// �[�x�}�b�v
Texture2D g_DepthMap : register(t1);
SamplerState g_sampler : register(s0);
SamplerComparisonState g_SamplerDepthMap : register(s1);

float3 DplusS(float3 N, float3 L, float NdotL, float3 view);


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main(PSPNTInputShadow input) : SV_TARGET
{
	//�@�����C�e�B���O
	float3 lightdir = normalize(LightDir.xyz);
	float3 N1 = normalize(input.norm);
	float4 RetColor = (saturate(dot(N1, -lightdir)) * Diffuse) + Emissive;
	RetColor += input.specular;
	RetColor.a = Diffuse.a;
	if (Activeflags.x){
		//�e�N�X�`���ƃf�t�B�[�Y���烉�C�e�B���O���쐬
		RetColor = g_texture.Sample(g_sampler, input.tex) * RetColor;
	}
	RetColor = saturate(RetColor);

	//�e�̔Z��
	const float3 ambient = float3(0.7f, 0.7f, 0.7f);
	float3 N = normalize(input.norm);
	float3 L = normalize(input.lightRay);
	float NdotL = dot(N, L);

	float2 shadowTexCoords;
	shadowTexCoords.x = 0.5f + (input.lightSpacePos.x / input.lightSpacePos.w * 0.5f);
	shadowTexCoords.y = 0.5f - (input.lightSpacePos.y / input.lightSpacePos.w * 0.5f);
	float pixelDepth = input.lightSpacePos.z / input.lightSpacePos.w;

	float lighting = 1;

	if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
		(saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
		(pixelDepth > 0))
	{

		float margin = acos(saturate(NdotL));
		float epsilon = 0.0005 / margin;

		epsilon = clamp(epsilon, 0, 0.1);

		lighting = float(g_DepthMap.SampleCmpLevelZero(
			g_SamplerDepthMap,
			shadowTexCoords,
			pixelDepth + epsilon
			)
			);
		if (lighting == 0.f)
		{
			//�e�̒�
			return float4(RetColor.xyz * ambient, RetColor.w);
		}
		else if (lighting < 1.0f)
		{
			//�e�Ɖe�ȊO�̋��E��
			float3 light = lighting * (ambient + DplusS(N, L, NdotL, input.lightView));
			float3 shadow = (1.0f - lighting) * ambient;
			return float4(RetColor.xyz * (light + shadow), RetColor.w);
		}
	}
	return float4(RetColor.xyz * (ambient + DplusS(N, L, NdotL, input.lightView)), RetColor.w);
}


float3 DplusS(float3 N, float3 L, float NdotL, float3 view)
{
	const float3 Kdiffuse = float3(.5f, .5f, .4f);
	const float3 Kspecular = float3(.2f, .2f, .3f);
	const float exponent = 3.f;

	// Compute the diffuse coefficient.
	float diffuseConst = saturate(NdotL);

	// Compute the diffuse lighting value.
	float3 diffuse = Kdiffuse * diffuseConst;

	// Compute the specular highlight.
	float3 R = reflect(-L, N);
	float3 V = normalize(view);
	float3 RdotV = dot(R, V);
	float3 specular = Kspecular * pow(saturate(RdotV), exponent);

	return (diffuse + specular);
}
