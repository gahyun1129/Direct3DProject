//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"

#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};

class CScene
{
public:
	CScene() {};
	~CScene() {};

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; };
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; };

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	void ReleaseObjects();

	void BuildLightsAndMaterials();

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	bool ProcessInput(UCHAR *pKeysBuffer) { return false; };
	virtual void AnimateObjects(float fTimeElapsed) {};
    void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	void ReleaseUploadBuffers();

	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

	CPlayer						*m_pPlayer = NULL;
	CHeightMapTerrain			*m_pTerrain = NULL;


	virtual void CreateEnemy() {};

	virtual void CheckPlayerByObjectCollisions() {};
	virtual void CheckMissileByObjectCollisions() {};
	virtual void CheckObjectArriveEndline() {};
	virtual void CheckObjectByGroundCollisions() {};
	virtual void CheckPlayetByGroundCollisions() {};

	void CheckPlayerArriveEndline();

public:
	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;

	std::list<CGameObject*>		m_lpGameObjects;

	LIGHT						*m_pLights = NULL;
	int							m_nLights = 0;

	XMFLOAT4					m_xmf4GlobalAmbient;

	ID3D12Resource				*m_pd3dcbLights = NULL;
	LIGHTS						*m_pcbMappedLights = NULL;

	float						m_fElapsedTime = 0.0f;

	float						m_fSpawnDelay = 3.0f;
	float						m_fSpawnElapsed = 0.0f;
	bool						m_bIsChangeSpawnField = false;

};


//////////////////////////////////////////////////////////////////////////////////////////

class CHellicopterScene : public CScene {
public:
	CHellicopterScene() {};
	~CHellicopterScene() {};

	virtual void CreateEnemy();

	virtual void CheckPlayerByObjectCollisions();
	virtual void CheckMissileByObjectCollisions();
	virtual void CheckObjectArriveEndline();
	virtual void CheckObjectByGroundCollisions();
	virtual void CheckPlayertByGroundCollisions();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void AnimateObjects(float fTimeElapsed);

public:

	CGameObject* pGunshipModel;
	CGunshipObject* pGunShipObject;
	CGameObject* pSuperCobraModel;
	CSuperCobraObject* pSuperCobraObject;
	CGameObject* pMi24Model;
	CMi24Object* pMi24Object;
};

class CTankScene : public CScene {
public:
	CTankScene() {};
	~CTankScene() {};

	virtual void CreateEnemy();

	virtual void CheckPlayerByObjectCollisions();
	virtual void CheckMissileByObjectCollisions();
	virtual void CheckObjectArriveEndline();
	virtual void CheckObjectByGroundCollisions();
	virtual void CheckPlayerByGroundCollisions();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void AnimateObjects(float fTimeElapsed);

public:

	CGameObject* pHummerModel;
	CHummerObject* pHummerObject;
	CGameObject* pMK2Model;
	CMK2Object* pMK2Object;
	CGameObject* pMK3Model;
	CMK3Object* pMK3Object;
};
