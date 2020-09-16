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
SamplerState samLinear : register( s0 );

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
	float3	normal		: NORMAL;
	float3	wpos		: COLOR0;
	float4	VPos		: COLOR1;
};

//! 新規のコンスタントバッファ作成
cbuffer cbGameParam : register(b4)
{
	float4 cbProjection : packoffset(c0);
};

//! 頂点シェーダ
OutputVS RenderVS( InputVS inVert )
{
	OutputVS	outVert;

	matrix	mtxVP = mul(mtxView, mtxProj);
	float4 Pos = float4(inVert.pos.xyz, 1.0);
	Pos = mul(Pos, mtxWorld);
	outVert.pos = mul(Pos, mtxVP);
	outVert.normal = normalize(mul(inVert.normal, (float3x3)mtxWorld));
	outVert.wpos = Pos;

	//深度算出用にビュー変換のみをおこなった座標を出力
	outVert.VPos = mul(Pos, mtxVP);
	return outVert;
}

//! ピクセルシェーダ
// マルチレンダーターゲット（出力先を構造体でまとめて出力）
struct OutputPS
{
	float4 col : SV_TARGET0;
	float4 pos : SV_TARGET1;
	float4 norml : SV_TARGET2;
	float4 depth : SV_TARGET3;
};
OutputPS RenderPS( OutputVS inPixel )
{
	OutputPS po;
	po.col = matDiffuse;
	po.pos = float4(inPixel.wpos.xyz, 1);
	po.norml = float4(inPixel.normal.xyz, 1);
	po.depth = float4(inPixel.VPos.z / cbProjection.x, 0.0f, 0.0f, 1.0f);
	return po;
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
