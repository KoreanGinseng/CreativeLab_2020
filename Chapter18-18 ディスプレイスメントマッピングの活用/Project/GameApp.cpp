/*************************************************************************//*!
					
					@file	GameApp.cpp
					@brief	��{�Q�[���A�v���B

															@author	�_�c�@��
															@date	2014.05.14
*//**************************************************************************/

//INCLUDE
#include	"GameApp.h"

//�J����
CCamera						gCamera;
//���C�g
CDirectionalLight			gLight;
//���b�V��
LPGeometry					pGeometry;
//�e�N�X�`��
CTexture					gTexture;
//�`��^�[�Q�b�g
CTexture                    gTarget;

//�V�F�[�_�[
CShader						gShader;
CShaderBind_3DPrimitiveBase	gShaderBind;

/*************************************************************************//*!
		@brief			�A�v���P�[�V�����̏�����
		@param			None
				
		@return			TRUE		����<br>
						����ȊO	���s�A�G���[�R�[�h���߂�l�ƂȂ�
*//**************************************************************************/
MofBool CGameApp::Initialize(void){
	//���\�[�X�z�u�f�B���N�g���̐ݒ�
	CUtilities::SetCurrentDirectory("Resource");
	
	//�J����������
	gCamera.SetViewPort();
	gCamera.LookAt(Vector3(-2.0f,2.0f,2.0f),Vector3(0,0,0),Vector3(0,1,0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//���C�g������
	gLight.SetDirection(CVector3(0, -1, -1));
	CGraphicsUtilities::SetDirectionalLight(&gLight);

	//���b�V���̓ǂݍ���
	pGeometry = CGraphicsUtilities::CreatePlaneGeometry(3, 3, 100, 100, TRUE, Vector3(0, 0, 0));
	//�e�N�X�`���̓ǂݍ���
	gTexture.Load("isi2_n.png");
	LPTexture pTex = new CTexture();
	pTex->Load("isi2.png");
	pGeometry->GetMaterial()->GetTextureArray()->AddLast(pTex);

	//�V�F�[�_�[�̓ǂݍ���
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);

	//�e�N�X�`���I�u�W�F�N�g��ݒ�
	gShaderBind.CreateShaderResource("txNormal");
	gShaderBind.CreateShaderResource("txHeight");
	
	//�`��^�[�Q�b�g���쐬����
	MofU32 sw = 512;
	MofU32 sh = 512;
	gTarget.CreateTarget(sw, sh, PIXELFORMAT_R32_FLOAT, BUFFERACCESS_GPUREADWRITE);

	return TRUE;
}
/*************************************************************************//*!
		@brief			�A�v���P�[�V�����̍X�V
		@param			None
				
		@return			TRUE		����<br>
						����ȊO	���s�A�G���[�R�[�h���߂�l�ƂȂ�
*//**************************************************************************/
MofBool CGameApp::Update(void){
	//�L�[�̍X�V
	g_pInput->RefreshKey();

	return TRUE;
}

/*************************************************************************//*!
		@brief			�A�v���P�[�V�����̕`��
		@param			None
				
		@return			TRUE		����<br>
						����ȊO	���s�A�G���[�R�[�h���߂�l�ƂȂ�
*//**************************************************************************/
MofBool CGameApp::Render(void){
	//�`�揈��
	g_pGraphics->RenderStart();
	//��ʂ̃N���A
	g_pGraphics->ClearTarget(0.0f,0.0f,1.0f,0.0f,1.0f,0);

	//���̕`��^�[�Q�b�g���擾����
	LPRenderTarget pold = g_pGraphics->GetRenderTarget();

	//�쐬�����e�N�X�`����`��^�[�Q�b�g�Ƃ��Đݒ肷��
	//�[�x�o�b�t�@�͌��̏������̂܂܎g�p����
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

	//�`��^�[�Q�b�g�����ɖ߂�
	g_pGraphics->SetRenderTarget(pold, g_pGraphics->GetDepthTarget());
	

	//�[�x�o�b�t�@�L����
	g_pGraphics->SetDepthEnable(TRUE);
	
	//�J������ݒ�
	gShaderBind.SetCamera(&gCamera);

	//���C�g��ݒ�
	gShaderBind.SetDirectionalLight(&gLight);

	//�e�N�X�`���̐ݒ�
	gShaderBind.GetShaderResource(0)->SetResource(&gTexture);
	gShaderBind.GetShaderResource(1)->SetResource(&gTarget);
	

	//���b�V���̕`��
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
	
	CGraphicsUtilities::RenderString(10, 10, MOF_COLOR_WHITE, "SPACE�L�[�ŃV�F�[�_�[�g�p");
	
	//�`��̏I��
	g_pGraphics->RenderEnd();
	return TRUE;
}
/*************************************************************************//*!
		@brief			�A�v���P�[�V�����̉��
		@param			None
				
		@return			TRUE		����<br>
						����ȊO	���s�A�G���[�R�[�h���߂�l�ƂȂ�
*//**************************************************************************/
MofBool CGameApp::Release(void){
	MOF_SAFE_DELETE(pGeometry);
	gTexture.Release();
	gTarget.Release();
	//�V�F�[�_�[�̉��
	gShaderBind.Release();
	gShader.Release();
	return TRUE;
}