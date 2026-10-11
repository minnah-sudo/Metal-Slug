#pragma once
#include <SFML/Window.hpp>
using namespace sf;
class Enemy;
struct Bullet {//bullet becomes a separate entity the moment it is fired
	float velX;
	float velY;
	bool active;
	Sprite bul;
	float posX;
	float posY;
	void moveBullet(char**, float, float);
	bool isRocket;//to allow them to move in projectile motion/steep arc for rocketlauncher
	bool isFlame;//to maintain stead stream of fire
	float startX;//for flamsehot to track blocks it has moved
	float startY;//only fo when flameshit shoots at 90 deg angle
	bool isLaser;//for a bullet to act as a long beam
	float laserTimer;//used for keeping beam visible for 0.5 secs
	char sourceName[20];
	int damage;
	bool fromEnemy;
	float isBeam;
	float gravity;
};
class Weapon {
protected:
	int ammo;
	int damage;
	float shootTimer;
	float reloadTime;//how much time to wait between each shot
	bool infinityAmmo;
	int type;//just to know which weapon is being used right now
public:
	Weapon();
	virtual void shoot(float x, float y, bool right, float ang, Bullet* bullets, Texture*, bool) = 0;//pure virtual func
	void resetTimer();
	void updateTimer();
	bool canSh()const;
	bool infAmmo()const;
	int getType()const;
	int getAmmo()const;
	void setAmmo(int);
	virtual ~Weapon() {};
};
class Pistol : public Weapon {//type 1 weapon
public:
	Pistol();
	void shoot(float, float, bool, float, Bullet*, Texture*, bool)override;
};
class HMG : public Weapon {//type 2 weapon
public:
	HMG();
	void shoot(float, float, bool, float, Bullet*, Texture*, bool)override;
};
class RocketLauncher : public Weapon {//type 3 weapon
public:
	RocketLauncher();
	void shoot(float, float, bool, float, Bullet*, Texture*, bool)override;
};
class FlameShot :public Weapon {//type 4 weapon
	int range;
public:
	FlameShot();
	void shoot(float, float, bool, float, Bullet*, Texture*, bool)override;
};
class LaserGun : public Weapon {//type 5 weapon
	int beamLength;//now the bullet will be stretched to this length and will act as the beam
public:
	LaserGun();
	void shoot(float, float, bool, float, Bullet*, Texture*, bool)override;
};

class Grenade {
	Sprite ger;
	float velX;
	float velY;
	float posX;
	float posY;
	bool fireBomb;//to check whether it would have properties of Fire Bomb Grenade specifically for Eri
	bool exploded;//to check whther it has collided with something and has exploded
	float gravity;//applies to velY for projectile motion
	float poolTimer;//for the fire pool being active for 10 sec
	bool active;
	Enemy* owner; // for enemy attack logic 
	bool fromEnemy;
public:
	Grenade();
	void throwGrenade(float, float, bool, bool);
	void update(char**);
	void addTex(Texture*);
	void setActive(bool);
	bool isActive()const;
	Sprite& getSp();
	bool getExploded()const;
	float getX()const;
	float getY()const;
	void collided();
	bool getFire()const;
	float getPoolTime()const;
	void setPool(float);
	void dropGrenade(float x, float y); // handles the grenade being dropped directly down by flying tara 
	void setOwner(Enemy* o);
	Enemy* getOwner() const;
	void setFromEnemy(bool e);
	bool getFromEnemy() const;
};
