#ifndef _camara
#define _camara

//Clase camara hecha por Rafael Rosas para los UltraLMADs
//Videojuegos

#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10math.h>

class Camara{
public:
	D3DXVECTOR3 posCam;
	D3DXVECTOR3 hdveo;
	D3DXVECTOR3 hdvoy;
	D3DXVECTOR3 refUp;
	D3DXVECTOR3 refRight;
	D3DXVECTOR3 refFront;
	D3DXMATRIX vista;
	D3DXMATRIX proyeccion;
	int ancho;
	int alto;
	float point[2];


	Camara(D3DXVECTOR3 eye, D3DXVECTOR3 target, D3DXVECTOR3 up, float Ancho, float Alto)
	{
		//posicion de la camara
		posCam = eye;
		//a donde ve
		hdveo = target;
		refUp = up;
		ancho = Ancho;
		alto = Alto;

		//crea la matriz de vista
		D3DXMatrixLookAtLH(&vista, &posCam, &hdveo, &refUp);
		//la de proyeccion
		D3DXMatrixPerspectiveFovLH( &proyeccion, 1.3, Ancho / Alto, 0.01f, 3000.0f );
		//las transpone para acelerar la multiplicacion
		D3DXMatrixTranspose( &vista, &vista );
		D3DXMatrixTranspose( &proyeccion, &proyeccion );

		D3DXVec3Normalize(&refUp, &refUp);

		refFront = D3DXVECTOR3(target.x - eye.x, target.y - eye.y, target.z - eye.z);
		D3DXVec3Normalize(&refFront, &refFront);

		D3DXVec3Cross(&refRight, &refFront, &refUp);
		D3DXVec3Normalize(&refRight, &refRight);

		point[0] = posCam.x;
		point[1] = posCam.z;

	}

	D3DXMATRIX UpdateCam(D3DXVECTOR3 Front, D3DXVECTOR3 Right)
	{
		refFront = Front;
		refRight = Right;
		
		hdveo = posCam + refFront;

		//ajustamos la matriz de vista con lo obtenido
		D3DXMatrixLookAtLH(&vista, &posCam, &hdveo, &refUp);
		D3DXMatrixTranspose( &vista, &vista );

		point[0] = posCam.x;
		point[1] = posCam.z;

		return vista;
	}
	D3DXMATRIX UpdateCam(float vel, float* velDir, float arriaba, float izqder)
	{
		D3DXVECTOR4 tempo;
		D3DXQUATERNION quatern; //quaternion temporal para la camara
		D3DXMATRIX giraUp, giraRight; //matrices temporales para los giros

		//creamos al quaternion segun el vector up
		D3DXQuaternionRotationAxis(&quatern, &refUp, izqder); 
		//lo normalizamos para que no acumule error
		D3DXQuaternionNormalize(&quatern, &quatern);
		//creamos la matriz de rotacion basados en el quaternion
		D3DXMatrixRotationQuaternion(&giraUp, &quatern);

		//transformamos a los vectores ded la camara
		D3DXVec3Transform(&tempo, &refFront, &giraUp);
		//como el resultado de la operacion anterior es Vec4 lo casteamos para hacerlo vec3
		refFront = (D3DXVECTOR3)tempo;
		//normalizamos para no acumular error
		D3DXVec3Normalize(&refFront, &refFront);
		//Con el vector de referencia y el nuevo front calculamos right de nuevo
		D3DXVec3Cross(&refRight, &refFront, &refUp);

		//una vez calculado right a partir de front y up ahora rotamos sobre right
		//repetimos el procedimiento anterior
		D3DXQuaternionRotationAxis(&quatern, &refRight, arriaba);
		D3DXQuaternionNormalize(&quatern, &quatern);
		D3DXMatrixRotationQuaternion(&giraRight, &quatern);

		D3DXVec3Transform(&tempo, &refFront, &giraRight);
		refFront = (D3DXVECTOR3)tempo;
		D3DXVec3Normalize(&refFront, &refFront);

		//calculamos cuanto nos debemos de mover en cada eje
		long double frontDistance = 0.0f;
		long double rightDistance = 0.0f;

		long double radians = atan2f(velDir[2], velDir[0]);

		if (velDir[2] == 1 && velDir[0] == 1)
			radians = radians;
		if(velDir[2] == -1 && velDir[0] == 0)
			radians = radians;

		if (velDir[0] != 0)
			frontDistance = cos(radians) * vel;
		if (velDir[2] != 0)
			rightDistance = sin(radians) * vel;


		posCam += refFront * frontDistance;
		posCam += refRight * rightDistance;
		hdveo = posCam + refFront;

		//ajustamos la matriz de vista con lo obtenido
		D3DXMatrixLookAtLH(&vista, &posCam, &hdveo, &refUp);
		D3DXMatrixTranspose( &vista, &vista );

		point[0] = posCam.x;
		point[1] = posCam.z;

		return vista;
	}
	~Camara()
	{
	}

	float* GetPoint() {
		return point;
	}

};
#endif