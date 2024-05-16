#ifndef _player
#define _player
#define DECELERATION 0.035f
#define MAXACCELERATION 0.05f
#define MAXSPEED 1.0f
#define MAXRUNNINGSPEED 1.5f
#define MAXRCROUCHINGSPEED 0.5f
#define MAXFALLACCELERATION 1.0f
#define MAXFALLSPEED 1.0f

//Clase camara hecha por Rafael Rosas para los UltraLMADs
//Videojuegos

#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10math.h>

#include "Camara.h"
#include "ModeloRR.h"


class Player {

private:
	int m_currentAnimation;
	int m_currentFrame;
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_refUp;
	D3DXVECTOR3 m_refRight;
	D3DXVECTOR3 m_refFront;
	D3DXVECTOR3 m_refRight2d;
	D3DXVECTOR3 m_refFront2d;
	Camara** m_Camera;
	bool m_isOnVehicle;
	int m_currentCamera;
	float height[2];
	float m_acceleration[3];
	float m_speed[3];

	float** m_currentVehicle;

	bool isPointInsideSphere(float* point, float* sphere) {
		bool collition = false;

		float distance = sqrt((point[0] - sphere[0]) * (point[0] - sphere[0]) +
			(point[1] - sphere[1]) * (point[1] - sphere[1]));

		if (distance < sphere[2])
			collition = true;
		return collition;
	}

public:

	ModeloRR** m_playerModels;

	int m_animations;
	int m_frames;

	D3DXVECTOR3 m_startPosition;

	enum cameras {
		firstPerson,
		thirdPerson
	};

	bool isJumping;
	bool isRunning;
	bool isCrouching;

	Player(D3DXVECTOR3 startPoint, int Ancho, int Alto, ModeloRR** models = NULL, int animations = 1, int frames = 1) {

		m_acceleration[0] = 0.0f;
		m_acceleration[1] = 0.0f;
		m_acceleration[2] = 0.0f;
		m_speed[0] = 0.0f;
		m_speed[1] = 0.0f;
		m_speed[2] = 0.0f;

		isJumping = false;
		isRunning = false;
		isCrouching = false;

		m_position = m_startPosition = startPoint;

		m_refUp = D3DXVECTOR3(0, 1, 0);

		D3DXVec3Normalize(&m_refUp, &m_refUp);

		D3DXVECTOR3 front = D3DXVECTOR3(m_position.x, m_position.y, m_position.z - 6);

		m_refFront = D3DXVECTOR3(front.x - m_position.x, front.y - m_position.y, front.z - m_position.z);
		D3DXVec3Normalize(&m_refFront, &m_refFront);

		D3DXVec3Cross(&m_refRight, &m_refFront, &m_refUp);
		D3DXVec3Normalize(&m_refRight, &m_refRight);

		m_refFront2d = m_refFront;
		m_refRight2d = m_refRight;

		height[firstPerson] = 1.2f;

		front.y += height[firstPerson];

		m_Camera = new Camara*[2];

		m_Camera[firstPerson] = new Camara(D3DXVECTOR3(m_position.x, m_position.y + height[firstPerson], m_position.z), front, D3DXVECTOR3(0, 1, 0), Ancho, Alto);

		front = D3DXVECTOR3(m_position.x, m_position.y, m_position.z - 6);

		height[thirdPerson] = 2.5f;

		front.y += height[thirdPerson];

		m_Camera[thirdPerson] = new Camara(D3DXVECTOR3(m_position.x, m_position.y + height[thirdPerson], m_position.z), front, D3DXVECTOR3(0, 1, 0), Ancho, Alto);

		m_currentCamera = firstPerson;

		m_playerModels = models;

		if (animations > 0)
			m_animations = animations;
		else m_animations = 1;

		if (frames > 0)
			m_frames = frames;
		else m_frames = 1;
			
		m_currentAnimation = 0;
		m_currentFrame = 0;

	}

	~Player() {
		Release();
	}

