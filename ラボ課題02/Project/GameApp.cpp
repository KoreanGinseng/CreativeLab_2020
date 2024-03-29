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
//カメラ角度
CVector3					gCamAngle;
//メッシュ
CMeshContainer				gMesh;
//描画ターゲット
CTexture                    gTarget[2];
//シェーダー
CShader						gShader;
CShaderBind_SpriteBase		gShaderBind;

struct cbGameParam
{
	Vector4 param;  // x : screenwidth, y : screenheight, z : center.x, w : center.y
	Vector4 param2; // x : Power,       y : none,         z : none,     w : none
};

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
	gCamera.LookAt(Vector3(0.0f, 0.0f, -2.0f), Vector3(0, 0, 0), Vector3(0, 1, 0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//メッシュの読み込み
	gMesh.Load("player.mom");

	//シェーダーの読み込み
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);
	gShaderBind.CreateShaderBuffer("cbGameParam", sizeof(cbGameParam));

	//深度描画ターゲットを作成する
	MofU32 sw = g_pGraphics->GetTargetWidth();
	MofU32 sh = g_pGraphics->GetTargetHeight();
	gTarget[0].CreateTarget(sw, sh, PIXELFORMAT_R8G8B8A8_UNORM, BUFFERACCESS_GPUREADWRITE);
	gTarget[1].CreateTarget(sw, sh, PIXELFORMAT_R8G8B8A8_UNORM, BUFFERACCESS_GPUREADWRITE);
	//シェーダーに２D描画用の行列を設定する
	CGraphicsUtilities::SetScreenSize(sw, sh, &gShaderBind);
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

	//マウスによるカメラ回転
	CVector3 vRot;
	if (g_pInput->IsMouseKeyHold(MOFMOUSE_LBUTTON))
	{
		g_pInput->GetMouseMove(vRot);
		vRot *= 0.005f;
	}
	gCamAngle += Vector3(vRot.y, vRot.x, 0);

	//カメラの向きを計算
	CMatrix33 matRot;
	matRot.RotationZXY(gCamAngle);
	CVector3 vFront(0, 0, 1);
	vFront.TransformCoord(matRot, vFront);

	//キーによるカメラ移動
	float speed = 0.1f;
	CVector3 vMove;
	if (g_pInput->IsKeyHold(MOFKEY_LEFT))
	{
		vMove.x -= speed;
	}
	if (g_pInput->IsKeyHold(MOFKEY_RIGHT))
	{
		vMove.x += speed;
	}
	if (g_pInput->IsKeyHold(MOFKEY_DOWN))
	{
		vMove.z -= speed;
	}
	if (g_pInput->IsKeyHold(MOFKEY_UP))
	{
		vMove.z += speed;
	}
	vMove.TransformCoord(matRot, vMove);
	CVector3 vPos = gCamera.GetViewPosition() + vMove;

	//カメラ更新
	gCamera.LookAt(vPos, vPos + vFront, Vector3(0, 1, 0));
	gCamera.Update();
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

	//深度バッファ有効化
	g_pGraphics->SetDepthEnable(TRUE);

	//元の描画ターゲットを取得する
	LPRenderTarget pold = g_pGraphics->GetRenderTarget();

	//作成したテクスチャを描画ターゲットとして設定する
	//深度バッファは元の情報をそのまま使用する
	g_pGraphics->SetRenderTarget(gTarget[0].GetRenderTarget(), g_pGraphics->GetDepthTarget());

	//画面のクリア
	g_pGraphics->ClearTarget(0, 0, 1, 0, 1, 0);

	static float t = 0.0f;
	t += CUtilities::GetFrameSecond();

	cbGameParam sb;
	sb.param.x  = g_pGraphics->GetTargetWidth();
	sb.param.y  = g_pGraphics->GetTargetHeight();
	sb.param.z  = 0.25f;
	sb.param.w  = 0.25f;
	sb.param2.x = 20.0f * t;

	gShaderBind.GetShaderBuffer(0)->SetBuffer(&sb);

	//メッシュの描画()
	CMatrix44 matWorld;
	gMesh.Render(matWorld);

	//描画ターゲットを元に戻す
	g_pGraphics->SetRenderTarget(gTarget[1].GetRenderTarget(), g_pGraphics->GetDepthTarget());

	int texNo = 0;
	for (int i = 0; i < 4; i++)
	{
		//画面のクリア
		g_pGraphics->ClearTarget(0, 0, 1, 0, 1, 0);

		//2D描画に変更
		g_pGraphics->SetDepthEnable(FALSE);
		g_pGraphics->SetBlending(BLEND_NONE);

		CGraphicsUtilities::RenderTexture(0, 0, &gTarget[texNo], &gShader, &gShaderBind);
		g_pGraphics->SetRenderTarget(gTarget[texNo].GetRenderTarget(), g_pGraphics->GetDepthTarget());
		texNo = texNo ? 0 : 1;

	}

	//描画ターゲットを元に戻す
	g_pGraphics->SetRenderTarget(pold, g_pGraphics->GetDepthTarget());

	CGraphicsUtilities::RenderTexture(0, 0, &gTarget[texNo], &gShader, &gShaderBind);

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
	gMesh.Release();
	gTarget[0].Release();
	gTarget[1].Release();
	//シェーダーの解放
	gShaderBind.Release();
	gShader.Release();
	return TRUE;
}