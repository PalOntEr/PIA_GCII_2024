#ifndef _enemy
#define _enemy
#define ENEMYDAMAGE 0.1f
#define ENEMYRADIUS 10.0f
#define INITIALENEMYHEALTH 100.0f

//Clase camara hecha por Rafael Rosas para los UltraLMADs
//Videojuegos

#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10math.h>

#include "Camara.h"
#include "Player.h"
#include "ModeloRR.h"


class Enemy {

private:
	int m_currentAnimation;
	int m_currentFrame;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_refUp;
	D3DXVECTOR3 m_refRight2d;
	D3DXVECTOR3 m_refFront2d;

	Player* player;
	float health;

	bool isAlive;

	bool isPointInsideSphere(float* point, float* sphere) {
		bool collition = false;

		float distance = sqrt((point[0] - sphere[0]) * (point[0] - sphere[0]) +
			(point[1] - sphere[1]) * (point[1] - sphere[1]));

		if (distance < sphere[2])
			collition = true;
		return collition;
	}

public:

	ModeloRR** m_enemyModels;

	int m_animations;
	int m_frames;

	D3DXVECTOR3 m_startPosition;

	Enemy(Player* targetPlayer, D3DXVECTOR3 startPoint, ModeloRR** models = NULL, int animations = 1, int frames = 1) {

		isAlive = true;

		health = INITIALENEMYHEALTH;

		m_position = m_startPosition = startPoint;

		m_refUp = D3DXVECTOR3(0, 1, 0);

		D3DXVec3Normalize(&m_refUp, &m_refUp);

		D3DXVECTOR3 front = D3DXVECTOR3(m_position.x, m_position.y, m_position.z - 6);

		m_refFront2d = D3DXVECTOR3(front.x - m_position.x, front.y - m_position.y, front.z - m_position.z);
		D3DXVec3Normalize(&m_refFront2d, &m_refFront2d);

		D3DXVec3Cross(&m_refRight2d, &m_refFront2d, &m_refUp);
		D3DXVec3Normalize(&m_refRight2d, &m_refRight2d);

		m_enemyModels = models;

		if (animations > 0)
			m_animations = animations;
		else m_animations = 1;

		if (frames > 0)
			m_frames = frames;
		else m_frames = 1;
			
		m_currentAnimation = 0;
		m_currentFrame = 0;

		player = targetPlayer;

	}

	~Enemy() {
		Release();
	}

	void MoveEnemy(float vel, float*** sceneModels = NULL, int numModels = 0) {
		
		if (!isAlive)
			return;

		D3DXVECTOR4 tempo;
		D3DXQUATERNION quatern; //quaternion temporal para la camara
		D3DXMATRIX giraUp; //matrices temporales para los giros
		D3DXVECTOR3 tempPosition = m_position;
		D3DXVECTOR3 tempRefUp = m_refUp;
		D3DXVECTOR3 tempRefRight2d = m_refRight2d;
		D3DXVECTOR3 tempRefFront2d = m_refFront2d;
		bool collided = false;

		D3DXVECTOR3 lookAt = { player->GetPosition().x - m_position[0],  0.0f, player->GetPosition().z - m_position[2] };

		D3DXVec3Normalize(&lookAt, &lookAt);

		long double radians = atan2f(lookAt.z, lookAt.x);

		float angle = radians * (180 / D3DX_PI);

		//creamos al quaternion segun el vector up
		D3DXQuaternionRotationAxis(&quatern, &tempRefUp, angle);
		//lo normalizamos para que no acumule error
		D3DXQuaternionNormalize(&quatern, &quatern);
		//creamos la matriz de rotacion basados en el quaternion
		D3DXMatrixRotationQuaternion(&giraUp, &quatern);

		//transformamos a los vectores ded la camara
		D3DXVec3Transform(&tempo, &tempRefFront2d, &giraUp);
		//como el resultado de la operacion anterior es Vec4 lo casteamos para hacerlo vec3
		tempRefFront2d = (D3DXVECTOR3)tempo;
		//normalizamos para no acumular error
		D3DXVec3Normalize(&tempRefFront2d, &tempRefFront2d);
		//Con el vector de referencia y el nuevo front calculamos right de nuevo
		D3DXVec3Cross(&tempRefRight2d, &tempRefFront2d, &tempRefUp);
		D3DXVec3Normalize(&tempRefRight2d, &tempRefRight2d);

		//calculamos cuanto nos debemos de mover en cada eje

		if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { player->GetPosition().x, player->GetPosition().z, player->getRadius()})) {
			collided = true;
			*player->getHealth() -= ENEMYDAMAGE;
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

		if (sceneModels) {
			for (int i = 1; i < numModels; i++) {
				if (sceneModels[i][4][0] == 1.0f)
					if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { sceneModels[i][1][0], sceneModels[i][1][2], sceneModels[i][4][1]})) {
						collided = true;
						break;
					}
			}
		}

		m_refUp = tempRefUp;
		m_refRight2d = tempRefRight2d;
		m_refFront2d = tempRefFront2d;
		m_position.y = tempPosition.y;
		if (!collided) {
			m_position.x = tempPosition.x + (rand() % 2 - 0.5f)*(0.5f);
			m_position.z = tempPosition.z + (rand() % 2 - 0.5f)*(0.5f);
			//m_position.y += (rand() % 5);
		}

		return;

	}

	void Draw(D3DXMATRIX vista, D3DXMATRIX proyeccion, D3DXVECTOR3 posCam, float specForce, float scale, XMFLOAT4* timer) {

		if (!isAlive)
			return;

		float rotAngle = atan2f(player->GetPosition().x - m_position.x, player->GetPosition().z - m_position.z) + D3DX_PI;

		m_enemyModels[m_currentAnimation][m_currentFrame].Draw(vista, proyeccion, m_position, posCam, specForce, rotAngle, 'Y', scale, timer);
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
		return ENEMYRADIUS;
	}

	float* getHealth() {
		return &health;
	}

	void killEnemy() {
		health = 0.0f;
		isAlive = false;
	}

	bool isEnemyAlive() {
		return isAlive;
	}

	void Release() {
	}



};

#endif