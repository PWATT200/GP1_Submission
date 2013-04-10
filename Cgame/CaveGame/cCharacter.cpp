

#include "cCharacter.h"



cCharacter::cCharacter() : cSprite()
{
	cCharacter::health = 20;
	cCharacter::damage = 2;
}


cCharacter::cCharacter(D3DXVECTOR3 sPosition, LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR theFilename, int healthpoints, int damage): cSprite(sPosition,pd3dDevice,theFilename)
{
	cCharacter::health = healthpoints;
	cCharacter::damage = damage;
}

void cCharacter::update(float deltaTime)
{
	D3DXVECTOR2 tempV2;
	D3DXVECTOR3 tempV3;
	tempV2 = cCharacter::getSpritePos2D() + (cCharacter::getTranslation())*deltaTime*20;
	tempV3 = D3DXVECTOR3(tempV2.x,tempV2.y,0.0f);
	cCharacter::setSpritePos(tempV3);
	cSprite::update();
}