#ifndef _turret
#define _turret
#define TURRETDAMAGE 10.0f
#define TURRETRADIUS 10.0f
#define INITIALTURRETHEALTH 100.0f
#define TURRETLOOKRADIUS 5.0f

#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10math.h>

#include "ModeloRR.h"
#include "XACT3Util.h"


class Turret {

private:
	int m_currentAnimation;
	int m_currentFrame;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_refUp;
	D3DXVECTOR3 m_refRight2d;
	D3DXVECTOR3 m_refFront2d;

	XACTINDEX cueIndex[4];
	CXACT3Util* m_XACT3;

	float** currentTarget;
	float health;
	float** turretInfo;

	bool isAlive;

	bool isPointInsideSphere(float* point, float* sphere) {
		bool collition = false;

		float distance = sqrt((point[0] - sphere[0]) * (point[0] - sphere[0]) +
			(point[1] - sphere[1]) * (point[1] - sphere[1]));

		if (distance < sphere[2])
			collition = true;
		return collition;
	}

	enum targetInfo {
		targetHealth,
		targetPosition,
		targetRadius,
	};

public:

	ModeloRR** m_turretModels;

	int m_animations;
	int m_frames;

	float*** possibleTargets;
	int numberOfTargets;
	float* playerPosition;

	D3DXVECTOR3 m_startPosition;

	Turret(D3DXVECTOR3 startPoint, float*** possibleTargets, int numberOfTargets, CXACT3Util* audioManager, ModeloRR** models = NULL, int animations = 1, int frames = 1) {

		isAlive = false;

		health = INITIALTURRETHEALTH;

		m_position = m_startPosition = startPoint;

		turretInfo = new float* [3];
		turretInfo[targetRadius] = new float;

		m_refUp = D3DXVECTOR3(0, 1, 0);

		D3DXVec3Normalize(&m_refUp, &m_refUp);

		D3DXVECTOR3 front = D3DXVECTOR3(m_position.x, m_position.y, m_position.z - 6);

		m_refFront2d = D3DXVECTOR3(front.x - m_position.x, front.y - m_position.y, front.z - m_position.z);
		D3DXVec3Normalize(&m_refFront2d, &m_refFront2d);

		D3DXVec3Cross(&m_refRight2d, &m_refFront2d, &m_refUp);
		D3DXVec3Normalize(&m_refRight2d, &m_refRight2d);

		m_turretModels = models;

		if (animations > 0)
			m_animations = animations;
		else m_animations = 1;

		if (frames > 0)
			m_frames = frames;
		else m_frames = 1;
			
		m_currentAnimation = 0;
		m_currentFrame = 0;

		this->possibleTargets = possibleTargets;
		this->numberOfTargets = numberOfTargets;

		m_XACT3 = audioManager;

		// Cargar los indeces de los Cues
		cueIndex[0] = m_XACT3->m_pSoundBank->GetCueIndex("spider_attack_1");
		cueIndex[1] = m_XACT3->m_pSoundBank->GetCueIndex("spider_attack_2");
		cueIndex[2] = m_XACT3->m_pSoundBank->GetCueIndex("spider_attack_3");
		cueIndex[3] = m_XACT3->m_pSoundBank->GetCueIndex("spider_attack_4");

		selectNewTarget();
	}

	~Turret() {
		Release();
	}

	bool Update(float* playerPos, float*** possibleTargets, int numberOfTargets) {
		if (health <= 0)
			isAlive = false;

		if (!isAlive)
			return isAlive;

		this->possibleTargets = possibleTargets;
		this->numberOfTargets = numberOfTargets;
		playerPosition = playerPos;

		if (!isPointInsideSphere(new float[2] { m_position.x, m_position.z}, new float[3] { currentTarget[targetPosition][0], currentTarget[targetPosition][2], TURRETLOOKRADIUS}) || currentTarget[targetHealth][0] <= 0)
			selectNewTarget();

		if (isPointInsideSphere(new float[2] { m_position.x, m_position.z}, new float[3] { currentTarget[targetPosition][0], currentTarget[targetPosition][2], currentTarget[targetRadius][0]})) {
			Attack();
		}

		return isAlive;

	}

	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, D3DXVECTOR3 posCam, float specForce, float scale, XMFLOAT4* timer) {

		if (!isAlive)
			return;
		float rotAngle = atan2f(currentTarget[targetPosition][0] - m_position.x, currentTarget[targetPosition][2] - m_position.z) + (D3DX_PI * 2);

		m_turretModels[m_currentAnimation][m_currentFrame].Draw(vista, proyeccion, m_position, posCam, specForce, rotAngle, 'Y', scale, timer);
	}

	D3DXVECTOR3 GetPosition() {
		return m_position;
	}

	D3DXVECTOR3 GetFrontReference2D() {
		return m_refFront2d;
	}

	D3DXVECTOR3 GetRightReference2D() {
		return m_refRight2d;
	}

	void SetPosition(D3DXVECTOR3 position) {
		m_position = position;
	}
	//Set specific axis position 1:X 2:Y 3:Z
	void SetPosition(int option, int position) {
		switch (option){
		case 1:
			m_position.x = position;
			break;
		case 2:
			m_position.y = position;
			break;
		case 3:
			m_position.z = position;
			break;
		}
	}

	float getRadius() {
		return TURRETRADIUS;
	}

	float* getHealth() {
		return &health;
	}

	float** getTurretInfo() {
		float** tempTurretInfo = new float* [3];
		tempTurretInfo[targetRadius] = new float{ TURRETRADIUS };
		tempTurretInfo[targetHealth] = &health;
		tempTurretInfo[targetPosition] = (float*)m_position;
		return tempTurretInfo;
	}

	void Attack() {
		currentTarget[targetHealth][0] -= TURRETDAMAGE;
		if(possibleTargets && numberOfTargets > 0)
			if(isPointInsideSphere(new float[2] { m_position.x, m_position.z}, new float[3] { playerPosition[0], playerPosition[2], 50.0f})){}
				//m_XACT3->m_pSoundBank->Play(cueIndex[rand() % 4], 0, 0, 0);
	}

	void killTurret() {
		health = 0.0f;
		isAlive = false;
	}

	bool isTurretAlive() {
		return isAlive;
	}

	void reviveTurret() {
		health = INITIALTURRETHEALTH;
		isAlive = true;
		selectNewTarget();
	}

	void selectNewTarget() {
		float closestDistance = D3D11_FLOAT32_MAX;
		float currentDistance = 0.0f;

		for (int i = 0; i < numberOfTargets; i++) {
			currentDistance = sqrt((m_position.x - possibleTargets[i][targetPosition][0]) * (m_position.x - possibleTargets[i][targetPosition][0]) +
				(m_position.z - possibleTargets[i][targetPosition][2]) * (m_position.z - possibleTargets[i][targetPosition][2]));

			if (currentDistance < closestDistance) {
				closestDistance = currentDistance;
				currentTarget = possibleTargets[i];
			}
		}
	}

	float** getTarget() {
		return currentTarget;
	}

	void Release() {
	}



};

#endif