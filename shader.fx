cbuffer cbPerObject
{
	float4x4 WVP;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_INPUT
{
	float4 inPos : POSITION;
	float2 inTexCoord : TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	output.Pos = mul(input.inPos, WVP);
	output.TexCoord = input.inTexCoord;

	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	return ObjTexture.Sample(ObjSamplerState, input.TexCoord);
}