//! �R���X�^���g�o�b�t�@
cbuffer cbSceneParam : register( b0 )
{
	float4	vecViewPos		: packoffset( c0 );
	matrix	mtxView			: packoffset( c1 );
	matrix	mtxProj			: packoffset( c5 );
};

cbuffer cbMeshParam : register( b1 )
{
	matrix	mtxWorld		: packoffset(c0);
	float4	colRevise		: packoffset(c4);
	float4	CoordsRevise	: packoffset(c5);
};

cbuffer cbMaterialParam : register( b2 )
{
	float4	matDiffuse		: packoffset( c0 );
	float4	matAmbient		: packoffset( c1 );
	float4	matSpeculer		: packoffset( c2 );
	float4	matEmissive		: packoffset( c3 );
	float	matPower		: packoffset( c4 );
};

cbuffer cbLightParam : register( b3 )
{
	float3	litDirection	: packoffset( c0 );
	float4	litDiffuse		: packoffset( c1 );
	float4	litAmbient		: packoffset( c2 );
	float4	litSpeculer		: packoffset( c3 );
};

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register(s0);

//! ���_����
struct InputVS
{
	float4	pos			: POSITION;
	float3	normal		: NORMAL;
    float2	Tex			: TEXCOORD;
	float4	color		: COLOR0;
};
struct OutputVS
{
	float4	pos			: SV_POSITION;
	float2	Tex			: TEXCOORD0;
};

//�V�K�̃e�N�X�`���I�u�W�F�N�g�̍쐬
Texture2D txNormal : register( t1 );

//! ���_�V�F�[�_
OutputVS RenderVS( InputVS inVert )
{
	OutputVS	outVert;

	matrix	mtxVP = mul( mtxView, mtxProj );

	//���_�V�F�[�_�[�Ŗ@���}�b�v���Q�Ƃ��Ė@�������ɒ��_���W���ړ�������
	float4 Pos = inVert.pos;
	Pos.xyz += txNormal.SampleLevel(samLinear, inVert.Tex, 0).xyz * 0.05f;
	Pos = mul(Pos, mtxWorld);


	outVert.pos = mul( Pos , mtxVP );

	outVert.Tex = inVert.Tex;
	return outVert;
}

//! �s�N�Z���V�F�[�_
float4 RenderPS( OutputVS inPixel ) : SV_TARGET
{
	//�@�����e�N�X�`������擾����
	//�e�N�X�`���̐F�ł͂O�`�P�Ȃ̂Ł[�P�`�P�͈̔͂ɕύX����
	float4 n = normalize((2.0f * txNormal.Sample(samLinear, inPixel.Tex)) - 1.0f);

	//�n�[�t�����o�[�g���C�e�B���O
	float l = dot(n, -litDirection);
	l = l * 0.5f + 0.5f;
	l *= l;

	//���C�e�B���O���ʂ���F���v�Z����
	float3 diff = l * litDiffuse.xyz * matDiffuse.xyz;
	float4 LP = float4( saturate( diff ) , matDiffuse.w );
	return LP * txDiffuse.Sample(samLinear, inPixel.Tex);
}

technique11 TShader
{
	pass P0
	{		
		  SetVertexShader( CompileShader( vs_4_0, RenderVS() ) );
		  SetGeometryShader( NULL );
		  SetHullShader( NULL );
		  SetDomainShader( NULL );
		  SetPixelShader(  CompileShader( ps_4_0, RenderPS() ) );
		  SetComputeShader( NULL );
	}
}
