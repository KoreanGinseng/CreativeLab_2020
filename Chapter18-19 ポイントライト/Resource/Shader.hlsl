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
	float3  wpos        : COLOR0;
};

//! 新規のコンスタントバッファ作成
cbuffer cbGameParam : register(b4)
{
	float4   cblgPosition    : packoffset(c0);
	float4   cblgDiffuse     : packoffset(c1);
	float4   cblgAttenuation : packoffset(c2);
};

//! 頂点シェーダ
OutputVS RenderVS( InputVS inVert )
{
	OutputVS	outVert;

	matrix	mtxVP  = mul( mtxView, mtxProj );
	float4 Pos     = float4(inVert.pos.xyz, 1.0);
	Pos            = mul(Pos, mtxWorld );
	outVert.pos    = mul( Pos , mtxVP );
	outVert.normal = normalize( mul( inVert.normal, (float3x3)mtxWorld ) );
	outVert.wpos   = Pos;

	return outVert;
}

//! ピクセルシェーダ
float4 RenderPS( OutputVS inPixel ) : SV_TARGET
{
	//法線
	float3 N = normalize(inPixel.normal);
	//ハーフランバートライティング
	float l = dot(N, -litDirection);
	l  = l * 0.5f + 0.5f;
	l *= l;
	float3 amb  = litAmbient.xyz * matAmbient.xyz;
	float3 diff = l * litDiffuse.xyz * matDiffuse.xyz;

	//ポイントライティング
	float3 lv = cblgPosition.xyz - inPixel.wpos.xyz;
	float d   = length(lv);
	lv        = normalize(lv);
	float pl  = dot(N, lv);
	float Att = 1.0f / (cblgAttenuation.x + cblgAttenuation.y * d + cblgAttenuation.z * d * d);
	pl       *= Att;
	diff     += clamp(pl, 0, 1)*cblgDiffuse.xyz * matDiffuse.xyz;
	
	float4 LP = float4(saturate(matEmissive.xyz + amb + diff), matDiffuse.w);
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
