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
//�V�F�[�_�[
CShader                     gShader;
CShaderBind_BumpMapping     gShaderBind;

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
	gCamera.LookAt(Vector3(0.0f,0.0f,-2.0f),Vector3(0,0,0),Vector3(0,1,0));
	gCamera.PerspectiveFov(MOF_ToRadian(60.0f),1024.0f / 768.0f,0.01f,1000.0f);
	gCamera.Update();
	CGraphicsUtilities::SetCamera(&gCamera);

	//���b�V���̓ǂݍ���
	gMesh.Load("Stage/Stage_01_mdl_base.mom");

	//�V�F�[�_�[�̓ǂݍ���
	gShader.Load("Shader.hlsl");
	gShaderBind.Create(&gShader);
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
	gCamAngle += Vector3(vRot.y,vRot.x,0);

	//�J�����̌������v�Z
	CMatrix33 matRot;
	matRot.RotationZXY(gCamAngle);
	CVector3 vFront(0,0,1);
	vFront.TransformCoord(matRot,vFront);

	//�L�[�ɂ��J�����ړ�
	float speed = 0.1f;
	CVector3 vMove;
	if(g_pInput->IsKeyHold(MOFKEY_LEFT))
	{
		vMove.x -= speed;
	}
	if(g_pInput->IsKeyHold(MOFKEY_RIGHT))
	{
		vMove.x += speed;
	}
	if(g_pInput->IsKeyHold(MOFKEY_DOWN))
	{
		vMove.z -= speed;
	}
	if(g_pInput->IsKeyHold(MOFKEY_UP))
	{
		vMove.z += speed;
	}
	vMove.TransformCoord(matRot,vMove);
	CVector3 vPos = gCamera.GetViewPosition() + vMove;

	//�J�����X�V
	gCamera.LookAt(vPos,vPos + vFront,Vector3(0,1,0));
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
	//��ʂ̃N���A
	g_pGraphics->ClearTarget(0.0f,0.0f,1.0f,0.0f,1.0f,0);
	
	//�[�x�o�b�t�@�L����
	g_pGraphics->SetDepthEnable(TRUE);

	//�J������ݒ�
	gShaderBind.SetCamera(&gCamera);
	
	//�V�F�[�_�[�G�t�F�N�g���g���ĕ`��
	if (g_pInput->IsKeyHold(MOFKEY_SPACE))
	{
		gShader.SetTechnique(0);
	}
	//�V���v���ȃV�F�[�_�[�`��
	else
	{
		gShader.SetTechnique(1);
	}

	//���b�V���̕`��
	CMatrix44 matWorld;
	gMesh.Render(matWorld, &gShader, &gShaderBind);

	CGraphicsUtilities::RenderString(10, 10, "'��''��''��''�E'�L�[�ŃJ�����ړ�");
	CGraphicsUtilities::RenderString(10, 34, "�}�E�X���h���b�O�ŃJ������]");
	CGraphicsUtilities::RenderString(10, 58, MOF_COLOR_WHITE, "SPACE�L�[�ŃV�F�[�_�[�g�p");
	
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
	//���b�V�����
	gMesh.Release();
	return TRUE;
}