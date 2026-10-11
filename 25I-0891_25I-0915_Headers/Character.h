#pragma once
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "Enemy.h"
using namespace sf;
class Weapon;
class Enemy;
struct PlayerData {
	float scale_x;// = 0.2;
	float scale_y;// = 0.2;
	int raw_img_x;//= 593;
	int raw_img_y;// = 470;
	int height;
	int width;
	Sprite sp;
	Texture tex;
	Texture pistolTex;
	Texture hmgTex;
	Texture rockL;
	Texture knife;
	Texture mummy;
	Texture undead;
	Texture fatso;
	Texture las;
	Texture flame;
};
class Character {
protected:
	int hp;
	int lives;
	bool alive;
	float posX;
	float posY;
	float baseSp;//max walking speed -> while walking curSp cannot exceed this
	float curSp;
	float spY;
	bool jumping;
	bool running;
	bool isRight;
	float acce;
	float gravity;
	bool dropping;
	char healthState[10];
	int stateFrameCounter;//for 1 second gap when taking damage
	bool isTar;//to check if character is Tarma to give it 20% less hp
	Weapon* currentWeapon;
	float aim;//for aiming angle in degrees
	int saturation;
	int grenades;
	int fireBombs;//for all except eri as her normal are firebombs asw
	float meleeCooldown;
	bool inVeh;
	float statusTimer;//this is for undead and mummy states
	int effect;//0 normal, 1 undead, 2 mummy
	PlayerData img;
	bool isMarc;
	bool isFio;
public:
	Character();
	void moveX(char**);
	void left();
	void right();
	void run();
	void stopRun();
	void jump(char**);
	bool onPlat(char**);
	bool isJump()const;
	void startJump();
	void startDrop();
	float getX()const;
	float getY()const;
	void setX(float);
	void setY(float);
	bool isAlive()const;
	bool wallBlocks(char**);
	bool faceRight()const;
	void setRight(bool);
	//start
	void takeDamage(bool);
	//end
	void updateStateTimer();
	int getHP()const;
	int getLives()const;
	const char* getHealth();
	float getAngle()const;
	void setAngle(float);
	Weapon* getWeapon()const;
	void setWeapon(Weapon*);
	virtual ~Character();//virtual destructor as we use base class ptrs
	virtual void switchWeapSp(int) = 0;
	void setSaturation(int);
	int getSaturation()const;
	void setGrenadeCount(int);
	void setFireBombCount(int);
	int getGerC()const;
	int getFireC()const;
	void updateMeleeTimer();//dis for da knife
	float getMelee()const;
	void setMelee(float);
	void setVeh(bool);
	bool getVeh()const;
	int getEffect()const;
	PlayerData& getImg();
	void applyEffect(int);//to tranform into undead or mummy
	float getBaseSp()const;
	float getAcc()const;
	bool getTar()const;
	bool getMarc()const;
	bool getFio()const;
	bool isFat()const;
	void setAlive(bool);
	void setLives(int);
};
class MarcoRossi : public Character {
public:
	MarcoRossi();
	void switchWeapSp(int)override;
};
class Tarma : public Character {
public:
	Tarma();
	void switchWeapSp(int)override;
};
class Fiolina :public Character {
public:
	Fiolina();
	void switchWeapSp(int)override;
};
class EriKasamoto : public Character {
public:
	EriKasamoto();
	void switchWeapSp(int)override;
};
class FusionCompanion :public Character {
	float lifeTime;
	float fireRateMult;
	float meleeDamMult;
	bool igShield;
	bool noMelee;
	float blasMult;
public:
	FusionCompanion(Character**, int, float, float);
	void switchWeapSp(int)override;
	void update(char**, float, Enemy**, int, Bullet*, Texture*, float);
};
