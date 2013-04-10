/*
=================
main.cpp
Main entry point for the Card application
=================
*/

#include "GameConstants.h"
#include "GameResources.h"
#include "cD3DManager.h"
#include "cD3DXSpriteMgr.h"
#include "cD3DXTexture.h"
#include "cSprite.h"
#include "cExplosion.h"
#include "cXAudio.h"
#include "cD3DXFont.h"
#include "cCharacter.h"

using namespace std;

HINSTANCE hInst; // global handle to hold the application instance
HWND wndHandle; // global variable to hold the window handle

// Get a reference to the DirectX Manager
static cD3DManager* d3dMgr = cD3DManager::getInstance();

// Get a reference to the DirectX Sprite renderer Manager 
static cD3DXSpriteMgr* d3dxSRMgr = cD3DXSpriteMgr::getInstance();

RECT clientBounds;

TCHAR szTempOutput[30];

char menuStartStr[50];
char menuQuitGameStr[50];
char mainMenuStr[50];

char playerHp[20];
char enemieHp[20];
char turnsLeft[20];

bool lookingRight = true;

bool stopRight = false;
bool stopLeft = false;

bool menuScreen = true;
bool moveScreen = false;
bool fightScreen = false;
bool gameOverScreen = false;
bool congratulationScreen = false;

bool playerAttack = false;
bool playerDefend = false;

bool playBgSound = false;

float warriorMove = 0.0f;

D3DXVECTOR3 click;

list<cExplosion*> gExplode;

vector<cCharacter*> cEnemies;
vector<cCharacter*>::iterator iter;
vector<cCharacter*>::iterator index;


int turnBeforeAttack = 0;

vector<cXAudio*> cAudio;
	
cXAudio currAudio;
cXAudio menuAudio;

//RECT Window;

RECT StartButton;  
RECT QuitButton; 

RECT playerHpBox;
RECT enemieHpBox;
RECT turnsBeforeAttackBox;

RECT AttackButton;
RECT DefendButton;

