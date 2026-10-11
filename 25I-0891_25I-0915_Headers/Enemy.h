#pragma once
#include <SFML/Graphics.hpp>
#include "Weapon.h"
#include "Character.h"
using namespace sf;

class Character;//fwd dec
// truct for sprite dimensions 
struct SpriteData {
	float scale_x;// = 0.2;
	float scale_y;// = 0.2;
	int raw_img_x;//= 593;
	int raw_img_y;// = 470;
	int height;
	int width;
};

// ENEMY CLASS (PARENT FOR ALL ENEMIES)
class Enemy {
protected:
	char* name;
	int hp;
	bool isAlive;
	bool isJumping;
	double speed;
	double positionX;
	double positionY;
	int batchMin;
	int batchMax;
	double gravity;
	double velocityY;
	bool facingRight;
	bool onGround;
	int enemyDamage;
	double attackRange;
	bool explosionHit;
	int score;
	SpriteData spritedata;
	int playerState;
	Texture idleTex;
	Texture walkTex[2];
	Texture attackTex;
	Texture dieTex;
	Sprite sprite;
	float minMove;
	float maxMove;
	bool dropped;//this is to check if probability of them dropping food has been calculated or not at time of death
	float burnTimer;
	int texCounter;
	int texFrame;
	int frameLimit;
	bool knifeHit;
	int deathTimer;
public:
	Enemy();
	virtual void move(const int, char**, int, int, float) = 0;
	virtual void die() = 0;
	virtual void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = 0) = 0;
	virtual void draw(RenderWindow& window, float camera_x, float camera_y) = 0;
	virtual void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) = 0;
	double getX();
	double getY();
	int getWidth();
	int getHeight();
	bool getAlive();
	void setPosition(double, double);
	virtual void setMoves(float, float); // making it virtual so the paratrooper can have its own implementation 
	bool isRange(float) const;
	virtual ~Enemy();
	bool getDropped()const;
	void setDropped(bool);
	int getHp()const;
	float getBurnTime()const;
	void setBurnTime(float);
	int getScore()const;
	bool getJumping()const;
};

class Infantry : public Enemy {
public:
	Infantry();
	void move(const int, char**, int, int, float) override;
	void die() override;
	void draw(RenderWindow& window, float camera_x, float camera_y) override;  // all infantry are drawn the same way 
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = 0) override;
};

class RebelSoldier : public Infantry {
	Weapon* pistol;
public:
	RebelSoldier(double x, double y);
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;  // update function will be implemneted inside the children as it will be different for different enemy types  
	~RebelSoldier();
};

class ShieldedSoldier : public Infantry {
	Weapon* pistol;
	bool shieldActive;
public:
	ShieldedSoldier(double x, double y);
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
	~ShieldedSoldier();
};

class GrenadeSoldier : public Infantry {
	float cooldown;   // for grenade throwing 
public:
	GrenadeSoldier(double x, double y);
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
};

class BazookaSoldier : public Infantry {
	Weapon* rocketLauncher;
public:
	BazookaSoldier(double, double);
	~BazookaSoldier();
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
};

class Undead : public Enemy {
public:
	Undead();
	void move(const int, char**, int, int, float) override;
	void die() override;
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
};
class MummyWarrior : public Undead {
	int resurrectTime;
	bool isCrumbling;
public:
	MummyWarrior(double, double);
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
};

class Zombie : public Undead {
	Weapon* pistol;
public:
	Zombie(double, double);
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	~Zombie();
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
};

class Aerial : public Enemy {
protected:
	bool isLanded;
	Infantry* soldier;
	int soldierType;
	Sprite flying;
public:
	Aerial(double, double);
	void move(const int, char**, int, int, float) override;
	void die() override;
	~Aerial();
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
};

class Paratrooper : public Aerial {
public:
	Paratrooper(double, double);
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
	void setMoves(float, float) override;
};


class Alien : public Enemy {
protected:
	bool phase1;
public:
	void move(const int, char**, int, int, float) override;
	void die() override;
};

class Martian : public Alien {
	Weapon* pistol;
	bool podDestroyed;
	Texture floating;
	float beamCooldown;
public:
	Martian(double, double);
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void fireBeam(float player_x, float player_y, Bullet* bullets, Texture* bulTex);
	~Martian();
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
};

class EnemyVehicle : public Enemy {
public:
	EnemyVehicle() {};
	virtual ~EnemyVehicle() {};
};

class FlyingTara : public EnemyVehicle {
	float cooldown;
public:
	FlyingTara(double x, double y);
	void move(const int, char**, int, int, float) override;
	void die() override;
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
};

class Bradley : public EnemyVehicle {
	RocketLauncher* launcher;
public:
	Bradley(double x, double y);
	void move(const int, char**, int, int, float) override;
	void die() override;
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
	~Bradley();
};

class Sub : public EnemyVehicle {
	RocketLauncher* launcher;
public:
	Sub(double x, double y);
	void move(const int, char**, int, int, float) override;
	void die() override;
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
	~Sub();
};
class IronNokona : public Enemy {
	RocketLauncher* launcher;
	Weapon* flamethrower;
	float cooldown;
	bool going;
public:
	IronNokona(double x, double y);
	void move(const int, char**, int, int, float) override;
	void die() override;
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
	~IronNokona();
};


class HairBuster : public Enemy {
	RocketLauncher* launcher;
	float cooldown;
public:
	HairBuster(double x, double y);
	void move(const int, char**, int, int, float) override;
	void die() override;
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
	~HairBuster();
};

class SeaSatan : public Enemy {
	RocketLauncher* launcher;
	Weapon* gun1;
	Weapon* gun2;
public:
	SeaSatan(double x, double y);
	void move(const int, char**, int, int, float) override;
	void die() override;
	void takeDamage(int d, char* weaponName, float damage_x, bool canPierce = false) override;
	void draw(RenderWindow& window, float camera_x, float camera_y) override;
	void update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*) override;
	~SeaSatan();
};