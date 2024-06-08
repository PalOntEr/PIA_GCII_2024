#ifndef _enemy
#define _enemy
#define ENEMYDAMAGE 0.01f
#define ENEMYRADIUS 10.0f
#define INITIALENEMYHEALTH 100.0f
#define ENEMYLOOKRADIUS 400.0f

//Clase camara hecha por Rafael Rosas para los UltraLMADs
//Videojuegos

#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10math.h>

#include "Camara.h"
#include "ModeloRR.h"
#include "XACT3Util.h"


class Enemy {

private:
	int m_currentAnimation;
	int m_currentFrame;
	D3DXVECTOR3 m_position;

	XACTINDEX cueIndex[5];
	CXACT3Util* m_XACT3;

	float** currentTarget;
	float health;
	float** enemyInfo;

	bool isAlive;
	bool isPlayingSound;
	bool isMovingBackwards;

	XMFLOAT4* dayCycleTimer;
	int lastTimeSoundWasPlayed;
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

	ModeloRR** m_enemyModels;

	int m_animations;
	int m_frames;

	float*** possibleTargets;
	int numberOfTargets;

	D3DXVECTOR3 m_startPosition;

	Enemy(D3DXVECTOR3 startPoint, float*** possibleTargets, int numberOfTargets, int* globalTimer, int* realTime, XMFLOAT4* dayCycleTimer, CXACT3Util* audioManager, ModeloRR** models = NULL, int animations = 1, int frames = 1) {

		isAlive = false;
		isPlayingSound = false;
		isMovingBackwards = false;
		this->realTime = realTime;
		this->globalTimer = globalTimer;
		this->dayCycleTimer = dayCycleTimer;

		health = INITIALENEMYHEALTH;

		m_position = m_startPosition = startPoint;

		enemyInfo = new float* [3];
		enemyInfo[targetRadius] = new float;

		m_enemyModels = models;

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
		cueIndex[4] = m_XACT3->m_pSoundBank->GetCueIndex("spider_death");

		selectNewTarget();

	}

	~Enemy() {
		Release();
	}

	void MoveEnemy(float vel, float*** sceneModels = NULL, int numModels = 0) {
		
		if (isAlive && health <= 0) {
			killEnemy();
		}

		if (!isAlive)
			return;

		D3DXVECTOR3 tempPosition = m_position;
		bool collided = false;

		if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { currentTarget[targetPosition][0], currentTarget[targetPosition][2], ENEMYLOOKRADIUS}))
			selectNewTarget();

		D3DXVECTOR3 lookAt = { currentTarget[targetPosition][0] - m_position[0],  0.0f, currentTarget[targetPosition][2] - m_position[2]};

		D3DXVec3Normalize(&lookAt, &lookAt);

		long double radians = atan2f(lookAt.z, lookAt.x);

		if (possibleTargets[3] != nullptr) {
			//int tempos = possibleTargets[3][targetPosition][0];
		}

		if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { possibleTargets[1][targetPosition][0], possibleTargets[1][targetPosition][2], possibleTargets[1][targetRadius][0]})) {
			collided = true;
			currentTarget = possibleTargets[1];
			isMovingBackwards = false;
		}
		if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { currentTarget[targetPosition][0], currentTarget[targetPosition][2], currentTarget[targetRadius][0]})) {
			collided = true;
			Attack();
		}
		else if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { possibleTargets[1][targetPosition][0], possibleTargets[1][targetPosition][2], possibleTargets[1][targetRadius][0] + 0.5f})) {
			collided = false;
		}

		float frontDistance = 0.0f;
		float rightDistance = 0.0f;

		if (lookAt.x != 0) {
			frontDistance = lookAt.x * vel;
		}
		if (lookAt.z != 0) {
			rightDistance = (lookAt.z * vel);
		}
		
		tempPosition.x += frontDistance;
		tempPosition.z += rightDistance;

		if (sceneModels && !collided) {
			for (int i = 1; i < numModels; i++) {
				if (sceneModels[i][4][0] == 1.0f)
					if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { sceneModels[i][1][0], sceneModels[i][1][2], sceneModels[i][4][1]})) {
						collided = true;
						isMovingBackwards = true;
						startedMovingBackwards = *realTime;
						break;
					}
			}
		}

		m_position.y = tempPosition.y;
		if (isMovingBackwards && *realTime - startedMovingBackwards < 1) {
			m_position.x += -frontDistance + (rand() % 2 - 0.5f) * (0.5f);
			m_position.z += rightDistance + (rand() % 2 - 0.5f) * (0.5f);
		}
		else if (isMovingBackwards && *realTime - startedMovingBackwards >= 2) {
			isMovingBackwards = false;
		}
		else if (!collided) {
			m_position.x = tempPosition.x + (rand() % 2 - 0.5f)*(0.5f);
			m_position.z = tempPosition.z + (rand() % 2 - 0.5f)*(0.5f);
			//m_position.y += (rand() % 5);
		}

		return;

	}

	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, D3DXVECTOR3 posCam, float specForce, float scale) {

		if (!isAlive)
			return;
		float rotAngle = atan2f(currentTarget[targetPosition][0] - m_position.x, currentTarget[targetPosition][2] - m_position.z) + D3DX_PI;

		m_enemyModels[m_currentAnimation][m_currentFrame].Draw(vista, proyeccion, m_position, posCam, specForce, rotAngle, 'Y', scale, dayCycleTimer);
	}

	D3DXVECTOR3 GetPosition() {
		return m_position;
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
		return ENEMYRADIUS;
	}

	float* getHealth() {
		return &health;
	}

	float** getEnemyInfo() {
		enemyInfo[targetHealth] = &health;
		enemyInfo[targetPosition] = (float*)m_position;
		enemyInfo[targetRadius][0] = ENEMYRADIUS;
		return enemyInfo;
	}

	void Attack() {
		currentTarget[targetHealth][0] -= ENEMYDAMAGE;
		if(!isPlayingSound && isPointInsideSphere(new float[2] { m_position.x, m_position.z}, new float[3] { possibleTargets[0][targetPosition][0], possibleTargets[0][targetPosition][2], 50.0f})){
			lastTimeSoundWasPlayed = *realTime;
			isPlayingSound = true;
			m_XACT3->m_pSoundBank->Play(cueIndex[rand() % 4], 0, 0, 0);
		}
		if (isPlayingSound && *realTime - lastTimeSoundWasPlayed >= 1) {
			isPlayingSound = false;
		}
	}

	void killEnemy() {
		health = 0.0f;
		isAlive = false;
		m_XACT3->m_pSoundBank->Play(cueIndex[4], 0, 0, 0);
	}

	bool isEnemyAlive() {
		return isAlive;
	}

	void reviveEnemy() {
		health = INITIALENEMYHEALTH;
		isAlive = true;
		selectNewTarget();
	}

	void selectNewTarget() {
		float closestDistance = D3D11_FLOAT32_MAX;
		float currentDistance = 0.0f;
		int newTarget = 0;

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