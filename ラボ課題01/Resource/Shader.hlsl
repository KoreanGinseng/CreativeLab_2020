//! コンスタントバッファ
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

//! 頂点属性
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

//! 新規のコンスタントバッファ作成
cbuffer cbGameParam : register(b4)
{
	float4	cbTime			: packoffset(c0);
};

//新規のテクスチャオブジェクトの作成
Texture2D txImage1 : register( t1 );
Texture2D txImage2 : register( t2 );


//! 頂点シェーダ
OutputVS RenderVS( InputVS inVert )
{
	OutputVS	outVert;

	matrix	mtxVP = mul( mtxView, mtxProj );
	float4 Pos = mul( inVert.pos + float4(0, sin(cbTime.x + inVert.pos.x), 0, 1), mtxWorld );
	outVert.pos = mul( Pos , mtxVP );

	outVert.Tex = inVert.Tex;
	return outVert;
}

//! ピクセルシェーダ
float4 RenderPS( OutputVS inPixel ) : SV_TARGET
{
	//フレンド率（U位置 + sin(time))をそのままブレンド率とする）
	//float bld = clamp(inPixel.Tex.x + sin(cbTime.x), 0, 1);
	float bld = clamp(inPixel.Tex.x + sin(cbTime.x), 0, 1);
	//2枚のテクスチャを線形補間で色を求める
	return lerp(txImage1.Sample(samLinear, inPixel.Tex),
		txImage2.Sample(samLinear, inPixel.Tex), bld);
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
