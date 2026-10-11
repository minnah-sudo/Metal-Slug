#pragma once
#include "Character.h"
#include "Enemy.h"
#include "Weapon.h"
#include "Collectibles.h"
#include "PlayerVehicle.h"
#include <SFML/Window.hpp>
using namespace sf;
class Level {
protected:
	char** lvl;
	const int width;
	const int height;
	const int cell_size;
	Enemy** enemies;
	int enemyCount;
	Sprite blockSprite;
	Texture plainBlockTex;
	Texture aerialBlockTex;
	Texture aquaticBlockTex;
	Texture plainTex;
	Texture aquaticTex;
	Texture aerialTex;
	Sprite bgSprite;
	SpriteData plain;
	SpriteData aquatic;
	SpriteData aerial;
	int plainStart;
	int plainEnd;
	int aquaticStart;
	int aquaticEnd;
	int aerialStart;
	int aerialEnd;
	EnemyVehicle** enemyVehicles;
	int enemyVehicleCount;
	Slug001* mSlug;
	int prisonerAerial;
	int prisonerPlains;
	int prisonerAquatic;
	SupplyCrate* crates;
	Prisoner* prisoners;
	Texture prisTex;
	SlugFlyer* flyer;
	SlugMariner* mariner;
	//start
	AmphibiousSlug* amphSlug;
	//end
public:
	Level();
	EnemyVehicle* getEnemyVehicle(int);
	int getEnemyVehicleCount();
	virtual void drawBg(RenderWindow& window, float camera_x, float camera_y);
	virtual void makeLevel() = 0;
	virtual void drawLevel(RenderWindow& window, float camera_x, float camera_y) = 0;
	virtual void spawnEnemies() = 0;
	void updateEnemies(const int cell_size, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*); // also not virtual 
	void checkEnemyCollisions(Bullet* bullets, Character*, float, Grenade*, RenderWindow&, Texture*, Texture*, Food*, Texture*, Texture*, float, int, Texture*, int&, int&);  // not virtual as it will be same for all levels 
	char** getLvl() const;
	Enemy* getEnemy(int);
	int getEnemyCount();
	int getWidth() const {
		return width;
	}
	int getHeight() const {
		return height;
	}
	virtual ~Level();
	Slug001* getMetalSlug();
	Enemy** getEnemies();
	int getEnCount()const;
	int getPAerial()const;
	int getPAquatic()const;
	int getPPlains()const;
	SlugFlyer* getFlyer();
	SlugMariner* getMariner();
	//start
	AmphibiousSlug* getAmph();
	//end
};

class Level1 : public Level {
public:
	void makeLevel() override;
	void drawLevel(RenderWindow& window, float camera_x, float camera_y) override;
	void spawnEnemies() override;
};

class Level2 : public Level {
	void makeLevel() override;
	void drawLevel(RenderWindow& window, float camera_x, float camera_y) override;
	void spawnEnemies() override;
};

class Level3 : public Level {
	void makeLevel() override;
	void drawLevel(RenderWindow& window, float camera_x, float camera_y) override;
	void spawnEnemies() override;
};

class Game {
	//composition btw
	Character* allCharacters[4];//we want run time polymorphism so make array of pointers
	Character* player;
	int  active;
	bool switched;
	Font gameFont;
	Texture red;
	Sprite redHue;
	Texture bulTex;
	Texture verBul;
	Bullet bullets[30];//total available bullets in the game that any pistol can use aslong as it is not active
	Grenade grenades[15];
	Texture gerTex;
	Texture  poolTex;
	bool grenadeKeyReleased;
	int levelNo;
	float totalWorld_width;
	Texture turkTex;
	Texture fruTex;
	Food foods[10];//array is for no.of foods visible on the screen at the moment
	Texture crateTex;//the no.of crates is ame as no.of prisoners
	Level* level;
	Texture bulSlug;
	Texture verBulSlug;
	FusionCompanion* companion;
	bool compUsed;
	//start
	bool devMode;
	int score;
	int killed;
	Texture liveT;
	Sprite liveSp;
	//end
public:
	Game(int, int);
	void switchCharacter();//called when Z pressed
	~Game();
	void drawPlayer(RenderWindow&, int, float);
	void gamePlay(RenderWindow&, char**, const int, int, int, float&, float&);
	void drawHue(RenderWindow&);
	bool isLevelComplete();
	Character* getPlayer()const;
	Level* getLevel() const;
	int getWidth() const {
		return level->getWidth();
	}
	int getHeight() const {
		return level->getHeight();
	}
	void applyFannumTax(Bullet&);
};
class GameMode {
protected:
	char* name;
	int type;
	bool isCompleted;
};

class SurvivalMode : public GameMode {

};
class BossLevel : public Level {
	void makeLevel() override;
	void drawLevel(RenderWindow& window, float camera_x, float camera_y) override;
	void spawnEnemies() override;
};