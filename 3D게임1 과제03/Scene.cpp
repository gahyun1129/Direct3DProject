//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

void CScene::BuildLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	// Player's Light
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(lightDefault, lightDefault, lightDefault, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(lightDefault, lightDefault, lightDefault, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);

	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	while (m_lpGameObjects.size()) {
		m_lpGameObjects.back()->Release();
		m_lpGameObjects.pop_back();
	}

	if (m_pTerrain) delete m_pTerrain;

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (const auto& elm : m_lpGameObjects) elm->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
}

void CScene::CheckPlayerArriveEndline()
{
	XMFLOAT3 xmf3PlayerPosition = m_pPlayer->GetPosition();
	if (xmf3PlayerPosition.x >= 7670.0f) {
		xmf3PlayerPosition.x = 7670.0f;
	}
	if (xmf3PlayerPosition.x <= 0.0f) {
		xmf3PlayerPosition.x = 0.0f;
	}
	if (xmf3PlayerPosition.y >= 1700.0f) {
		xmf3PlayerPosition.y = 1700.0f;
	}
	if (xmf3PlayerPosition.z >= 7670.0f) {
		xmf3PlayerPosition.z = 7670.0f;
	}
	if (xmf3PlayerPosition.z <= 0.0f) {
		xmf3PlayerPosition.z = 0.0f;
	}
	m_pPlayer->SetPosition(xmf3PlayerPosition);
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);

	for (const auto& elm : m_lpGameObjects) {
		elm->UpdateTransform(NULL);
		elm->Render(pd3dCommandList, pCamera);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// hellicopterScene
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CHellicopterScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CMissileObject::PrepareExplosion(pd3dDevice, pd3dCommandList);
	CHellicopterObject::PrepareExplosion(pd3dDevice, pd3dCommandList);
	CHellicopterObject::PrepareMovePosition();

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildLightsAndMaterials();

	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다. 
	XMFLOAT3 xmf3Scale(30.0f, 10.0f, 30.0f);
	XMFLOAT4 xmf4Color(0.8f, 0.2f, 0.2f, 0.0f);
	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다. 
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메
	쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 17,
		17, xmf3Scale, xmf4Color);
#else
	//지형을 하나의 격자 메쉬(257x257)로 생성한다. 
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 257,
		257, xmf3Scale, xmf4Color);
#endif

	m_pTerrain->SetPosition(0.0f, 0.0f, 0.0f);

	pGunshipModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Gunship.bin");
	pGunshipModel->SetBoundingBox(pGunshipModel->m_xmOOBB, pGunshipModel);
	pGunshipModel->SetScale(5.0f, 5.0f, 5.0f);
	pGunShipObject = nullptr;

	pSuperCobraModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SuperCobra.bin");
	pSuperCobraModel->SetBoundingBox(pSuperCobraModel->m_xmOOBB, pSuperCobraModel);
	pSuperCobraModel->SetScale(5.0f, 5.0f, 5.0f);
	pSuperCobraObject = nullptr;

	pMi24Model = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Mi24.bin");
	pMi24Model->SetBoundingBox(pMi24Model->m_xmOOBB, pMi24Model);
	pMi24Model->SetScale(5.0f, 5.0f, 5.0f);
	pMi24Object = nullptr;

	CreateEnemy();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CHellicopterScene::CreateEnemy()
{
	int nSelectedObject = RD::GetRandomint(0, 2);
	int nSelectedPos = RD::GetRandomint(0, 6);

	XMFLOAT3 SpawnPosition = CHellicopterObject::m_vxmf3MovePosition[nSelectedPos];

	switch (nSelectedObject) {
	case 0:
		pGunShipObject = new CGunshipObject();
		pGunShipObject->SetChild(pGunshipModel, true);
		pGunShipObject->OnInitialize();
		pGunShipObject->SetBoundingBox(pGunShipObject->m_xmOOBB, pGunshipModel);
		pGunShipObject->SetPosition(SpawnPosition);
		m_lpGameObjects.push_back(pGunShipObject);
		break;
	case 1:
		pSuperCobraObject = new CSuperCobraObject();
		pSuperCobraObject->SetChild(pSuperCobraModel, true);
		pSuperCobraObject->OnInitialize();
		pSuperCobraObject->SetBoundingBox(pSuperCobraObject->m_xmOOBB, pSuperCobraModel);
		pSuperCobraObject->SetPosition(SpawnPosition);
		m_lpGameObjects.push_back(pSuperCobraObject);
		break;
	case 2:
		pMi24Object = new CMi24Object();
		pMi24Object->SetChild(pMi24Model, true);
		pMi24Object->OnInitialize();
		pMi24Object->SetBoundingBox(pMi24Object->m_xmOOBB, pMi24Model);
		pMi24Object->SetPosition(SpawnPosition);
		m_lpGameObjects.push_back(pMi24Object);
		break;
	}
}

