#ifndef _player
#define _player
#define DECELERATION 0.035f
#define MAXACCELERATION 0.05f
#define MAXSPEED 1.0f
#define MAXVEHICLESPEED 5.0f
#define MAXRUNNINGSPEED 1.5f
#define MAXRCROUCHINGSPEED 0.5f
#define MAXFALLACCELERATION 1.0f
#define MAXFALLSPEED 1.0f
#define RADIUS 10.0f
#define VEHICLE_RADIUS 3.0f
#define INITIALHEALTH 100.0f
#define DAMAGE 10.0f
#define COLLIDING_CAMERA_STOPS_MOVEMENT false
#define CAMERA_COLLIDES_WITH_STRUCTURES false

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
	float height[3];
	float m_acceleration[3];
	float m_speed[3];

	float health;

	float** m_currentVehicle;

	float** playerInfo;

	enum targetInfo {
		targetHealth,
		targetPosition,
		targetRadius
	};

	bool isPointInsideSphere(float* point, float* sphere) {
		bool collition = false;

		float distance = sqrt((point[0] - sphere[0]) * (point[0] - sphere[0]) +
			(point[1] - sphere[1]) * (point[1] - sphere[1]));

		if (distance < sphere[2])
			collition = true;
		return collition;
	}

	bool isPointInsideRect(float* point, float* wall) {

		if (point[0] >= wall[0] &&         // right of the left edge AND
			point[0] <= wall[0] + wall[3] &&    // left of the right edge AND
			point[1] >= wall[1] &&         // below the top AND
			point[1] <= wall[1] + wall[4]) {    // above the bottom
			return true;
		}

		return false;
	}

	struct inventoryItem {
		string name = "";
		int model = 0;
		int shopId = 0;
	};

	struct inventorySlot {
		inventoryItem item;
		bool isOccupied = false;
	};

