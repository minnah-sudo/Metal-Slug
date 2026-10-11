#pragma once
class Collectible {//ABSTRACT ABU JEE
protected:
	float posX;
	float posY;
	Sprite sp;
	bool active;
public:
	Collectible();
	void activate(float, float);
	void deactivate();
	bool isActive()const;
	virtual ~Collectible() {}
	virtual void effectPlayer(Character* player) = 0;//every collectible has a different effect
	float getX()const;
	float getY()const;
	virtual void update(char**, float) = 0;//this is for applying gravity and it falling on platform
	Sprite& getSp();
	bool collision(float, float);
};
class Food : public Collectible {//only drops sometimes 
	int replenishAmount;
public:
	void spawn(float, float, Texture*, Texture*);
	void effectPlayer(Character*)override;
	void update(char**, float)override;
};
class Prisoner : public Collectible {
	bool free;
public:
	Prisoner();
	void update(char**, float)override;
	void effectPlayer(Character*)override;
	bool isFree()const;
	void spawn(float, float, Texture*);
};

class SupplyCrate :public Collectible {
	int weaponType;//1 HMG, 2 RocketLauncher, 3 Flame, 4 Laser
	int grenadeCount;//3-5 hand ger, or 1-3 firebomb for eri
	bool isFire;//to check if it has firebombs
public:
	void spawn(float, float, Texture*);
	void update(char**, float)override;
	void effectPlayer(Character*)override;//this is only called when player chooses not to take the weapon
	int getGrenadeC()const;
	bool isBomb()const;
	int getType()const;
};