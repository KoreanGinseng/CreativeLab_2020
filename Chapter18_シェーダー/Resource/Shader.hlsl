//! �R���X�^���g�o�b�t�@
cbuffer cbSceneParam : register(b0)
{
	float4 vecViewPos : packoffset(c0);
	matrix mtxView    : packoffset(c1);
	matrix mtxProj    : packoffset(c5);
};

cbuffer cbMeshParam : register(b1)
{
	matrix mtxWorld     : packoffset(c0);
	float4 colRevise    : packoffset(c4);
	float4 coordsRevise : packoffset(c5);
};

cbuffer cbMaterialParam : register(b2)
{
	float4 matDiffuse  : packoffset(c0);
	float4 matAmbient  : packoffset(c1);
	float4 matSpeculer : packoffset(c2);
	float4 matEmissive : packoffset(c3);
	float  matPower    : packoffset(c4);
};

cbuffer cbLightParam : register(b3)
{
	float3 litDirection : packoffset(c0);
	float4 litDiffuse   : packoffset(c1);
	float4 litAmbient   : packoffset(c2);
	float4 litSpeculer  : packoffset(c3);
};

Texture2D txDiffuse    : register(t0);
SamplerState samLinear : register(s0);

//! ���_����
struct InputVS
{
	float4 pos    : POSITION;
	float3 normal : NORMAL;
	float2 tex    : TEXCOORD;
	float4 color  : COLOR0;
};

struct OutputVS
{
	float4 pos    : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex    : TEXCOORD0;
};

//! ���_�V�F�[�_
OutputVS RenderVS(InputVS inVert)
{
	OutputVS outVert;

	matrix mtxVP   = mul(   mtxView,  mtxProj);
	float4 Pos     = mul(inVert.pos, mtxWorld);
	outVert.pos    = mul(       Pos,    mtxVP);
	outVert.normal = normalize(mul(inVert.normal, (float3x3)mtxWorld));

	outVert.tex    = inVert.tex;

	return outVert;
}

//! �s�N�Z���V�F�[�_
float4 RenderPS(OutputVS inPixel) : SV_TARGET
{
	float3 amb  = litAmbient.xyz * matAmbient.xyz;

	// ���C�g�̉e���x���v�Z(0�ȏ�)
	float l     = max(0.0f, dot(inPixel.normal, -litDirection));
	
	// ���C�g�e���~���C�g�f�B�q���[�Y�F�~�}�e���A���f�B�q���[�Y�F
	float3 diff = l * litDiffuse.xyz * matDiffuse.xyz;
	
	// �ŏI�I�ȐF�Ƃ��āA�G�~�b�V�u�{�����{�g�U���ˌ��̒l�� 0 ~ 1 �͈̔͂̂Ȃ�悤�v�Z�B
	float4 LP   = float4(saturate(matEmissive.xyz + amb + diff), matDiffuse.w);
	
	return LP * txDiffuse.Sample(samLinear, inPixel.tex); //RGBA �ŐF���w��
}

technique11 TShader
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, RenderVS()));
		SetGeometryShader(NULL);
		SetHullShader(NULL);
		SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, RenderPS()));
		SetComputeShader(NULL);
	}
};
