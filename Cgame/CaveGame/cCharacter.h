
#ifndef _CCHARACTER_H
#define _CCHARACTER_H
#include "cSprite.h"

class cCharacter : public cSprite
{
private:
	

public:
	int health;
	int damage;
	cCharacter();
	cCharacter(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename, int healthPoints, int damage); // Constructor
	void update(float deltaTime);						// update the character position based on the translation
};
#endif