void CHellicopterScene::CheckPlayerByObjectCollisions()
{
	CAirplanePlayer* Player = (CAirplanePlayer*)m_pPlayer;
	for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
		CHellicopterObject* Hellicopter = (CHellicopterObject*)*iter;
		if (Hellicopter->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB)) {
			Hellicopter->m_bBlowingUp = true;
		}
		if (Hellicopter->m_bBlowingUp && Hellicopter->m_fElapsedTimes >= Hellicopter->m_fDuration) {
			m_lpGameObjects.erase(iter);
			m_pPlayer->HP -= 20;
			cout << m_pPlayer->HP << endl;
			break;
		}
	}
}

void CHellicopterScene::CheckMissileByObjectCollisions()
{
	CAirplanePlayer* Player = (CAirplanePlayer*)m_pPlayer;

	for (int i = 0; i < MAX_LAUNCH_MISSILE; ++i) {
		if (Player->m_pMissileObject[i].m_bIsShooted && !Player->m_pMissileObject[i].m_bBlowingUp) {
			for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
				CHellicopterObject* Hellicopter = (CHellicopterObject*)*iter;
				if (Hellicopter->m_xmOOBB.Intersects(Player->m_pMissileObject[i].m_xmOOBB)) {
					Player->m_pMissileObject[i].ExploseMissile();
					m_lpGameObjects.erase(iter);
					break;
				}
			}
		}
	}
}

void CHellicopterScene::CheckObjectArriveEndline()
{
	for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
		CHellicopterObject* Hellicopter = (CHellicopterObject*)*iter;
		XMFLOAT3 xmf3HellicopterPosition = Hellicopter->GetPosition();
		if (xmf3HellicopterPosition.x >= 7670.0f) {
			xmf3HellicopterPosition.x = 7670.0f;
		}
		if (xmf3HellicopterPosition.x <= 0.0f) {
			xmf3HellicopterPosition.x = 0.0f;
		}
		if (xmf3HellicopterPosition.y >= 1700.0f) {
			xmf3HellicopterPosition.y = 1700.0f;
		}
		if (xmf3HellicopterPosition.z >= 7670.0f) {
			xmf3HellicopterPosition.z = 7670.0f;
		}
		if (xmf3HellicopterPosition.z <= 0.0f) {
			xmf3HellicopterPosition.z = 0.0f;
		}
		Hellicopter->SetPosition(xmf3HellicopterPosition);
	}
}

void CHellicopterScene::CheckObjectByGroundCollisions()
{
	for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
		CHellicopterObject* Hellicopter = (CHellicopterObject*)*iter;
		XMFLOAT3 xmf3HellicopterPosition = Hellicopter->GetPosition();
		float fHeight = m_pTerrain->GetHeight(xmf3HellicopterPosition.x, xmf3HellicopterPosition.z);
		if (xmf3HellicopterPosition.y < fHeight)
		{
			xmf3HellicopterPosition.y = fHeight;
			Hellicopter->SetPosition(xmf3HellicopterPosition);
		}
	}
}

void CHellicopterScene::CheckPlayertByGroundCollisions()
{
	XMFLOAT3 xmf3PlayerPosition = m_pPlayer->GetPosition();
	float fHeight = m_pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z);
	if (xmf3PlayerPosition.y < fHeight)
	{
		xmf3PlayerPosition.y = fHeight;
		m_pPlayer->SetPosition(xmf3PlayerPosition);
	}
}

void CHellicopterScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (const auto& elm : m_lpGameObjects) {
		CHellicopterObject* Object = (CHellicopterObject*)elm;
		Object->Animate(fTimeElapsed, NULL, m_pPlayer->GetPosition());
	}

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}

	m_fSpawnElapsed += fTimeElapsed;
	if (m_fSpawnElapsed >= m_fSpawnDelay) {
		m_fSpawnElapsed -= m_fSpawnDelay;
		if (lightDefault > 0.75f) {
			lightScale = -0.05f;
		}
		else if (lightDefault < 0.3f) {
			lightScale = 0.05f;
		}
		lightDefault += lightScale;
		m_pLights[2].m_xmf4Diffuse = XMFLOAT4(lightDefault, lightDefault, lightDefault, 1.0f);
		m_pLights[2].m_xmf4Ambient = XMFLOAT4(lightDefault, lightDefault, lightDefault, 1.0f);
		sceneColor[0] += lightScale;
		sceneColor[1] += lightScale;
		sceneColor[2] += lightScale;
		cout << "color: " << sceneColor[0] << endl;
		cout << lightDefault << endl;
		CreateEnemy();
	}

	CheckMissileByObjectCollisions();
	CheckObjectArriveEndline();
	CheckPlayerByObjectCollisions();
	CheckPlayerArriveEndline();
	CheckObjectByGroundCollisions();
	CheckPlayertByGroundCollisions();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TankScene
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CTankScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CMissileObject::PrepareExplosion(pd3dDevice, pd3dCommandList);
	CTankObject::PrepareExplosion(pd3dDevice, pd3dCommandList);
	CTankObject::PrepareMovePosition();

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildLightsAndMaterials();

	//지형을 확대할 스케일 벡터이다. x-축과 z-축은 8배, y-축은 2배 확대한다. 
	XMFLOAT3 xmf3Scale(30.0f, 2.0f, 30.0f);
	XMFLOAT4 xmf4Color(0.8f, 0.2f, 0.2f, 0.0f);
	//지형을 높이 맵 이미지 파일(HeightMap.raw)을 사용하여 생성한다. 높이 맵의 크기는 가로x세로(257x257)이다. 
#ifdef _WITH_TERRAIN_PARTITION
	/*하나의 격자 메쉬의 크기는 가로x세로(17x17)이다. 지형 전체는 가로 방향으로 16개, 세로 방향으로 16의 격자 메
	쉬를 가진다. 지형을 구성하는 격자 메쉬의 개수는 총 256(16x16)개가 된다.*/
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 17,
		17, xmf3Scale, xmf4Color);
#else
	//지형을 하나의 격자 메쉬(257x257)로 생성한다. 
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList,
		m_pd3dGraphicsRootSignature, _T("HeightMap.raw"), 257, 257, 257,
		257, xmf3Scale, xmf4Color);
#endif

	m_pTerrain->SetPosition(0.0f, 0.0f, 0.0f);

	pHummerModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Hummer.bin");
	pHummerModel->SetBoundingBox(pHummerModel->m_xmOOBB, pHummerModel);
	pHummerModel->SetScale(5.0f, 5.0f, 5.0f);
	pHummerObject = nullptr;

	pMK2Model = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/MK2.bin");
	pMK2Model->SetBoundingBox(pMK2Model->m_xmOOBB, pMK2Model);
	pMK2Model->SetScale(5.0f, 5.0f, 5.0f);
	pMK2Object = nullptr;

	pMK3Model = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/MK3.bin");
	pMK3Model->SetBoundingBox(pMK3Model->m_xmOOBB, pMK3Model);
	pMK3Model->SetScale(5.0f, 5.0f, 5.0f);
	pMK3Object = nullptr;

	CreateEnemy();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CTankScene::CreateEnemy()
{
	int nSelectedObject = RD::GetRandomint(0, 2);
	int nSelectedPos = RD::GetRandomint(0, 6);

	XMFLOAT3 SpawnPosition = CTankObject::m_vxmf3MovePosition[nSelectedPos];

	switch (nSelectedObject) {
	case 0:
		pHummerObject = new CHummerObject();
		pHummerObject->SetChild(pHummerModel, true);
		pHummerObject->OnInitialize();
		pHummerObject->SetBoundingBox(pHummerObject->m_xmOOBB, pHummerModel);
		pHummerObject->SetPosition(SpawnPosition);
		m_lpGameObjects.push_back(pHummerObject);
		break;
	case 1:
		pMK2Object = new CMK2Object();
		pMK2Object->SetChild(pMK2Model, true);
		pMK2Object->OnInitialize();
		pMK2Object->SetBoundingBox(pMK2Object->m_xmOOBB, pMK2Model);
		pMK2Object->SetPosition(SpawnPosition);
		m_lpGameObjects.push_back(pMK2Object);
		break;
	case 2:
		pMK3Object = new CMK3Object();
		pMK3Object->SetChild(pMK3Model, true);
		pMK3Object->OnInitialize();
		pMK3Object->SetBoundingBox(pMK3Object->m_xmOOBB, pMK3Model);
		pMK3Object->SetPosition(SpawnPosition);
		m_lpGameObjects.push_back(pMK3Object);
		break;
	}
}

