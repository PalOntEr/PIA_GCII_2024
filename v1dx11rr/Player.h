#ifndef _player
#define _player

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
	int currentCamera;
	float height;

	enum cameras {
		firstPerson,
		thirdPerson
	};

public:

	ModeloRR** m_playerModels;

	int m_animations;
	int m_frames;

	D3DXVECTOR3 m_startPosition;


	Player(D3DXVECTOR3 startPoint, int Ancho, int Alto, ModeloRR** models = NULL, int animations = 1, int frames = 1) {

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

		height = 2.5f;

		front.y += height;

		m_Camera = new Camara*[2];

		m_Camera[firstPerson] = new Camara(D3DXVECTOR3(m_position.x, m_position.y + height, m_position.z), front, D3DXVECTOR3(0, 1, 0), Ancho, Alto);
		m_Camera[thirdPerson] = NULL;

		currentCamera = firstPerson;

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

	void MovePlayer(float vel, float* velDir, float arriaba, float izqder) {
		
		D3DXVECTOR4 tempo;
		D3DXQUATERNION quatern; //quaternion temporal para la camara
		D3DXMATRIX giraUp, giraRight; //matrices temporales para los giros

		//creamos al quaternion segun el vector up
		D3DXQuaternionRotationAxis(&quatern, &m_refUp, izqder);
		//lo normalizamos para que no acumule error
		D3DXQuaternionNormalize(&quatern, &quatern);
		//creamos la matriz de rotacion basados en el quaternion
		D3DXMatrixRotationQuaternion(&giraUp, &quatern);

		//transformamos a los vectores ded la camara
		D3DXVec3Transform(&tempo, &m_refFront, &giraUp);
		//como el resultado de la operacion anterior es Vec4 lo casteamos para hacerlo vec3
		m_refFront = (D3DXVECTOR3)tempo;
		//normalizamos para no acumular error
		D3DXVec3Normalize(&m_refFront, &m_refFront);
		//Con el vector de referencia y el nuevo front calculamos right de nuevo
		D3DXVec3Cross(&m_refRight, &m_refFront, &m_refUp);
		m_refRight2d = m_refRight;

		D3DXVec3Transform(&tempo, &m_refFront2d, &giraUp);
		m_refFront2d = (D3DXVECTOR3)tempo;
		D3DXVec3Normalize(&m_refFront2d, &m_refFront2d);

		//una vez calculado right a partir de front y up ahora rotamos sobre right
		//repetimos el procedimiento anterior
		D3DXQuaternionRotationAxis(&quatern, &m_refRight, arriaba);
		D3DXQuaternionNormalize(&quatern, &quatern);
		D3DXMatrixRotationQuaternion(&giraRight, &quatern);

		D3DXVec3Transform(&tempo, &m_refFront, &giraRight);
		m_refFront = (D3DXVECTOR3)tempo;
		D3DXVec3Normalize(&m_refFront, &m_refFront);

		//calculamos cuanto nos debemos de mover en cada eje
		long double frontDistance = 0.0f;
		long double rightDistance = 0.0f;

		long double radians = atan2f(velDir[2], velDir[0]);	

		if (velDir[0] != 0)
			frontDistance = cos(radians) * vel;
		if (velDir[2] != 0)
			rightDistance = sin(radians) * vel;			

		m_position += m_refFront2d * frontDistance;
		m_position += m_refRight2d * rightDistance;

		D3DXVECTOR3 cameraPosition = m_position;
		cameraPosition.y += height;
		m_Camera[firstPerson]->posCam = cameraPosition;
		m_Camera[firstPerson]->UpdateCam(m_refFront, m_refRight);

	}

	void Draw(XMFLOAT4* timer) {

		D3DXVECTOR3 playerRef = GetFrontReference2D();
		float rotAngle = playerRef.z > 0 ? atanf(playerRef.x / playerRef.z) : atanf(playerRef.x / playerRef.z) + D3DX_PI;

		m_playerModels[m_currentAnimation][m_currentFrame].Draw(m_Camera[currentCamera]->vista, m_Camera[currentCamera]->proyeccion, m_position, m_Camera[currentCamera]->posCam, 10.0f, rotAngle, 'Y', 1.0f, timer);
	}

	D3DXVECTOR3 GetPosition() {
		return m_position;
	}

	Camara* GetCamera(bool person) {
		if (person)
			return m_Camera[firstPerson];
		else
			return  m_Camera[thirdPerson];
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

	void Release() {
	}



};

#endif