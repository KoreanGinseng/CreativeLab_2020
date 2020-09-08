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
//���b�V��
CMeshContainer				gMesh;
//�`��^�[�Q�b�g
CTexture					gTarget;
//�V�F�[�_�[
CShader						gShader;
CShaderBind_SpriteBase		gShaderBind;

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
	gCamera.LookAt(Vector3(-2.0f,2.0f,-2.0f),Vector3(0,0,0),Vector3(0,1,0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//���b�V���̓ǂݍ���
	gMesh.Load("player.mom");

	//�`��^�[�Q�b�g���쐬����
	MofU32 sw = g_pGraphics->GetTargetWidth();
	MofU32 sh = g_pGraphics->GetTargetHeight();
	gTarget.CreateTarget(sw, sh, PIXELFORMAT_R8G8B8A8_UNORM, BUFFERACCESS_GPUREADWRITE);

	//�V�F�[�_�[�̓ǂݍ���
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);
	//�V�F�[�_�[�ɂQD�`��p�̍s���ݒ肷��
	CGraphicsUtilities::SetScreenSize(sw, sh, &gShaderBind);
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

	//���̕`��^�[�Q�b�g���擾����
	LPRenderTarget pold = g_pGraphics->GetRenderTarget();

	//�쐬�����e�N�X�`����`��^�[�Q�b�g�Ƃ��Đݒ肷��
	//�[�x�o�b�t�@�͌��̏������̂܂܎g�p����
	g_pGraphics->SetRenderTarget(gTarget.GetRenderTarget(), g_pGraphics->GetDepthTarget());
	
	//��ʂ̃N���A
	g_pGraphics->ClearTarget(0.0f,0.0f,1.0f,1.0f,1.0f,0);
	
	//�[�x�o�b�t�@�L����
	g_pGraphics->SetDepthEnable(TRUE);

	//���b�V���̕`��
	CMatrix44 matWorld;
	gMesh.Render(matWorld);
	
	//�`��^�[�Q�b�g�����ɖ߂�
	g_pGraphics->SetRenderTarget(pold, g_pGraphics->GetDepthTarget());
	g_pGraphics->SetDepthEnable(FALSE);

	//�`�悵���^�[�Q�b�g���e�N�X�`���Ƃ��ĉ�ʂɕ`�悷��
	if (!g_pInput->IsKeyHold(MOFKEY_SPACE))
	{
		CGraphicsUtilities::RenderTexture(0, 0, &gTarget);
	}
	else
	{
		//�V�F�[�_�[�ɑ���o�b�t�@���쐬
		CGraphicsUtilities::RenderTexture(0, 0, &gTarget, &gShader, &gShaderBind);
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
	gMesh.Release();
	gTarget.Release();
	//�V�F�[�_�[�̉��
	gShaderBind.Release();
	gShader.Release();
	return TRUE;
}