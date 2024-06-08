#include <windows.h>
#include <windowsx.h>
#include <sstream>

#include "DXRR.h"
#include "GamePadRR.h"

#include <dinput.h>
#include <xinput.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#define SCREEN_X 1920
#define SCREEN_Y 1080
#define SENSIBILITY 2

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

DXRR *dxrr;
GamePadRR *gamePad;
tagPOINT initialPoint;
tagPOINT actualPoint;
LPDIRECTINPUT8 m_pDirectInput = NULL;
LPDIRECTINPUTDEVICE8 m_pKeyboardDevice = NULL;
LPDIRECTINPUTDEVICE8 m_pMouseDevice = NULL;
bool windowInit = false;
bool windowFocused = false;
bool F5isPressed = false;
bool SpaceisPressed = false;
bool LMBisPressed = false;
bool RMBisPressed = false;
bool numberisPressed = false;
bool F2isPressed = false;

void createMouseDevice(HWND hWnd) {
    m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, 0);

    m_pMouseDevice->SetDataFormat(&c_dfDIMouse);

    m_pMouseDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    m_pMouseDevice->Acquire();

}

void createKeyboardDevice(HWND hWnd) {
    m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboardDevice, 0);

    m_pKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);

    m_pKeyboardDevice->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    m_pKeyboardDevice->Acquire();

}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY, sizeX, sizeY;
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wchar_t* applicationName = L"AntDefender";

    sizeX = GetSystemMetrics(SM_CXSCREEN);
    sizeY = GetSystemMetrics(SM_CYSCREEN);

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = applicationName;


    RegisterClassEx(&wc);

    // If full screen set the screen to maximum size of the users desktop and 32bit.
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth = (unsigned long)sizeX;
    dmScreenSettings.dmPelsHeight = (unsigned long)sizeY;
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Change the display settings to full screen.
    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    // Set the position of the window to the top left corner.
    posX = posY = 0;

    //RECT wr = {0, 0, SCREEN_X, SCREEN_Y};
    //AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    hWnd = CreateWindowEx(WS_EX_APPWINDOW,
                        applicationName,
                        applicationName,
                        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
                        posX,
                        posY,
                        sizeX,
                        sizeY,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);
    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);
	dxrr = new DXRR(hWnd, sizeX, sizeY);
    dxrr->vel = 0;
    gamePad = new GamePadRR(1);

    ClientToScreen(hWnd, &initialPoint);
    actualPoint.x = initialPoint.x + sizeX / 2;
    actualPoint.y = initialPoint.y + sizeY / 2;

	SetTimer(hWnd, 100, 33, NULL);
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    ::DirectInput8Create(
        hInstance, DIRECTINPUT_VERSION,
        IID_IDirectInput8, (void**)&m_pDirectInput, 0);
    createMouseDevice(hWnd);
    createKeyboardDevice(hWnd);

    windowInit = true;

    while(TRUE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(msg.message == WM_QUIT)
                break;
        }
        if(windowFocused)
            dxrr->Render();
    }

    if (m_pMouseDevice) {
        m_pMouseDevice->Unacquire();
        m_pMouseDevice->Release();
    }
    m_pMouseDevice = NULL;

    if (m_pKeyboardDevice) {
        m_pKeyboardDevice->Unacquire();
        m_pKeyboardDevice->Release();
    }
    m_pKeyboardDevice = NULL;

    if (m_pDirectInput)
        m_pDirectInput->Release();
    m_pDirectInput = NULL;

    return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    float xPos = 0;
    float yPos = 0;

    switch(message)
    {
        case WM_KILLFOCUS: {

            if (windowInit) {
                m_pMouseDevice->Unacquire();
                m_pKeyboardDevice->Unacquire();
            }
            windowFocused = false;
            ShowCursor(!windowFocused);
            return 0;
        }

        case WM_SETFOCUS: {

            if (windowInit) {
                m_pMouseDevice->Acquire();
                m_pKeyboardDevice->Acquire();
            }
            windowFocused = true;
            ShowCursor(!windowFocused);
            SetCursorPos(actualPoint.x + 1, actualPoint.y + 1);
            if(windowInit)
                SendMessageW(hWnd, WM_MOUSEMOVE, wParam, lParam);
            return 0;
        }

        case WM_DESTROY:
            {
				KillTimer(hWnd, 100);
                PostQuitMessage(0);
                return 0;
            } break;

        case WM_CLOSE:
            {
                KillTimer(hWnd, 100);
                PostQuitMessage(0);
                return 0;
            } break;

		case WM_TIMER:
			{
                return 0;
			} break;
        
        case WM_MOUSEMOVE: {

            if (windowFocused)
                SetCursorPos(actualPoint.x, actualPoint.y);
            else
                return 0;

            dxrr->frameBillboard++;
            if (dxrr->frameBillboard == 32)
                dxrr->frameBillboard = 0;

            dxrr->izqder = 0;
            dxrr->arriaba = 0;
            dxrr->vel = 0;
            dxrr->velDir[0] = 0;
            dxrr->velDir[1] = 0;
            dxrr->velDir[2] = 0;

            char keyboardData[256];
            m_pKeyboardDevice->GetDeviceState(sizeof(keyboardData), (void*)&keyboardData);

            if (keyboardData[DIK_S] & 0x80 || keyboardData[DIK_W] & 0x80 || keyboardData[DIK_A] & 0x80 || keyboardData[DIK_D] & 0x80) {

                dxrr->vel += 0.5f;
                if (keyboardData[DIK_LSHIFT]) {
                    dxrr->vel += 0.5f;
                    dxrr->player->isRunning = true;
                }
                else {
                    dxrr->player->isRunning = false;
                }
                if (keyboardData[DIK_LCONTROL]) {
                    dxrr->player->isCrouching = true;
                }
                else {
                    dxrr->player->isCrouching = false;
                }

                if (keyboardData[DIK_S] & 0x80) {
                    dxrr->velDir[0] -= 1.0f;
                }

                if (keyboardData[DIK_W] & 0x80) {
                    dxrr->velDir[0] += 1.0f;
                }

                if (keyboardData[DIK_A] & 0x80) {
                    dxrr->velDir[2] += 1.0f;
                }

                if (keyboardData[DIK_D] & 0x80) {
                    dxrr->velDir[2] -= 1.0f;
                }

            }

            if (keyboardData[DIK_SPACE] & 0x80) {
                if (!SpaceisPressed && !dxrr->won && !dxrr->lost && !dxrr->player->isJumping) {
                    dxrr->velDir[1] += 0.5f;
                    dxrr->player->isJumping = true;
                    dxrr->m_XACT3.m_pSoundBank->Play(dxrr->jumpCueIndex, 0, 0, 0);
                    SpaceisPressed = true;
                }
            }
            else
                SpaceisPressed = false;

            if (keyboardData[DIK_B] & 0x80) {
                dxrr->breakpoint = true;
            }

            if (keyboardData[DIK_F2] & 0x80) {
                if (!F2isPressed) {
                    dxrr->restartGame();
                }
                F2isPressed = true;
            }
            else
                F2isPressed = false;

            //if (keyboardData[DIK_Q] & 0x80) {
            //    //wstringstream wss;
            //    //wss << "X: " << dxrr->player->GetPosition().x << endl;
            //    //wss << "Y: " << dxrr->player->GetPosition().y << endl;
            //    //wss << "Z: " << dxrr->player->GetPosition().z << endl;
            //    //MessageBox(hWnd, wss.str().c_str(), L"Coordinates", MB_OK);
            //}

            if (dxrr->realTime >= ULTIMATEGAMEDURATION || keyboardData[DIK_ESCAPE] & 0x80) {
                KillTimer(hWnd, 100);
                PostQuitMessage(0);
                return 0;
            }

            DIMOUSESTATE mouseData;
            m_pMouseDevice->GetDeviceState(sizeof(mouseData), (void*)&mouseData);

            float playerPosition[2]{ dxrr->player->GetPosition().x, dxrr->player->GetPosition().z };
            float vehiclePosition[3]{ dxrr->sceneVehicle[1][0], dxrr->sceneVehicle[1][2], dxrr->sceneVehicle[4][1] };
            if (mouseData.rgbButtons[0] & 0x80) {
                if (!LMBisPressed)
                    if (!dxrr->won && !dxrr->lost && dxrr->isPointInsideSphere(playerPosition, vehiclePosition)) {
                        if (!dxrr->player->isDriving) {
                            dxrr->player->SetCurrentVehicle(dxrr->sceneVehicle);
                        }
                        dxrr->player->isDriving = !dxrr->player->isDriving;
                    }
                LMBisPressed = true;
                //MessageBox(hWnd, L"Left MB pressed", L"INFO", MB_OK | MB_ICONINFORMATION);
            }
            else
                LMBisPressed = false;

            if (mouseData.rgbButtons[1] & 0x80) {
                if (!RMBisPressed && !dxrr->won && !dxrr->lost) {
                    dxrr->DamageEnemies(dxrr->spiderEnemies);
                    dxrr->pickUpLeaves();
                    if (dxrr->player->isPlacing[dxrr->player->placingActive] == 1) {
                        if(dxrr->placeTurret())
                            dxrr->player->isPlacing[dxrr->player->placingActive] = 0;
                    }
                }
                RMBisPressed = true;
                //MessageBox(hWnd, L"Right MB pressed", L"INFO", MB_OK | MB_ICONINFORMATION);
            }
            else
                RMBisPressed = false;

            if (keyboardData[DIK_F5] & 0x80 || mouseData.rgbButtons[3] & 0x80) {
                if (!F5isPressed) {
                    switch (dxrr->player->GetCameraInt())
                    {
                    case Player::firstPerson:
                        dxrr->player->setCamera(Player::thirdPerson);
                        break;
                    case Player::thirdPerson:
                        dxrr->player->setCamera(Player::firstPerson);
                        break;
                    }
                    F5isPressed = true;
                }
            }
            else
                F5isPressed = false;

            if (keyboardData[DIK_0] & 0x80 || keyboardData[DIK_Q] & 0x80) {
                if (!numberisPressed) {
                    float antSalesmanPosition[3]{ 0.0f, -7.0f, 10.0f };
                    if (!dxrr->won && !dxrr->lost && dxrr->isPointInsideSphere(playerPosition, antSalesmanPosition))
                        if (dxrr->player->cantLeaves >= dxrr->shopList[0].price && dxrr->player->addItemToInventory(dxrr->shopList[0].name, dxrr->shopList[0].model, 0)) {
                            dxrr->player->cantLeaves -= dxrr->shopList[0].price;
                        }
                    if (!dxrr->won && !dxrr->lost && dxrr->player->playerHasItem(0)) {
                        if (dxrr->player->isPlacing[dxrr->player->placingActive] == 0) {
                            dxrr->player->isPlacing[dxrr->player->placingActive] = 1;
                            dxrr->player->isPlacing[dxrr->player->placingModel] = dxrr->shopList[0].model;
                            dxrr->player->isPlacing[dxrr->player->placingId] = 0;
                        }
                        else if (dxrr->player->isPlacing[dxrr->player->placingActive] == 1 && dxrr->player->isPlacing[dxrr->player->placingModel] != dxrr->shopList[0].model) {
                            dxrr->player->isPlacing[dxrr->player->placingModel] = dxrr->shopList[0].model;
                            dxrr->player->isPlacing[dxrr->player->placingId] = 0;
                        }
                        else if (dxrr->player->isPlacing[dxrr->player->placingActive] == 1 && dxrr->player->isPlacing[dxrr->player->placingModel] == dxrr->shopList[0].model) {
                            dxrr->player->isPlacing[dxrr->player->placingActive] = 0;
                        }
                    }
                    else {
                        dxrr->player->isPlacing[dxrr->player->placingActive] = 0;
                    }
                }
                numberisPressed = true;
            }
            else {
                numberisPressed = false;
            }

            // Mouse move
            dxrr->izqder = (mouseData.lX / 1000.0f) * SENSIBILITY;
            dxrr->arriaba = -(mouseData.lY / 1000.0f) * SENSIBILITY;

            if (gamePad->IsConnected())
            {


                float grados = (float)gamePad->GetState().Gamepad.sThumbRX / 32767.0;

                if (grados > 0.19 || grados < -0.19) dxrr->izqder = grados / 15;

                grados = (float)gamePad->GetState().Gamepad.sThumbRY / 32767.0;

                if (grados > 0.19 || grados < -0.19)dxrr->arriaba = grados / 15;


                float velocidad = (float)gamePad->GetState().Gamepad.sThumbLY / 32767.0;
                if (velocidad > 0.19 || velocidad < -0.19) {
                    if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
                        velocidad *= 14.5;
                    else if (gamePad->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) velocidad /= 3;
                    else velocidad *= 2.5;
                    if (velocidad > 0.19) {
                        dxrr->vel = velocidad;
                        dxrr->velDir[0] += 1.0f;
                    }
                    else if (velocidad < -0.19) {
                        dxrr->vel = velocidad;
                        dxrr->velDir[0] += 1.0f;
                    }
                }

            }
            return 0;

        }break;

        default: 
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
}