void CTankScene::CheckPlayerByObjectCollisions()
{
	CTankPlayer* Player = (CTankPlayer*)m_pPlayer;
	for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
		CTankObject* Tank = (CTankObject*)*iter;
		if (Tank->m_xmOOBB.Intersects(m_pPlayer->m_xmOOBB)) {
			Tank->m_bBlowingUp = true;
		}
		if (Tank->m_bBlowingUp && Tank->m_fElapsedTimes >= Tank->m_fDuration) {
			m_lpGameObjects.erase(iter);
			m_pPlayer->HP -= 20;
			cout << m_pPlayer->HP << endl;
			break;
		}
	}
}

void CTankScene::CheckMissileByObjectCollisions()
{
	CTankPlayer* Player = (CTankPlayer*)m_pPlayer;

	for (const auto& bullet : Player->m_pBulletObjects | views::filter([](const auto& a) {return a->m_bIsShooted; })) {
		for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
			CTankObject* Tank = (CTankObject*)*iter;
			if (Tank->m_xmOOBB.Intersects(bullet->m_xmOOBB)) {
				m_lpGameObjects.erase(iter);
				break;
			}
		}
	}
}

void CTankScene::CheckObjectArriveEndline()
{
	for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
		CTankObject* Tank = (CTankObject*)*iter;
		XMFLOAT3 xmf3TankPosition = Tank->GetPosition();
		if (xmf3TankPosition.x >= 7670.0f) {
			xmf3TankPosition.x = 7670.0f;
		}
		if (xmf3TankPosition.x <= 0.0f) {
			xmf3TankPosition.x = 0.0f;
		}
		if (xmf3TankPosition.y >= 1700.0f) {
			xmf3TankPosition.y = 1700.0f;
		}
		if (xmf3TankPosition.z >= 7670.0f) {
			xmf3TankPosition.z = 7670.0f;
		}
		if (xmf3TankPosition.z <= 0.0f) {
			xmf3TankPosition.z = 0.0f;
		}
		Tank->SetPosition(xmf3TankPosition);
	}
}

void CTankScene::CheckObjectByGroundCollisions()
{
	for (auto iter = m_lpGameObjects.begin(); iter != m_lpGameObjects.end(); ++iter) {
		CTankObject* Tank = (CTankObject*)*iter;
		XMFLOAT3 xmf3TankPosition = Tank->GetPosition();
		float fHeight = m_pTerrain->GetHeight(xmf3TankPosition.x, xmf3TankPosition.z);
		xmf3TankPosition.y = fHeight;
		Tank->SetPosition(xmf3TankPosition);
	}
}

void CTankScene::CheckPlayerByGroundCollisions()
{
	XMFLOAT3 xmf3PlayerPosition = m_pPlayer->GetPosition();
	float fHeight = m_pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z);
	xmf3PlayerPosition.y = fHeight;
	m_pPlayer->SetPosition(xmf3PlayerPosition);
}

void CTankScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (const auto& elm : m_lpGameObjects) {
		CTankObject* Object = (CTankObject*)elm;
		Object->Animate(fTimeElapsed, NULL, m_pPlayer->GetPosition());
	}

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();

	}

	m_fSpawnElapsed += fTimeElapsed;
	if (m_fSpawnElapsed >= m_fSpawnDelay) {
		m_fSpawnElapsed -= m_fSpawnDelay;
		if (lightDefault > 0.75f) {
			lightScale = -0.05f;
		}
		else if (lightDefault < 0.3f) {
			lightScale = 0.05f;
		}
		lightDefault += lightScale;
		m_pLights[2].m_xmf4Diffuse = XMFLOAT4(lightDefault, lightDefault, lightDefault, 1.0f);
		m_pLights[2].m_xmf4Ambient = XMFLOAT4(lightDefault, lightDefault, lightDefault, 1.0f);
		sceneColor[0] += lightScale;
		sceneColor[1] += lightScale;
		sceneColor[2] += lightScale;
		cout << "color: " << sceneColor[0] << endl;
		cout << lightDefault << endl;
		CreateEnemy();
	}

	CheckMissileByObjectCollisions();
	CheckObjectArriveEndline();
	CheckPlayerByObjectCollisions();
	CheckPlayerArriveEndline();
	CheckObjectByGroundCollisions();
	CheckPlayerByGroundCollisions();
}