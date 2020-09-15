/*************************************************************************//*!
					
					@file	GameApp.cpp
					@brief	基本ゲームアプリ。

															@author	濱田　享
															@date	2014.05.14
*//**************************************************************************/

//INCLUDE
#include	"GameApp.h"

//カメラ
CCamera						gCamera;
//ライト
CDirectionalLight			gLight;
//メッシュ
LPGeometry					pGeometry;
//テクスチャ
CTexture					gTexture;
//描画ターゲット
CTexture                    gTarget;

//シェーダー
CShader						gShader;
CShaderBind_3DPrimitiveBase	gShaderBind;

/*************************************************************************//*!
		@brief			アプリケーションの初期化
		@param			None
				
		@return			TRUE		成功<br>
						それ以外	失敗、エラーコードが戻り値となる
*//**************************************************************************/
MofBool CGameApp::Initialize(void){
	//リソース配置ディレクトリの設定
	CUtilities::SetCurrentDirectory("Resource");
	
	//カメラ初期化
	gCamera.SetViewPort();
	gCamera.LookAt(Vector3(-2.0f,2.0f,2.0f),Vector3(0,0,0),Vector3(0,1,0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//ライト初期化
	gLight.SetDirection(CVector3(0, -1, -1));
	CGraphicsUtilities::SetDirectionalLight(&gLight);

	//メッシュの読み込み
	pGeometry = CGraphicsUtilities::CreatePlaneGeometry(3, 3, 100, 100, TRUE, Vector3(0, 0, 0));
	//テクスチャの読み込み
	gTexture.Load("isi2_n.png");
	LPTexture pTex = new CTexture();
	pTex->Load("isi2.png");
	pGeometry->GetMaterial()->GetTextureArray()->AddLast(pTex);

	//シェーダーの読み込み
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);

	//テクスチャオブジェクトを設定
	gShaderBind.CreateShaderResource("txNormal");
	gShaderBind.CreateShaderResource("txHeight");
	
	//描画ターゲットを作成する
	MofU32 sw = 512;
	MofU32 sh = 512;
	gTarget.CreateTarget(sw, sh, PIXELFORMAT_R32_FLOAT, BUFFERACCESS_GPUREADWRITE);

	return TRUE;
}
/*************************************************************************//*!
		@brief			アプリケーションの更新
		@param			None
				
		@return			TRUE		成功<br>
						それ以外	失敗、エラーコードが戻り値となる
*//**************************************************************************/
MofBool CGameApp::Update(void){
	//キーの更新
	g_pInput->RefreshKey();

	return TRUE;
}

/*************************************************************************//*!
		@brief			アプリケーションの描画
		@param			None
				
		@return			TRUE		成功<br>
						それ以外	失敗、エラーコードが戻り値となる
*//**************************************************************************/
MofBool CGameApp::Render(void){
	//描画処理
	g_pGraphics->RenderStart();
	//画面のクリア
	g_pGraphics->ClearTarget(0.0f,0.0f,1.0f,0.0f,1.0f,0);

	//元の描画ターゲットを取得する
	LPRenderTarget pold = g_pGraphics->GetRenderTarget();

	//作成したテクスチャを描画ターゲットとして設定する
	//深度バッファは元の情報をそのまま使用する
	g_pGraphics->SetRenderTarget(gTarget.GetRenderTarget(), g_pGraphics->GetDepthTarget());

	if (g_pInput->IsMouseKeyHold(MOFMOUSE_LBUTTON))
	{
		MofFloat mx, my;
		g_pInput->GetMousePos(mx, my);
		g_pGraphics->SetBlending(BLEND_ADD);
		CGraphicsUtilities::RenderFillCircle(mx, my, 10.0f,
			MOF_ARGB(255, 255, 0, 0), MOF_ARGB(0, 0, 0, 0));
		g_pGraphics->SetBlending(BLEND_NORMAL);
	}

	//描画ターゲットを元に戻す
	g_pGraphics->SetRenderTarget(pold, g_pGraphics->GetDepthTarget());
	

	//深度バッファ有効化
	g_pGraphics->SetDepthEnable(TRUE);
	
	//カメラを設定
	gShaderBind.SetCamera(&gCamera);

	//ライトを設定
	gShaderBind.SetDirectionalLight(&gLight);

	//テクスチャの設定
	gShaderBind.GetShaderResource(0)->SetResource(&gTexture);
	gShaderBind.GetShaderResource(1)->SetResource(&gTarget);
	

	//メッシュの描画
	CMatrix44 matWorld;
	matWorld.RotationX(MOF_ToRadian(90));
	if (g_pInput->IsKeyHold(MOFKEY_SPACE))
	{
		pGeometry->Render(matWorld, &gShader, &gShaderBind);
	}
	else
	{
		pGeometry->Render(matWorld);
	}

	g_pGraphics->SetDepthEnable(FALSE);

	if (g_pInput->IsKeyHold(MOFKEY_F1))
	{
		gTarget.Render(0, 0);
	}
	
	CGraphicsUtilities::RenderString(10, 10, MOF_COLOR_WHITE, "SPACEキーでシェーダー使用");
	
	//描画の終了
	g_pGraphics->RenderEnd();
	return TRUE;
}
/*************************************************************************//*!
		@brief			アプリケーションの解放
		@param			None
				
		@return			TRUE		成功<br>
						それ以外	失敗、エラーコードが戻り値となる
*//**************************************************************************/
MofBool CGameApp::Release(void){
	MOF_SAFE_DELETE(pGeometry);
	gTexture.Release();
	gTarget.Release();
	//シェーダーの解放
	gShaderBind.Release();
	gShader.Release();
	return TRUE;
}