public:

	ModeloRR** m_playerModels;

	int m_animations;
	int m_frames;

	D3DXVECTOR3 m_startPosition;

	enum cameras {
		firstPerson,
		thirdPerson
	};

	enum placing {
		placingActive,
		placingModel,
		placingX,
		placingY,
		placingZ,
		placingId
	};

	bool isJumping;
	bool isRunning;
	bool isCrouching;
	bool isDriving;
	float* isPlacing;

	int cantLeaves;

	int totalSlotsInInventory;

	inventorySlot* inventorySlots;

	Player(D3DXVECTOR3 startPoint, int Ancho, int Alto, ModeloRR** models = NULL, int animations = 1, int frames = 1) {

		health = INITIALHEALTH;

		cantLeaves = 0;

		totalSlotsInInventory = 5;
		inventorySlots = new inventorySlot[totalSlotsInInventory];
		inventorySlots[0].isOccupied = true;
		inventorySlots[0].item.name = "Mantis";
		inventorySlots[0].item.model = 13;
		inventorySlots[0].item.shopId = 0;
		inventorySlots[1].isOccupied = true;
		inventorySlots[1].item.name = "Mantis";
		inventorySlots[1].item.model = 13;
		inventorySlots[1].item.shopId = 0;
		inventorySlots[2].isOccupied = true;
		inventorySlots[2].item.name = "Mantis";
		inventorySlots[2].item.model = 13;
		inventorySlots[2].item.shopId = 0;
		inventorySlots[3].isOccupied = true;
		inventorySlots[3].item.name = "Mantis";
		inventorySlots[3].item.model = 13;
		inventorySlots[3].item.shopId = 0;

		playerInfo = new float*[3];
		playerInfo[targetRadius] = new float[1];

		m_acceleration[0] = 0.0f;
		m_acceleration[1] = 0.0f;
		m_acceleration[2] = 0.0f;
		m_speed[0] = 0.0f;
		m_speed[1] = 0.0f;
		m_speed[2] = 0.0f;

		isJumping = false;
		isRunning = false;
		isCrouching = false;
		isDriving = false;
		isPlacing = new float[6] { 0.0f };

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

		height[2] = 4.0f;

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

	void MovePlayer(float vel, float* velDir, float arriaba, float izqder, float*** sceneModels = NULL, int numModels = 0, float ** sceneWalls = NULL, int numWalls = 0) {
		
		D3DXVECTOR4 tempo;
		D3DXQUATERNION quatern; //quaternion temporal para la camara
		D3DXMATRIX giraUp, giraRight; //matrices temporales para los giros
		D3DXVECTOR3 tempPosition;
		if(!isDriving)
			tempPosition = m_position;
		else tempPosition = m_currentVehicle[1];
		D3DXVECTOR3 tempRefUp = m_refUp;
		D3DXVECTOR3 tempRefRight = m_refRight;
		D3DXVECTOR3 tempRefFront = m_refFront;
		D3DXVECTOR3 tempRefFrontPreRight;
		D3DXVECTOR3 tempRefOnlyFront = m_refFront;
		D3DXVECTOR3 tempRefRight2d = m_refRight2d;
		D3DXVECTOR3 tempRefFront2d = m_refFront2d;
		bool collided = false;
		
		D3DXQuaternionRotationAxis(&quatern, &tempRefRight, arriaba);
		D3DXQuaternionNormalize(&quatern, &quatern);
		D3DXMatrixRotationQuaternion(&giraRight, &quatern);

		D3DXVec3Transform(&tempo, &tempRefOnlyFront, &giraRight);
		tempRefOnlyFront = (D3DXVECTOR3)tempo;
		D3DXVec3Normalize(&tempRefOnlyFront, &tempRefOnlyFront);

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

		if (!isDriving && !isRunning && !isCrouching) {
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
		else if (isDriving)
		{
			if (m_speed[0] + m_acceleration[0] > MAXVEHICLESPEED)
				m_speed[0] = MAXVEHICLESPEED;
			else if (m_speed[0] + m_acceleration[0] < -MAXVEHICLESPEED)
				m_speed[0] = -MAXVEHICLESPEED;
			else
				m_speed[0] += m_acceleration[0];

			if (m_speed[2] + m_acceleration[2] > MAXVEHICLESPEED)
				m_speed[2] = MAXVEHICLESPEED;
			else if (m_speed[2] + m_acceleration[2] < -MAXVEHICLESPEED)
				m_speed[2] = -MAXVEHICLESPEED;
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

		if (tempPosition.x > 900 || tempPosition.x < -900) {
			tempPosition.x > 900 ? tempPosition.x = 900 : tempPosition.x = -900;
			m_acceleration[0] = 0;
		}
		if (tempPosition.z > 900 || tempPosition.z < -800) {
			tempPosition.z > 900 ? tempPosition.z = 900 : tempPosition.z = -800;
			m_acceleration[2] = 0;
		}

		if (!collided && sceneModels) {
			for (int i = 1; i < numModels; i++) {
				if (sceneModels[i][4][0] == 1.0f)
					if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { sceneModels[i][1][0], sceneModels[i][1][2], sceneModels[i][4][1] + (isDriving ? VEHICLE_RADIUS : 0) })) {
						collided = true;
						m_speed[0] = 0;
						m_speed[2] = 0;
						break;
					}
			}
		}

		if (!isDriving && sceneWalls && (isPointInsideRect(new float[2] { tempPosition.x, tempPosition.z}, new float[5] { -2.0f, 15.0f, 0.0f, 4.0f, 15.0f }) || isPointInsideRect(new float[2] { tempPosition.x, tempPosition.z}, new float[5] { -17.0f, -17.0f, 0.0f, 34.0f, 34.0f }))) {
			for (int i = 0; i < numWalls; i++) {
				if (isPointInsideRect(new float[2] { tempPosition.x, tempPosition.z}, sceneWalls[i])) {
					collided = true;
					m_speed[0] = 0;
					m_speed[2] = 0;
					break;
				}
			}
			isPlacing[placingActive] = 0;
		}
		else if (isPointInsideSphere(new float[2] { tempPosition.x, tempPosition.z}, new float[3] { 0, 0, 30 + (isDriving ? VEHICLE_RADIUS : 0) })) {
			collided = true;
			m_speed[0] = 0;
			m_speed[2] = 0;
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
		tempCameraPosition -= tempRefFront * 20.0f;
		bool cameraCollidedH = false;
		bool cameraCollidedV = false;
		if (m_currentCamera == thirdPerson) {
			if (tempCameraPosition.x > 900 || tempCameraPosition.x < -900)
				cameraCollidedH = true;
			if (tempCameraPosition.z > 900 || tempCameraPosition.z < -800)
				cameraCollidedH = true;

			if (CAMERA_COLLIDES_WITH_STRUCTURES && sceneWalls) {
				for (int i = 0; i < numWalls; i++) {
					if (isPointInsideRect(new float[2] { tempCameraPosition.x, tempCameraPosition.z}, sceneWalls[i])) {
						cameraCollidedH = true;
						if(COLLIDING_CAMERA_STOPS_MOVEMENT)
							collided = true;
						break;
					}
				}
			}
		}

		if ((tempCameraPosition.y < tempPosition.y - 1.0f) || (tempCameraPosition.y > tempPosition.y + 10.0f))
			cameraCollidedV = true;

		if (!cameraCollidedH && !cameraCollidedV) {
			m_refFront = tempRefFront;
			m_refFront2d = tempRefFront2d;
		}
		else if (cameraCollidedV && !cameraCollidedH) {
			m_refFront = tempRefFrontPreRight;
			m_refFront2d = tempRefFront2d;
		}
		else if (!cameraCollidedV && cameraCollidedH) {
			m_refFront = tempRefOnlyFront;
		}

		tempRefRight2d = tempRefRight;
		m_refRight2d = tempRefRight2d;
		m_refRight = tempRefRight;

		m_refUp = tempRefUp;

		if (!isDriving) {
			if (!collided) {
				m_position.x = tempPosition.x;
				m_position.z = tempPosition.z;
			}
			m_position.y = tempPosition.y;
		}
		else {
			if (!collided) {
				m_currentVehicle[1][0] = tempPosition.x;
				m_currentVehicle[1][2] = tempPosition.z;
			}
			m_currentVehicle[1][1] = tempPosition.y;
			m_position = m_currentVehicle[1];
			m_position += tempRefFront2d * 2.0f;
			m_position.y += height[2];
		}

		D3DXVECTOR3 cameraPosition = m_position;
		cameraPosition.y += height[firstPerson];
		m_Camera[firstPerson]->posCam = cameraPosition + m_refFront2d * 1.4f;
		m_Camera[firstPerson]->UpdateCam(m_refFront, m_refRight);

		cameraPosition = m_position;
		cameraPosition.y += height[thirdPerson];
		m_Camera[thirdPerson]->posCam = cameraPosition - m_refFront * 20.0f;
		m_Camera[thirdPerson]->UpdateCam(m_refFront, m_refRight);

		return;

	}

	void Draw(XMFLOAT4* timer) {

		D3DXVECTOR3 playerRef = GetFrontReference2D();
		float rotAngle = playerRef.z > 0 ? atanf(playerRef.x / playerRef.z) : atanf(playerRef.x / playerRef.z) + D3DX_PI;

		m_playerModels[m_currentAnimation][m_currentFrame].Draw(m_Camera[m_currentCamera]->vista, m_Camera[m_currentCamera]->proyeccion, m_position, m_Camera[m_currentCamera]->posCam, 10.0f, rotAngle, 'Y', 1.0f, timer);
	}

	D3DXVECTOR3 GetPosition() {
		if (isDriving) {
			return m_currentVehicle[1];
		}
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

	D3DXVECTOR3 GetRightReference() {
		return m_refRight;
	}
	D3DXVECTOR3 GetRightReference2D() {
		return m_refRight2d;
	}

	void SetCurrentVehicle(float** newVehicle) {
		m_currentVehicle = newVehicle;
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

	float* getHeights() {
		return height;
	}

	float* getSpeed() {
		return m_speed;
	}

	float* getAcceleration() {
		return m_acceleration;
	}

	float getRadius() {
		return RADIUS;
	}

	float* getHealth() {
		return &health;
	}

	float** getPlayerInfo() {
		float** tempPlayerInfo = new float*[3];
		tempPlayerInfo[targetRadius] = new float{ RADIUS };
		tempPlayerInfo[targetHealth] = &health;
		tempPlayerInfo[targetPosition] = (float*)m_position;
		return tempPlayerInfo;
	}

	//Returns true if there was space in the inventory and item was added
	bool addItemToInventory(string Name, int model, int shopId) {
		bool added = false;
		for (int i = 0; i < totalSlotsInInventory; i++) {
			if (!inventorySlots[i].isOccupied) {
				inventorySlots[i].isOccupied = true;
				inventorySlots[i].item.name = Name;
				inventorySlots[i].item.model = model;
				inventorySlots[i].item.shopId = shopId;
				added = true;
				break;
			}
		}
		return added;
	}

	//Returns true if there was an item removed form the inventory
	bool removeItemFromInventory(int shopId) {
		bool removed = false;
		for (int i = 0; i < totalSlotsInInventory; i++) {
			if (inventorySlots[i].isOccupied && inventorySlots[i].item.shopId == shopId) {
				inventorySlots[i].isOccupied = false;
				inventorySlots[i].item.name = "";
				inventorySlots[i].item.model = 0;
				inventorySlots[i].item.shopId = 0;
				removed = true;
				break;
			}
		}
		return removed;
	}

	bool playerHasItem(int shopId) {
		bool hasIt = false;
		for (int i = 0; i < totalSlotsInInventory; i++) {
			if (inventorySlots[i].isOccupied && inventorySlots[i].item.shopId == shopId) {
				hasIt = true;
				break;
			}
		}
		return hasIt;
	}

	void Release() {
		delete[] playerInfo;
		delete[] inventorySlots;
	}

};

#endif