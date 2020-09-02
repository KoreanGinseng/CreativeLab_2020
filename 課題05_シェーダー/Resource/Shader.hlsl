// �T���v���`��

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
SamplerState samLinear : register( s0 );

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
	float3	normal		: NORMAL;

	//�ۑ�R�������C�e�B���O�ɕK�v�ȏ�񁦓��͍ς�
	float3	eye			: TEXCOORD0;
};

//�ۑ�Q�������C�e�B���O�ɕK�v�ȏ�񁦓��͍ς�
float3				gSkyColor = {	0.2f,		0.2f,		0.3f};			//��̐F
float3				gGroundColor = {0.2f,		0.3f,		0.2f};			//�n�ʂ̐F
float3				gGroundAxis = {	0.0f,		1.0f,		0.0f};			//�n�ʂ̎�

//! ���_�V�F�[�_
OutputVS RenderVS( InputVS inVert )
{
	OutputVS	outVert;

	matrix	mtxVP = mul( mtxView, mtxProj );
	float4 Pos = mul( inVert.pos, mtxWorld );
	outVert.pos = mul( Pos , mtxVP );
	outVert.normal = normalize( mul( inVert.normal, (float3x3)mtxWorld ) );

	//�ۑ�R�������C�e�B���O�ɕK�v�ȏ�񁦓��͍ς�
	outVert.eye = normalize(vecViewPos.xyz - Pos.xyz);
	return outVert;
}

//! �s�N�Z���V�F�[�_
float4 RenderPS( OutputVS inPixel ) : SV_TARGET
{
	//�@��
	float3 N = normalize( inPixel.normal );
	//����
	float3 E = normalize( inPixel.eye );

	//------------------------------------------------------
	//�ۑ�Q�������C�e�B���O
	//------------------------------------------------------
	float3 amb = matAmbient.xyz * litAmbient.xyz;
	float S = dot(N, gGroundAxis) * 0.5f + 0.5f;
	amb += mul(gSkyColor, S) + mul(gGroundColor, (1 - S));

	//------------------------------------------------------
	//�ۑ�P�n�[�t�����o�[�g���C�e�B���O
	//------------------------------------------------------
	float l = dot(N, -litDirection);
	l = pow(l * 0.5f + 0.5f, 2);


	//------------------------------------------------------
	//�ۑ�R�������C�e�B���O
	//------------------------------------------------------
	float rim = 1 - max(dot(N,             E), 0);
	float le  =     max(dot(E, -litDirection), 0);
	rim = pow(rim, 5);

	//�ŏI�I�ȐF�v�Z
	float3 diff = l * litDiffuse.xyz * matDiffuse.xyz;
	float4 LP = float4( saturate( matEmissive.xyz + amb + diff ) , matDiffuse.w );

	LP.xyz += litDiffuse.xyz * rim * le;
	return LP;
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
