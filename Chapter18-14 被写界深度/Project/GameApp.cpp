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
//�J�����p�x
CVector3					gCamAngle;
//���b�V��
CMeshContainer				gMesh;
//�`��^�[�Q�b�g
CTexture                    gTarget;
CTexture					gDepthTarget;
//�V�F�[�_�[
CShader						gShader;
CShaderBind_3DPrimitiveBase	gShaderBind;

//�V�F�[�_�[���̃R���X�^���g�o�b�t�@�Ɠ��l�̍\����
struct cbGameParam
{
	Vector4	cbProjection;
};

//�V�F�[�_�[
CShader                     gDofShader;
CShaderBind_SpriteBase      gDofShaderBind;

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
	gCamera.LookAt(Vector3(0.0f, 0.0f, -2.0f), Vector3(0, 0, 0), Vector3(0, 1, 0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//���b�V���̓ǂݍ���
	gMesh.Load("player.mom");

	//�V�F�[�_�[�̓ǂݍ���
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);

	//�V�F�[�_�[�Ƃ̃o�b�t�@�̘A�g
	gShaderBind.CreateShaderBuffer("cbGameParam", sizeof(cbGameParam));

	//�[�x�`��^�[�Q�b�g���쐬����
	MofU32 sw = g_pGraphics->GetTargetWidth();
	MofU32 sh = g_pGraphics->GetTargetHeight();
	gDepthTarget.CreateTarget(sw, sh, PIXELFORMAT_R32_FLOAT, BUFFERACCESS_GPUREADWRITE);

	//�ʏ�̕`��^�[�Q�b�g���쐬����
	gTarget.CreateTarget(sw, sh, PIXELFORMAT_R8G8B8A8_UNORM, BUFFERACCESS_GPUREADWRITE);
	
	//�V�F�[�_�[�̓ǂݍ���
	gDofShader.Load("Dof.hlsl");
	gDofShaderBind.Create(&gDofShader);

	//�V�F�[�_�[�ɂQD�`��p�̍s���ݒ肷��
	CGraphicsUtilities::SetScreenSize(sw, sh, &gDofShaderBind);

	//�e�N�X�`���I�u�W�F�N�g��ݒ�
	gDofShaderBind.CreateShaderResource("txDepth");
	
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

	//�}�E�X�ɂ��J������]
	CVector3 vRot;
	if (g_pInput->IsMouseKeyHold(MOFMOUSE_LBUTTON))
	{
		g_pInput->GetMouseMove(vRot);
		vRot *= 0.005f;
	}
	gCamAngle += Vector3(vRot.y, vRot.x, 0);

	//�J�����̌������v�Z
	CMatrix33 matRot;
	matRot.RotationZXY(gCamAngle);
	CVector3 vFront(0, 0, 1);
	vFront.TransformCoord(matRot, vFront);

	//�L�[�ɂ��J�����ړ�
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

	//�J�����X�V
	gCamera.LookAt(vPos, vPos + vFront, Vector3(0, 1, 0));
	gCamera.Update();
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
	g_pGraphics->SetRenderTarget(gDepthTarget.GetRenderTarget(), g_pGraphics->GetDepthTarget());

	//��ʂ̃N���A
	g_pGraphics->ClearTarget(0.0f,0.0f,0.0f,0.0f,1.0f,0);
	
	//�[�x�o�b�t�@�L����
	g_pGraphics->SetDepthEnable(TRUE);
	
	//�J������ݒ�
	gShaderBind.SetCamera(&gCamera);

	//�V�F�[�_�[�ɑ���o�b�t�@���쐬
	cbGameParam sb;
	sb.cbProjection.x = 3;
	//�V�F�[�_�[�Ƀo�b�t�@�𑗂�
	gShaderBind.GetShaderBuffer(0)->SetBuffer(&sb);

	//���b�V���̕`��(�[�x���o�͂���)
	CMatrix44 matWorld;
	gMesh.Render(matWorld, &gShader, &gShaderBind);


	//�`��^�[�Q�b�g��F�o�͗p�ɐ؂�ւ���
	g_pGraphics->SetRenderTarget(gTarget.GetRenderTarget(), g_pGraphics->GetDepthTarget());

	//��ʂ̃N���A
	g_pGraphics->ClearTarget(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0);

	//���b�V���̕`��(�ʏ�̕`��)
	gMesh.Render(matWorld);

	//�`��^�[�Q�b�g�����ɖ߂�
	g_pGraphics->SetRenderTarget(pold, g_pGraphics->GetDepthTarget());

	//2D�`��ɕύX
	g_pGraphics->SetDepthEnable(FALSE);
	g_pGraphics->SetBlending(BLEND_NONE);

	//��ʂ̃N���A
	g_pGraphics->ClearTarget(0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0);

	//�`�悵���^�[�Q�b�g���e�N�X�`���Ƃ��ĉ�ʂɕ`�悷��
	if (!g_pInput->IsKeyHold(MOFKEY_SPACE))
	{
		//��ʂ�`�悷��
		CGraphicsUtilities::RenderTexture(0, 0, &gTarget);
		//�m�F�p�ɉ�ʉE��ɐ[�x��\������
		gDepthTarget.Render(CRectangle(0, 0, g_pGraphics->GetTargetWidth() / 4, g_pGraphics->GetTargetHeight() / 4));
	}
	else
	{
		//�e�N�X�`���̐ݒ�
		gDofShaderBind.GetShaderResource(0)->SetResource(&gDepthTarget);
		CGraphicsUtilities::RenderTexture(0, 0, &gTarget, &gDofShader, &gDofShaderBind);
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
	gDepthTarget.Release();
	//�V�F�[�_�[�̉��
	gShaderBind.Release();
	gShader.Release();
	gDofShaderBind.Release();
	gDofShader.Release();
	return TRUE;
}