/*
==================================================================
* LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
* LPARAM lParam)
* The window procedure
==================================================================
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Check any available messages from the queue
	switch (message)
	{

	case WM_KEYDOWN:
		{
			if (wParam == VK_LEFT && moveScreen == true)
			{
				lookingRight = false;
				warriorMove = -10;

			}else if (wParam == VK_RIGHT  && moveScreen == true)
			{
				lookingRight = true;
				warriorMove = 10;	
			} 
			return 0;
		}

	case WM_KEYUP:
		{
			if (wParam == VK_LEFT || wParam == VK_RIGHT)
				warriorMove = 0;
			return 0;
		}

	case WM_LBUTTONDOWN:
		{
			POINT mouseXY;
			mouseXY.x = LOWORD(lParam);
			mouseXY.y = HIWORD(lParam);

			click = D3DXVECTOR3((float)mouseXY.x,(float)mouseXY.y, 0.0f);
			if(menuScreen == true){
				if( (StartButton.right > click.x) && 
					(StartButton.left < click.x) && 
					(StartButton.top < click.y)&& 
					(StartButton.bottom > click.y))
				{
					menuScreen = false;
					moveScreen = true;

					playBgSound = false;
					
				}

				if( (QuitButton.right > click.x) && 
					(QuitButton.left < click.x) && 
					(QuitButton.top < click.y)&& 
					(QuitButton.bottom > click.y))
				{
					PostQuitMessage(0);
					return(0);
				}
			}

			if(fightScreen == true)
			{
				if( (AttackButton.right > click.x) && 
					(AttackButton.left < click.x) && 
					(AttackButton.top < click.y)&& 
					(AttackButton.bottom > click.y))
				{
					turnBeforeAttack -= 1;
					playerAttack = true;
					cAudio[1]->setSound(L"Sounds\\hit.wav",false);
					cAudio[1]->play();
				}

				if( (DefendButton.right > click.x) && 
					(DefendButton.left < click.x) && 
					(DefendButton.top < click.y)&& 
					(DefendButton.bottom > click.y))
				{
					turnBeforeAttack -= 1;
					playerDefend = true;
					cAudio[2]->setSound(L"Sounds\\block.wav",false);
					cAudio[2]->play();
				}
			}
			if(gameOverScreen == true)
			{
				if( (StartButton.right > click.x) && 
					(StartButton.left < click.x) && 
					(StartButton.top < click.y)&& 
					(StartButton.bottom > click.y))
				{
					menuScreen = true;
					gameOverScreen = false;
					
					playBgSound = false;
				}

				if( (QuitButton.right > click.x) && 
					(QuitButton.left < click.x) && 
					(QuitButton.top < click.y)&& 
					(QuitButton.bottom > click.y))
				{
					PostQuitMessage(0);
					return(0);
				}

				
			}
			if(congratulationScreen == true)
				{
					menuScreen = true;
					congratulationScreen = false;
				}
			StringCchPrintf(szTempOutput, STRSAFE_MAX_CCH, TEXT("Mouse: lLastX=%d lLastY=%d\r\n"), LOWORD(lParam), HIWORD(lParam));
			OutputDebugString(szTempOutput);
			return 0;
		}


	case WM_CLOSE:
		{
			// Exit the Game
			PostQuitMessage(0);
			return 0;
		}

	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	// Always return the message to the default window
	// procedure for further processing
	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
==================================================================
* bool initWindow( HINSTANCE hInstance )
* initWindow registers the window class for the application, creates the window
==================================================================
*/
bool initWindow( HINSTANCE hInstance )
{
	WNDCLASSEX wcex;
	// Fill in the WNDCLASSEX structure. This describes how the window
	// will look to the system
	wcex.cbSize = sizeof(WNDCLASSEX); // the size of the structure
	wcex.style = CS_HREDRAW | CS_VREDRAW; // the class style
	wcex.lpfnWndProc = (WNDPROC)WndProc; // the window procedure callback
	wcex.cbClsExtra = 0; // extra bytes to allocate for this class
	wcex.cbWndExtra = 0; // extra bytes to allocate for this instance
	wcex.hInstance = hInstance; // handle to the application instance
	wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_MyWindowIcon)); // icon to associate with the application
	wcex.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_GUNSIGHT));// the default cursor
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // the background color
	wcex.lpszMenuName = NULL; // the resource name for the menu
	wcex.lpszClassName = "CaveGame"; // the class name being created
	wcex.hIconSm = LoadIcon(hInstance,"Balloon.ico"); // the handle to the small icon

	RegisterClassEx(&wcex);
	// Create the window
	wndHandle = CreateWindow("CaveGame",			// the window class to use
		"The Cave",			// the title bar text
		WS_OVERLAPPEDWINDOW,	// the window style
		CW_USEDEFAULT, // the starting x coordinate
		CW_USEDEFAULT, // the starting y coordinate
		819, // the pixel width of the window
		648, // the pixel height of the window
		NULL, // the parent window; NULL for desktop
		NULL, // the menu for the application; NULL for none
		hInstance, // the handle to the application instance
		NULL); // no values passed to the window
	// Make sure that the window handle that is created is valid
	if (!wndHandle)
		return false;
	// Display the window on the screen
	ShowWindow(wndHandle, SW_SHOW);
	UpdateWindow(wndHandle);
	return true;
}

