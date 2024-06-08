#ifndef _turret
#define _turret
#define TURRETDAMAGE 1.0f
#define TURRETRADIUS 10.0f
#define INITIALTURRETHEALTH 100.0f
#define TURRETLOOKRADIUS 5.0f
#define TURRETVELOCITY 0.5f

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
	float** antholeInfo;

	bool isAlive;
	bool isMovingBackwards;

	XMFLOAT4* dayCycleTimer;
	int startedMovingBackwards;
	int* globalTimer;
	int* realTime;

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

	Turret(D3DXVECTOR3 startPoint, float*** possibleTargets, int numberOfTargets, float** antholeInfo, int* globalTimer, int* realTime, XMFLOAT4* dayCycleTimer, CXACT3Util* audioManager, ModeloRR** models = NULL, int animations = 1, int frames = 1) {

		isAlive = false;
		isMovingBackwards = false;
		this->realTime = realTime;
		this->globalTimer = globalTimer;
		this->dayCycleTimer = dayCycleTimer;
		this->antholeInfo = antholeInfo;

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

	bool Update(float* playerPos, float*** possibleTargets, int numberOfTargets, float*** sceneModels = NULL, int numModels = 0) {
		if (health <= 0 && isAlive)
			isAlive = false;

		if (!isAlive)
			return isAlive;

		D3DXVECTOR3 tempPosition = m_position;
		bool collided = false;

		if (this->possibleTargets) {
			delete[] this->possibleTargets;
		}

		this->possibleTargets = possibleTargets;
		this->numberOfTargets = numberOfTargets;
		playerPosition = playerPos;

		float positionXZ[2]{ m_position.x, m_position.z };
		float tempTargetInfo[3]{ currentTarget[targetPosition][0], currentTarget[targetPosition][2], TURRETLOOKRADIUS };
		if (!isPointInsideSphere(positionXZ, tempTargetInfo) || currentTarget[targetHealth][0] <= 0)
			selectNewTarget();

		D3DXVECTOR3 lookAt = { currentTarget[targetPosition][0] - m_position[0],  0.0f, currentTarget[targetPosition][2] - m_position[2] };

		D3DXVec3Normalize(&lookAt, &lookAt);

		float frontDistance = 0.0f;
		float rightDistance = 0.0f;

		if (lookAt.x != 0) {
			frontDistance = lookAt.x * TURRETVELOCITY;
		}
		if (lookAt.z != 0) {
			rightDistance = (lookAt.z * TURRETVELOCITY);
		}

		tempPosition.x += frontDistance;
		tempPosition.z += rightDistance;

		float tempPositionXZ[2]{ tempPosition.x, tempPosition.z };

		if (sceneModels && !collided) {
			for (int i = 1; i < numModels; i++) {
				float sceneModelInfo[3]{ sceneModels[i][1][0], sceneModels[i][1][2], sceneModels[i][4][1] };
				if (sceneModels[i][4][0] == 1.0f)
					if (isPointInsideSphere(tempPositionXZ, sceneModelInfo)) {
						collided = true;
						isMovingBackwards = true;
						startedMovingBackwards = *realTime;
						break;
					}
			}
			float antholeInfoArray[3]{ antholeInfo[1][0], antholeInfo[1][2], antholeInfo[2][0] - 10 };
			if (isPointInsideSphere(tempPositionXZ, antholeInfoArray)) {
				collided = true;
				isMovingBackwards = true;
				startedMovingBackwards = *realTime;
			}
		}

		if (isMovingBackwards && *realTime - startedMovingBackwards < 1) {
			m_position.x -= frontDistance * 0.5f;
			m_position.z += rightDistance * 0.5f;
			float tempCurrentPositionXZ[2]{ m_position.x, m_position.z };

			for (int i = 1; i < numModels; i++) {
				float sceneModelInfo[3]{ sceneModels[i][1][0], sceneModels[i][1][2], sceneModels[i][4][1] };
				if (sceneModels[i][4][0] == 1.0f)
					if (isPointInsideSphere(tempCurrentPositionXZ, sceneModelInfo)) {
						m_position.x += frontDistance;
						m_position.z -= rightDistance;
						break;
					}
			}
			float antholeInfoArray[3]{ antholeInfo[1][0], antholeInfo[1][2], antholeInfo[2][0] - 10 };
			if (isPointInsideSphere(tempPositionXZ, antholeInfoArray)) {
				m_position.x -= frontDistance;
				m_position.z -= rightDistance;
			}
		}
		else if (isMovingBackwards && *realTime - startedMovingBackwards >= 1) {
			isMovingBackwards = false;
		}
		else if (!collided) {
			m_position.x = tempPosition.x;
			m_position.z = tempPosition.z;
		}

		float currentPositionXZ[2]{ m_position.x, m_position.z };
		float currentTargetInfo[3]{ currentTarget[targetPosition][0], currentTarget[targetPosition][2], currentTarget[targetRadius][0] };
		if (isPointInsideSphere(currentPositionXZ, currentTargetInfo)) {
			Attack();
		}

		return isAlive;

	}

	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, D3DXVECTOR3 posCam, float specForce, float scale) {

		if (!isAlive)
			return;
		float rotAngle = atan2f(currentTarget[targetPosition][0] - m_position.x, currentTarget[targetPosition][2] - m_position.z) + (D3DX_PI * 2);
		if (isMovingBackwards)
			rotAngle -= D3DX_PI;

		m_turretModels[m_currentAnimation][m_currentFrame].Draw(vista, proyeccion, m_position, posCam, specForce, rotAngle, 'Y', scale, dayCycleTimer);
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
		//if(possibleTargets && numberOfTargets > 0)
			//if(isPointInsideSphere(new float[2] { m_position.x, m_position.z}, new float[3] { playerPosition[0], playerPosition[2], 50.0f})){}
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