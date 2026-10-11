#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;
class PlayerVehicle {
protected:
	bool occupied;
	Sprite sp;
	Texture tex[3];
	int texHeights[3];
	int texWidths[3];
	float posX;
	float posY;
	bool active;
	int hits;//a vehicle can take 3 hits before it 
	float velY;
	float velX;
	float gravity;
	int vehHeight;
	int vehWidth;
public:
	PlayerVehicle();
	void spawn(float, float);
	void setOccupy(bool);
	float getX()const;
	float getY()const;
	virtual void movement(char**, bool, bool) = 0;
	virtual void animate(bool, float, bool, float) = 0;
	Sprite getSp()const;
	bool getOccupy()const;
	void takeDamage(bool, Character*);
	bool getActive()const;
};
class Slug001 :public PlayerVehicle {//MetalSlug
	Texture jumpTex;
	int jumpHeight;
	int jumpWidth;
	float spTimer;
	bool isJump;
	Texture tiltTex[3];
	int tiltWidth[3];
	int tiltHeight[3];
	float angle;//just for sprites
	float aim;
public:
	Slug001();
	void movement(char**, bool, bool)override;
	void animate(bool, float, bool, float)override;
	void aiming();//take keyboard inputs here
	float getAim()const;
};
class SlugFlyer :public PlayerVehicle {
	int missileNum;
	float bulletTimer;
	float animTimer;
	float shootTimer;//these are for the missiles
	float reloadTime;
public:
	SlugFlyer();
	void movement(char**, bool, bool)override;//this ignores gravity and just moves up and down
	void animate(bool, float, bool, float)override;
	void updateTimer();//this is for the missile timer
	void fire(Bullet*, bool, Texture*, float);
};
class SlugMariner : public PlayerVehicle {
	int amVert;//verticle misisle ammount
	int amHor;//horizontal missile ammount
	int amRev;//reverse proj ammount
	float reloadTime;
	float shootTimer;
public:
	SlugMariner();
	void movement(char**, bool, bool)override;
	void animate(bool, float, bool, float)override;
	void fire(Bullet*, bool, Texture*, float, int);
	void updateTimer();
};
//start
class AmphibiousSlug :public PlayerVehicle {
	PlayerVehicle* current;//to track which vehicle is it right now
	//composition
	Slug001* normal;//the three modes it can switch to depending on position  
	SlugFlyer* flyer;//current will point to any one of them and will call their respective functions
	SlugMariner* mariner;
public:
	AmphibiousSlug();
	void updateMode();
	void movement(char**, bool, bool)override;
	void animate(bool, float, bool, float)override;
	void updateTimer();//this is for when its mariner or flyer
	void fire(Bullet*, bool, Texture*, float, int);
	Sprite getSp()const;
	~AmphibiousSlug();
	void aiming();//for when its normal metal slug
	Slug001* getNorm();
};
//end