/*
==================================================================
// This is winmain, the main entry point for Windows applications
==================================================================
*/
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	// Initialize the window
	if ( !initWindow( hInstance ) )
		return false;
	// called after creating the window
	if ( !d3dMgr->initD3DManager(wndHandle) )
		return false;
	if ( !d3dxSRMgr->initD3DXSpriteMgr(d3dMgr->getTheD3DDevice()))
		return false;

	// Grab the frequency of the high def timer
	__int64 freq = 0;				// measured in counts per second;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	float sPC = 1.0f / (float)freq;			// number of seconds per count

	__int64 currentTime = 0;				// current time measured in counts per second;
	__int64 previousTime = 0;				// previous time measured in counts per second;

	float numFrames   = 0.0f;				// Used to hold the number of frames
	float timeElapsed = 0.0f;				// cumulative elapsed time

	GetClientRect(wndHandle,&clientBounds);

	float fpsRate = 1.0f/25.0f;

	bool reset = false;

	sprintf_s(menuStartStr,50,"  Start Game");

	sprintf_s(mainMenuStr,50,"  Main Menu");

	sprintf_s(menuQuitGameStr,50,"  Quit Game");

	LPDIRECT3DSURFACE9 menuSurface;	
	LPDIRECT3DSURFACE9 gameSurface;
	LPDIRECT3DSURFACE9 fightSurface;
	LPDIRECT3DSURFACE9 gameOverSurface;
	LPDIRECT3DSURFACE9 currentSurface;	
	LPDIRECT3DSURFACE9 congratulationSurface;

	LPDIRECT3DSURFACE9 theBackbuffer = NULL;  // This will hold the back buffer

	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	// Game Font
	cD3DXFont* caveMenuFont = new cD3DXFont(d3dMgr->getTheD3DDevice(),hInstance, "Redcap");


	// Create the background surfaces
	menuSurface = d3dMgr->getD3DSurfaceFromFile("Images\\menu.png");
	gameSurface = d3dMgr->getD3DSurfaceFromFile("Images\\moveScene.png");
	fightSurface = d3dMgr->getD3DSurfaceFromFile("Images\\fightLevel.png");
	gameOverSurface = d3dMgr->getD3DSurfaceFromFile("Images\\gameOver.png");
	congratulationSurface =  d3dMgr->getD3DSurfaceFromFile("Images\\Congratulation.png");

	//Create the textures used 
	cD3DXTexture warriorR(d3dMgr->getTheD3DDevice(),"Images\\warriorR.png");
	cD3DXTexture warriorL(d3dMgr->getTheD3DDevice(),"Images\\warriorL.png");

	D3DXVECTOR3 aCharacter;

	SetRect(&StartButton, 300, 200, 500, 260); 
	SetRect(&QuitButton, 300, 300, 500, 360); 	
	SetRect(&AttackButton, 50, 200, 200, 250);
	SetRect(&DefendButton, 50, 275, 200, 325);

	SetRect(&playerHpBox,20, 560, 250, 600);
	SetRect(&enemieHpBox,550, 560, 800, 600);
	SetRect(&turnsBeforeAttackBox,300, 560, 500, 600);

	srand((unsigned int)time(NULL));
	turnBeforeAttack = (rand() % 3 + 1);

	// Create Warrior
	D3DXVECTOR3 warriorPos = D3DXVECTOR3(200,300,0);
	cCharacter theWarrior(warriorPos,d3dMgr->getTheD3DDevice(),"Images\\warriorR.png", 40 , 3);

	// Create Enemies

	cEnemies.push_back(new cCharacter(D3DXVECTOR3(400,300,0),d3dMgr->getTheD3DDevice(),"Images\\skeleton.png", 5, 2));
	cEnemies.push_back(new cCharacter(D3DXVECTOR3(600,300,0),d3dMgr->getTheD3DDevice(),"Images\\skeleton.png", 5, 2));
	cEnemies.push_back(new cCharacter(D3DXVECTOR3(700,300,0),d3dMgr->getTheD3DDevice(),"Images\\zombie.png", 10, 4));
	cEnemies.push_back(new cCharacter(D3DXVECTOR3(800,300,0),d3dMgr->getTheD3DDevice(),"Images\\skeleton.png", 5, 2));
	cEnemies.push_back(new cCharacter(D3DXVECTOR3(1000,300,0),d3dMgr->getTheD3DDevice(),"Images\\zombie.png", 10, 4));
	cEnemies.push_back(new cCharacter(D3DXVECTOR3(1100,300,0),d3dMgr->getTheD3DDevice(),"Images\\zombie.png", 10, 4));

	//Set caveLevelTerrain
	//D3DXVECTOR3 caveLevelPos = D3DXVECTOR3(0,0,0);
	//cSprite sCave(caveLevelPos, d3dMgr->getTheD3DDevice(), "Images\\CaveLevel.png");

	D3DXVECTOR3 posHolder = D3DXVECTOR3(0,0,0);

	cAudio.push_back(new cXAudio());
	cAudio.push_back(new cXAudio());
	cAudio.push_back(new cXAudio());

	cAudio[0]->setSound(L"Sounds\\menu.wav",true);
	cAudio[0]->play();


	while( msg.message!=WM_QUIT )
	{
		// Check the message queue
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			// Game code goes here
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
			float dt = (currentTime - previousTime)*sPC;

			if(menuScreen == true){
				currentSurface = menuSurface;	
				reset = false;
				if(playBgSound == false){
					playBgSound = true;
					cAudio[0]->stop();
					cAudio[0]->setSound(L"Sounds\\menu.wav",true);
					cAudio[0]->play();
				}
			}else if(moveScreen == true){
				currentSurface = gameSurface;
				if(playBgSound == false){
					playBgSound = true;
					cAudio[0]->stop();
					cAudio[0]->setSound(L"Sounds\\level.wav",true);
					cAudio[0]->play();
				}
				if(lookingRight == true){
					theWarrior.setRealTexture(warriorR);				
				}else {
					theWarrior.setRealTexture(warriorL);
				}

			} else if(fightScreen == true){
				currentSurface = fightSurface;
				if(playBgSound == false){
					playBgSound = true;
					cAudio[0]->stop();
					cAudio[0]->setSound(L"Sounds\\combat.wav",true);
					cAudio[0]->play();
				}

				sprintf_s(playerHp,"Player Health : %d\n", theWarrior.health);
				sprintf_s(enemieHp,"Enemie Health : %d\n", (*index)->health);
				sprintf_s(turnsLeft,"Turns Left : %d\n", turnBeforeAttack);

				if(turnBeforeAttack == 0){
					srand((unsigned int)time(NULL));
					turnBeforeAttack = (rand() % 3 + 1);

					if(playerDefend == true) {
						theWarrior.health -= (*index)->damage/2;
						
					}else{
						theWarrior.health -= (*index)->damage;
						
					}
					playerDefend = false;

					if(theWarrior.health <= 0){
						gameOverScreen = true;
						fightScreen = false;
						playBgSound = false;
					}
					// ennemie attacks
				}else if (playerAttack == true || playerDefend == true){
					if (playerAttack == true){
					
						(*index)->health -= theWarrior.damage;	
					}
					playerAttack = false;
					playerDefend = false;

					if((*index)->health <= 0){
						playBgSound = false;
						fightScreen = false;
						moveScreen = true;
						cEnemies.erase(index);	
					}
					if(cEnemies.empty()){
						playBgSound = false;
						fightScreen = false;
						moveScreen = false;
						congratulationScreen = true;

						currentSurface = congratulationSurface;
						cAudio[0]->stop();
						cAudio[0]->setSound(L"Sounds\\menu.wav",true);
						cAudio[0]->play();
						if(reset == false){
							warriorPos = D3DXVECTOR3(200,300,0);
							theWarrior.setSpritePos(warriorPos);
							theWarrior.health = 35;

							cEnemies.clear();
							cEnemies.push_back(new cCharacter(D3DXVECTOR3(400,300,0),d3dMgr->getTheD3DDevice(),"Images\\skeleton.png", 5, 2));
							cEnemies.push_back(new cCharacter(D3DXVECTOR3(600,300,0),d3dMgr->getTheD3DDevice(),"Images\\skeleton.png", 5, 2));
							cEnemies.push_back(new cCharacter(D3DXVECTOR3(700,300,0),d3dMgr->getTheD3DDevice(),"Images\\zombie.png", 10, 4));
							cEnemies.push_back(new cCharacter(D3DXVECTOR3(800,300,0),d3dMgr->getTheD3DDevice(),"Images\\skeleton.png", 5, 2));
							cEnemies.push_back(new cCharacter(D3DXVECTOR3(1000,300,0),d3dMgr->getTheD3DDevice(),"Images\\zombie.png", 10, 4));
							cEnemies.push_back(new cCharacter(D3DXVECTOR3(1100,300,0),d3dMgr->getTheD3DDevice(),"Images\\zombie.png", 10, 4));
							reset = true;
						}
					}

				}
			}else if(gameOverScreen == true){
				currentSurface = gameOverSurface;
				if(playBgSound == false){
					playBgSound = true;
					cAudio[0]->stop();
					cAudio[0]->setSound(L"Sounds\\menu.wav",true);
					cAudio[0]->play();
				}
				if(reset == false){
					warriorPos = D3DXVECTOR3(200,300,0);
					theWarrior.setSpritePos(warriorPos);
					theWarrior.health = 35;

					cEnemies.clear();
					cEnemies.push_back(new cCharacter(D3DXVECTOR3(400,300,0),d3dMgr->getTheD3DDevice(),"Images\\skeleton.png", 5, 2));
					cEnemies.push_back(new cCharacter(D3DXVECTOR3(600,300,0),d3dMgr->getTheD3DDevice(),"Images\\skeleton.png", 5, 2));
					cEnemies.push_back(new cCharacter(D3DXVECTOR3(700,300,0),d3dMgr->getTheD3DDevice(),"Images\\zombie.png", 10, 4));
					cEnemies.push_back(new cCharacter(D3DXVECTOR3(800,300,0),d3dMgr->getTheD3DDevice(),"Images\\skeleton.png", 5, 2));
					cEnemies.push_back(new cCharacter(D3DXVECTOR3(1000,300,0),d3dMgr->getTheD3DDevice(),"Images\\zombie.png", 10, 4));
					cEnemies.push_back(new cCharacter(D3DXVECTOR3(1100,300,0),d3dMgr->getTheD3DDevice(),"Images\\zombie.png", 10, 4));
					reset = true;
				}
			}

			// Accumulate how much time has passed.
			timeElapsed += dt;

			if(timeElapsed > fpsRate)
			{
				//Update the character position
				if (aCharacter.x>(clientBounds.right - 400)){
					
					if(warriorMove > 0)
						warriorMove = 0;
					
					for(iter = cEnemies.begin(); iter != cEnemies.end(); ++iter)
					{
						(*iter)->setTranslation(D3DXVECTOR2(-10,0));
						(*iter)->update(timeElapsed);
					
					}			
				}else{
					for(iter = cEnemies.begin(); iter != cEnemies.end(); ++iter)
					{
						(*iter)->setTranslation(D3DXVECTOR2(0,0));
					}
				}

				if( aCharacter.x < 2 ){	
					if(warriorMove < 0)
						warriorMove = 0;	
				}

				theWarrior.setTranslation(D3DXVECTOR2(warriorMove,0));
				theWarrior.update(timeElapsed);

				aCharacter = theWarrior.getSpritePos();

				for(iter = cEnemies.begin(); iter != cEnemies.end(); ++iter)
				{
					if (theWarrior.collidedWith(theWarrior.getBoundingRect(),(*iter)->getBoundingRect()))
					{
						fightScreen = true;
						index = iter;
						theWarrior.setSpritePos(D3DXVECTOR3(300,300,0));
						(*index)->setSpritePos(D3DXVECTOR3(450,300,0));
						warriorMove = 0;
						moveScreen = false;
						OutputDebugString("FIGHT!!");
						
						playBgSound = false;
					}
				}

				d3dMgr->beginRender();
				theBackbuffer = d3dMgr->getTheBackBuffer();
				d3dMgr->updateTheSurface(currentSurface, theBackbuffer);
				d3dMgr->releaseTheBackbuffer(theBackbuffer);

				d3dxSRMgr->beginDraw();

				if(fightScreen == true || moveScreen == true ){
					//d3dxSRMgr->drawSprite(sCave.getTexture(),NULL,NULL,&caveLevelPos,0xFFFFFFFF);
					d3dxSRMgr->drawSprite(theWarrior.getTexture(),NULL,NULL,&theWarrior.getSpritePos(),0xFFFFFFFF);

					vector<cCharacter*>::iterator iterB = cEnemies.begin();

					for(iterB = cEnemies.begin(); iterB != cEnemies.end(); ++iterB)
					{

						d3dxSRMgr->drawSprite((*iterB)->getTexture(),NULL,NULL,&(*iterB)->getSpritePos(),0xFFFFFFFF);

					}	

				}
				d3dxSRMgr->endDraw();

				if (menuScreen == true){
					caveMenuFont->printText(menuStartStr,StartButton);
					caveMenuFont->printText(menuQuitGameStr,QuitButton);
				}

				if (gameOverScreen == true){
					caveMenuFont->printText(mainMenuStr,StartButton);
					caveMenuFont->printText(menuQuitGameStr,QuitButton);
				}

				if(fightScreen == true){
					caveMenuFont->printText(playerHp, playerHpBox);
					caveMenuFont->printText(turnsLeft, turnsBeforeAttackBox);
					caveMenuFont->printText(enemieHp, enemieHpBox);
				}



				d3dMgr->endRender();
				//OutputDebugString("timeElapsed > fpsRate");
				timeElapsed = 0.0f;
			}

			previousTime = currentTime;
			/*
			StringCchPrintf(szTempOutput, 30, TEXT("dt=%f\n"), dt);
			OutputDebugString(szTempOutput);
			StringCchPrintf(szTempOutput, 30, TEXT("timeElapsed=%f\n"), timeElapsed);
			OutputDebugString(szTempOutput);
			StringCchPrintf(szTempOutput, 30, TEXT("previousTime=%u\n"), previousTime);
			OutputDebugString(szTempOutput);
			StringCchPrintf(szTempOutput, 30, TEXT("fpsRate=%f\n"), fpsRate);
			OutputDebugString(szTempOutput);*/

		}
	}
	d3dxSRMgr->cleanUp();
	d3dMgr->clean();
	return (int) msg.wParam;
}