	void MovePlayer(float vel, float* velDir, float arriaba, float izqder, float*** sceneModels = NULL, int numModels = 0) {
		
		D3DXVECTOR4 tempo;
		D3DXQUATERNION quatern; //quaternion temporal para la camara
		D3DXMATRIX giraUp, giraRight; //matrices temporales para los giros
		D3DXVECTOR3 tempPosition = m_position;
		D3DXVECTOR3 tempRefUp = m_refUp;
		D3DXVECTOR3 tempRefRight = m_refRight;
		D3DXVECTOR3 tempRefFront = m_refFront;
		D3DXVECTOR3 tempRefFrontPreRight;
		D3DXVECTOR3 tempRefRight2d = m_refRight2d;
		D3DXVECTOR3 tempRefFront2d = m_refFront2d;
		bool collided = false;

		//creamos al quaternion segun el vector up
		D3DXQuaternionRotationAxis(&quatern, &tempRefUp, izqder);
		//lo normalizamos para que no acumule error
		D3DXQuaternionNormalize(&quatern, &quatern);
		//creamos la matriz de rotacion basados en el quaternion
		D3DXMatrixRotationQuaternion(&giraUp, &quatern);

		//transformamos a los vectores ded la camara
		D3DXVec3Transform(&tempo, &tempRefFront, &giraUp);
		//como el resultado de la operacion anterior es Vec4 lo casteamos para hacerlo vec3
		tempRefFront = (D3DXVECTOR3)tempo;
		tempRefFrontPreRight = tempRefFront;
		//normalizamos para no acumular error
		D3DXVec3Normalize(&tempRefFront, &tempRefFront);
		D3DXVec3Normalize(&tempRefFrontPreRight, &tempRefFrontPreRight);
		//Con el vector de referencia y el nuevo front calculamos right de nuevo
		D3DXVec3Cross(&tempRefRight, &tempRefFront, &tempRefUp);
		tempRefRight2d = tempRefRight;

		D3DXVec3Transform(&tempo, &tempRefFront2d, &giraUp);
		tempRefFront2d = (D3DXVECTOR3)tempo;
		D3DXVec3Normalize(&tempRefFront2d, &tempRefFront2d);

		//una vez calculado right a partir de front y up ahora rotamos sobre right
		//repetimos el procedimiento anterior
		D3DXQuaternionRotationAxis(&quatern, &tempRefRight, arriaba);
		D3DXQuaternionNormalize(&quatern, &quatern);
		D3DXMatrixRotationQuaternion(&giraRight, &quatern);

		D3DXVec3Transform(&tempo, &tempRefFront, &giraRight);
		tempRefFront = (D3DXVECTOR3)tempo;
		D3DXVec3Normalize(&tempRefFront, &tempRefFront);

		//calculamos cuanto nos debemos de mover en cada eje

		long double radians = atan2f(velDir[2], velDir[0]);	

		if (velDir[0] != 0) {
			if (m_acceleration[0] + cos(radians) * vel * 0.1f > MAXACCELERATION)
				m_acceleration[0] = MAXACCELERATION;
			else if (m_acceleration[0] + cos(radians) * vel * 0.1f < -MAXACCELERATION)
				m_acceleration[0] = -MAXACCELERATION;
			else
				m_acceleration[0] += cos(radians) * vel * 0.1f;
		}
		if (velDir[2] != 0) {
			if (m_acceleration[2] + sin(radians) * vel * 0.1f > MAXACCELERATION)
				m_acceleration[2] = MAXACCELERATION;
			else if (m_acceleration[2] + sin(radians) * vel * 0.1f < -MAXACCELERATION)
				m_acceleration[2] = -MAXACCELERATION;
			else
				m_acceleration[2] += sin(radians) * vel * 0.1f;
		}

		if (!isRunning && !isCrouching) {
			if(m_speed[0] + m_acceleration[0] > MAXSPEED)
				m_speed[0] = MAXSPEED;
			else if(m_speed[0] + m_acceleration[0] < -MAXSPEED)
				m_speed[0] = -MAXSPEED;
			else
				m_speed[0] += m_acceleration[0];

			if(m_speed[2] + m_acceleration[2] > MAXSPEED)
				m_speed[2] = MAXSPEED;
			else if(m_speed[2] + m_acceleration[2] < -MAXSPEED)
				m_speed[2] = -MAXSPEED;
			else
				m_speed[2] += m_acceleration[2];
		}
		else if (isRunning){
			if (m_speed[0] + m_acceleration[0] > MAXRUNNINGSPEED)
				m_speed[0] = MAXRUNNINGSPEED;
			else if (m_speed[0] + m_acceleration[0] < -MAXRUNNINGSPEED)
				m_speed[0] = -MAXRUNNINGSPEED;
			else
				m_speed[0] += m_acceleration[0];

			if (m_speed[2] + m_acceleration[2] > MAXRUNNINGSPEED)
				m_speed[2] = MAXRUNNINGSPEED;
			else if (m_speed[2] + m_acceleration[2] < -MAXRUNNINGSPEED)
				m_speed[2] = -MAXRUNNINGSPEED;
			else
				m_speed[2] += m_acceleration[2];
		}
		else if (isCrouching)
		{
			if (m_speed[0] + m_acceleration[0] > MAXRCROUCHINGSPEED)
				m_speed[0] = MAXRCROUCHINGSPEED;
			else if (m_speed[0] + m_acceleration[0] < -MAXRCROUCHINGSPEED)
				m_speed[0] = -MAXRCROUCHINGSPEED;
			else
				m_speed[0] += m_acceleration[0];

			if (m_speed[2] + m_acceleration[2] > MAXRCROUCHINGSPEED)
				m_speed[2] = MAXRCROUCHINGSPEED;
			else if (m_speed[2] + m_acceleration[2] < -MAXRCROUCHINGSPEED)
				m_speed[2] = -MAXRCROUCHINGSPEED;
			else
				m_speed[2] += m_acceleration[2];
		}

		tempPosition += tempRefFront2d * m_speed[0];
		tempPosition += tempRefRight2d * m_speed[2];

		if (m_speed[0] > 0)
			m_speed[0] - DECELERATION >= 0 ? m_speed[0] -= DECELERATION : m_speed[0] = 0;
		if (m_speed[0] < 0)
			m_speed[0] + DECELERATION <= 0 ? m_speed[0] += DECELERATION : m_speed[0] = 0;
		if (m_speed[2] > 0)
			m_speed[2] - DECELERATION >= 0 ? m_speed[2] -= DECELERATION : m_speed[2] = 0;
		if (m_speed[2] < 0)
			m_speed[2] + DECELERATION <= 0 ? m_speed[2] += DECELERATION : m_speed[2] = 0;
		if (m_acceleration[0] > 0)
			m_acceleration[0] - DECELERATION >= 0 ? m_acceleration[0] -= DECELERATION : m_acceleration[0] = 0;
		if (m_acceleration[0] < 0)
			m_acceleration[0] + DECELERATION <= 0 ? m_acceleration[0] += DECELERATION : m_acceleration[0] = 0;
		if (m_acceleration[2] > 0)
			m_acceleration[2] - DECELERATION >= 0 ? m_acceleration[2] -= DECELERATION : m_acceleration[2] = 0;
		if (m_acceleration[2] < 0)
			m_acceleration[2] + DECELERATION <= 0 ? m_acceleration[2] += DECELERATION : m_acceleration[2] = 0;

		if (sceneModels) {
			for (int i = 1; i < numModels; i++) {
				if (sceneModels[i][4][0] == 1.0f)
					if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { sceneModels[i][1][0], sceneModels[i][1][2], sceneModels[i][4][1]})) {
						collided = true;
						m_speed[0] = 0;
						m_speed[2] = 0;
						break;
					}
			}
		}

		if (velDir[1] != 0) {
			if (m_acceleration[1] + velDir[1] > MAXFALLACCELERATION)
				m_acceleration[1] = MAXFALLACCELERATION;
			else if (m_acceleration[0] + velDir[1] < -MAXFALLACCELERATION)
				m_acceleration[1] = -MAXFALLACCELERATION;
			else
				m_acceleration[1] += velDir[1];
		}

		if (isJumping) {
			if (m_speed[1] + m_acceleration[1] + velDir[1] > MAXFALLSPEED)
				m_speed[1] = MAXFALLSPEED;
			else if (m_speed[1] + m_acceleration[1] + velDir[1] < -MAXFALLSPEED)
				m_speed[1] = -MAXFALLSPEED;
			else
				m_speed[1] += m_acceleration[1] + velDir[1];
			tempPosition.y += m_speed[1];
		}
		else {
			m_speed[1] = 0;
			m_acceleration[1] = 0;
		}

		D3DXVECTOR3 tempCameraPosition = tempPosition;
		tempCameraPosition.y += height[thirdPerson];
		if (!(tempCameraPosition.y - tempRefFront.y * 10.0f < tempPosition.y + 1.0f) && !(tempCameraPosition.y - tempRefFront.y * 10.0f > tempPosition.y + 10.0f)) {
			m_refFront = tempRefFront;
		}
		else {
			m_refFront = tempRefFrontPreRight;
		}

		m_refUp = tempRefUp;
		m_refRight2d = tempRefRight2d;
		m_refFront2d = tempRefFront2d;
		m_refRight = tempRefRight;
		if (!collided) {
			m_position.x = tempPosition.x;
			m_position.z = tempPosition.z;
		}
		m_position.y = tempPosition.y;

		D3DXVECTOR3 cameraPosition = m_position;
		cameraPosition.y += height[firstPerson];
		m_Camera[firstPerson]->posCam = cameraPosition + m_refFront2d * 1.4f;
		m_Camera[firstPerson]->UpdateCam(m_refFront, m_refRight);

		cameraPosition = m_position;
		cameraPosition.y += height[thirdPerson];
		m_Camera[thirdPerson]->posCam = cameraPosition - m_refFront * 10.0f;
		m_Camera[thirdPerson]->UpdateCam(m_refFront, m_refRight);

		return;

	}

	void Draw(XMFLOAT4* timer) {

		D3DXVECTOR3 playerRef = GetFrontReference2D();
		float rotAngle = playerRef.z > 0 ? atanf(playerRef.x / playerRef.z) : atanf(playerRef.x / playerRef.z) + D3DX_PI;

		m_playerModels[m_currentAnimation][m_currentFrame].Draw(m_Camera[m_currentCamera]->vista, m_Camera[m_currentCamera]->proyeccion, m_position, m_Camera[m_currentCamera]->posCam, 10.0f, rotAngle, 'Y', 1.0f, timer);
	}

	D3DXVECTOR3 GetPosition() {
		return m_position;
	}

	Camara* GetCamera() {
		return m_Camera[m_currentCamera];
	}

	Camara* GetCamera(bool person) {
		if (person)
			return m_Camera[firstPerson];
		else
			return  m_Camera[thirdPerson];
	}

	int GetCameraInt() {
		return m_currentCamera;
	}

	void moveInYAxis() {
		m_position.y -= m_acceleration[1];
	}

	void setCamera(int newCamera) {
		if(newCamera == firstPerson || newCamera == thirdPerson)
		m_currentCamera = newCamera;
	}

	D3DXVECTOR3 GetFrontReference() {
		return m_refFront;
	}
	D3DXVECTOR3 GetFrontReference2D() {
		return m_refFront2d;
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
	//Set specific speed value 1:X 2:Y 3:Z
	void SetSpeed(int option, float speed) {
		switch (option){
		case 1:
			m_speed[0] = speed;
			break;
		case 2:
			m_speed[1] = speed;
			break;
		case 3:
			m_speed[2] = speed;
			break;
		}
	}
	//Set specific acceleration value 1:X 2:Y 3:Z
	void SetAcceleration(int option, float acceleration) {
		switch (option){
		case 1:
			m_acceleration[0] = acceleration;
			break;
		case 2:
			m_acceleration[1] = acceleration;
			break;
		case 3:
			m_acceleration[2] = acceleration;
			break;
		}
	}

	float* getSpeed() {
		return m_speed;
	}
	float* getAcceleration() {
		return m_acceleration;
	}

	void Release() {
	}



};

#endif