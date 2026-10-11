#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "25I-0891_25I-0915_Headers/Game.h"
#include "25I-0891_25I-0915_Headers/Character.h"
#include "25I-0891_25I-0915_Headers/Weapon.h"
#include "25I-0891_25I-0915_Headers/Enemy.h"
#include "25I-0891_25I-0915_Headers/Collectibles.h"
#include <ctime>

using namespace sf;
using namespace std;

int screen_x = 1600;
int screen_y = 900;

//CHARCTER.H STUFF
Character::Character() {
	baseSp = 5.0f; lives = 2; alive = true; jumping = false; running = false;
	isRight = true; curSp = 0.0f; acce = 0.7f; img.scale_x = 0.2f; img.scale_y = 0.2f; img.raw_img_x = 593;
	img.raw_img_y = 470; img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
	gravity = 0.4f; spY = 0.0f;
	healthState[0] = 'H'; healthState[1] = 'E'; healthState[2] = 'A'; healthState[3] = 'L'; healthState[4] = 'T';
	healthState[5] = 'H'; healthState[6] = 'Y'; healthState[7] = '\0'; isTar = false; saturation = 10;
	currentWeapon = new Pistol();//every character starts of with its own pistol
	aim = 0.0f;//default angle is staright horizontal
	posX = 380; posY = 610;//TEMPORARY
	grenades = 10; fireBombs = 0;
	meleeCooldown = 0.0f; inVeh = false; effect = 0; statusTimer = 0.0f; img.mummy.loadFromFile("25I-0891_25I-0915_Assets/PlayerMummy.png");
	isMarc = false; isFio = false;

}
//start
void Character::setAlive(bool a) { alive = a; }
void Character::setLives(int i) { lives = i; }
void Character::setVeh(bool v) { inVeh = v; }
bool Character::getVeh()const { return inVeh; }
int Character::getEffect()const { return effect; }
PlayerData& Character::getImg() { return img; }
void Character::applyEffect(int e) {
	if (inVeh)return;
	effect = e; statusTimer = 10.0f;
	switchWeapSp(-1);//random val send
}
void Character::updateMeleeTimer() { if (meleeCooldown > 0)meleeCooldown -= (1.0f / 60.0f); }
float Character::getMelee()const { return meleeCooldown; }
void Character::setMelee(float m) { meleeCooldown = m; }
//end
bool Character::wallBlocks(char** lvl) {//used so that it doesnt walk through bloacks infront of it
	int col = 0; float x = posX + curSp;
	if (isRight) col = (x + img.width) / 64;
	else col = x / 64;
	if (col < 0 || col >= 180) return true;
	int rowB = (posY + img.height - 1) / 64;//gives row of feet as a block may span more than one row
	if (rowB >= 0 && rowB < 20 && (lvl[rowB][col] == 'g' || lvl[rowB][col] == 'h' || lvl[rowB][col] == 'i'))return true;
	int rowT = posY / 64;//gives row of head
	if (rowT >= 0 && rowT < 20 && (lvl[rowT][col] == 'g' || lvl[rowT][col] == 'h' || lvl[rowT][col] == 'i'))return true;
	return false;

}
void Character::moveX(char** lvl) { if (!wallBlocks(lvl))posX += curSp; else stopRun(); }
void Character::run() {
	running = true;
	if (isRight)curSp = baseSp * 2;//increase speed by 2
	else curSp = -baseSp * 2;
}
void Character::stopRun() { curSp = 0.0f; running = false; }
void Character::right() {
	curSp += acce; isRight = true;
	int max = baseSp;
	if (effect == 1)max = baseSp * 0.5f;//undead halves the speed
	curSp = (curSp > max ? max : curSp);
}
void Character::left() {
	curSp -= acce; isRight = false; int max = baseSp;
	if (effect == 1)max = baseSp * 0.5f;//undead halves the speed
	curSp = (curSp < -max ? -max : curSp);
}
bool Character::isFat()const { return saturation >= 18; }
bool Character::onPlat(char** lvl) {
	int row = (posY + img.height + 2) / 64, col = (posX + img.width) / 64;
	int colLeft = (posX) / 64;
	if (row >= 20) row = 19;
	if (row < 0 || col < 0 || col >= 180)return false;
	if (colLeft < 0 || colLeft >= 180)return false;
	if (lvl[row][col] == 'g' || lvl[row][col] == 'h' || lvl[row][col] == 'i')return true;
	if (lvl[row][colLeft] == 'g' || lvl[row][colLeft] == 'h' || lvl[row][colLeft] == 'i')return true;
	return false;
}
bool Character::isJump()const { return jumping; } // start // end 
void Character::startJump() { jumping = true; spY = -20.0f; }//vertical vel starts as neg so that 
//adding it to position decreaes(moves up) position
void Character::jump(char** lvl) {
	posY += spY;//upwards speed is to increase the posY as spY starts with negative value posY decreases first
	spY += gravity;//due to this speed decreae each time till goes from -ve to +ve or 0 to +ve
	if (dropping && spY > 3.0f) dropping = false;//once it has moved further down the current platform it can land then ->wouldve speed more than 3 by then
	if (!dropping && spY > 0 && this->onPlat(lvl)) {//add spY > 0 check so that when head touches platform it goes through instead of teleporting there
		//must land only when falling falling->spY +ve
		jumping = false;
		spY = 0.0f;
		int row = (posY + img.height) / 64;
		posY = row * 64 - img.height;//so that it lands on exact block 
		//prevent rounding off errors/ truncation errors by using integer divison
	}
}
void Character::startDrop() { jumping = true; dropping = true; spY = 0.0f; }//now vertical vel starts at 0 as we want position to 
//increase(move down) when we add speed to it
float Character::getY()const { return posY; }
float Character::getX()const { return posX; }
void Character::setX(float x) { posX = x; }
void Character::setY(float y) { posY = y; }
bool Character::isAlive()const { return alive; }
int Character::getHP()const { return hp; }
int Character::getLives()const { return lives; }
float Character::getAngle()const { return aim; }
void Character::setAngle(float a) { aim = a; }
Weapon* Character::getWeapon()const { return currentWeapon; }
MarcoRossi::MarcoRossi() {
	img.tex.loadFromFile("25I-0891_25I-0915_Assets/MarcoWalk1.png");
	img.sp.setTexture(img.tex); img.sp.setScale(img.scale_x, img.scale_y);
	img.raw_img_x = 135; img.raw_img_y = 169;
	img.scale_y = 0.6f; img.scale_x = 0.5f;
	img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
	posY = 0 * 64 - img.height; posX = 6 * 64 - img.width;
	grenades = 8;
	img.pistolTex.loadFromFile("25I-0891_25I-0915_Assets/MarcoPistol.png"); img.hmgTex.loadFromFile("25I-0891_25I-0915_Assets/MarcoHmg.png");
	img.rockL.loadFromFile("25I-0891_25I-0915_Assets/MarcoRockL.png"); img.knife.loadFromFile("25I-0891_25I-0915_Assets/MarcoKnife.png");
	img.undead.loadFromFile("25I-0891_25I-0915_Assets/MarcoZombie.png"); isMarc = true; img.fatso.loadFromFile("25I-0891_25I-0915_Assets/MarcoFat.png");
	img.las.loadFromFile("25I-0891_25I-0915_Assets/MarcoLaser.png"); img.flame.loadFromFile("25I-0891_25I-0915_Assets/MarcoFlame.png");
}
void MarcoRossi::switchWeapSp(int type) {
	if (isFat()) {
		img.sp.setTexture(img.fatso, true);
		img.raw_img_x = 290; img.raw_img_y = 268;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (effect == 1) {
		img.sp.setTexture(img.undead, true);
		img.raw_img_x = 333; img.raw_img_y = 329;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (effect == 2) {
		img.sp.setTexture(img.mummy, true);
		img.raw_img_x = 293; img.raw_img_y = 317;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 1) {
		img.sp.setTexture(img.pistolTex, true);//true tells to reset the rectangle according to new texture
		img.raw_img_x = 257; img.raw_img_y = 293;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 2) {
		img.sp.setTexture(img.hmgTex, true);
		img.raw_img_x = 321; img.raw_img_y = 360;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 3) {
		img.sp.setTexture(img.rockL, true);
		img.raw_img_x = 289; img.raw_img_y = 291;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == -1) {//for knife
		img.sp.setTexture(img.knife, true);
		img.raw_img_x = 234; img.raw_img_y = 168;
		img.scale_y = 0.6f; img.scale_x = 0.6f;
	}
	else if (type == 4) {
		img.sp.setTexture(img.flame, true);
		img.raw_img_x = 554; img.raw_img_y = 450;
		img.scale_y = 0.3f; img.scale_x = 0.3f;
	}
	else if (type == 5) {
		img.sp.setTexture(img.las, true);
		img.raw_img_x = 541; img.raw_img_y = 461;
		img.scale_y = 0.3f; img.scale_x = 0.3f;
	}
	img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
}
Tarma::Tarma() {
	img.tex.loadFromFile("25I-0891_25I-0915_Assets/TarmaWalk1.png");
	img.sp.setTexture(img.tex); img.sp.setScale(img.scale_x, img.scale_y);
	img.raw_img_x = 92; img.raw_img_y = 115;
	img.scale_y = 0.8f; img.scale_x = 0.65f;
	img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
	posY = 19 * 64 - img.height; posX = 6 * 64 - img.width;
	isTar = true;
	img.pistolTex.loadFromFile("25I-0891_25I-0915_Assets/TarmaPistol.png"); img.hmgTex.loadFromFile("25I-0891_25I-0915_Assets/TarmaHmg.png");
	img.rockL.loadFromFile("25I-0891_25I-0915_Assets/TarmaRockL.png"); img.knife.loadFromFile("25I-0891_25I-0915_Assets/TarmaKnife.png");
	img.undead.loadFromFile("25I-0891_25I-0915_Assets/TarmaZombie.png"); img.fatso.loadFromFile("25I-0891_25I-0915_Assets/TarmaFat.png");
	img.las.loadFromFile("25I-0891_25I-0915_Assets/TarmaLaser.png"); img.flame.loadFromFile("25I-0891_25I-0915_Assets/TarmaFlame.png");
}
void Tarma::switchWeapSp(int type) {
	if (isFat()) {
		img.sp.setTexture(img.fatso, true);
		img.raw_img_x = 343; img.raw_img_y = 317;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (effect == 1) {
		img.sp.setTexture(img.undead, true);
		img.raw_img_x = 400; img.raw_img_y = 330;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (effect == 2) {
		img.sp.setTexture(img.mummy, true);
		img.raw_img_x = 293; img.raw_img_y = 317;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 1) {
		img.sp.setTexture(img.pistolTex, true);
		img.raw_img_x = 258; img.raw_img_y = 298;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 2) {
		img.sp.setTexture(img.hmgTex, true);
		img.raw_img_x = 293; img.raw_img_y = 335;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 3) {
		img.sp.setTexture(img.rockL, true);
		img.raw_img_x = 313; img.raw_img_y = 316;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == -1) {//for knife
		img.sp.setTexture(img.knife, true);
		img.raw_img_x = 301; img.raw_img_y = 171;
		img.scale_y = 0.6f; img.scale_x = 0.6f;
	}
	else if (type == 4) {
		img.sp.setTexture(img.flame, true);
		img.raw_img_x = 382; img.raw_img_y = 340;
		img.scale_y = 0.3f; img.scale_x = 0.3f;
	}
	else if (type == 5) {
		img.sp.setTexture(img.las, true);
		img.raw_img_x = 549; img.raw_img_y = 454;
		img.scale_y = 0.3f; img.scale_x = 0.3f;
	}
	img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
}
Fiolina::Fiolina() {
	img.tex.loadFromFile("25I-0891_25I-0915_Assets/FioWalk1.png");
	img.sp.setTexture(img.tex); img.sp.setScale(img.scale_x, img.scale_y);
	img.raw_img_x = 88; img.raw_img_y = 144;
	img.scale_y = 0.8f; img.scale_x = 0.6f;
	img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
	posY = 19 * 64 - img.height; posX = 6 * 64 - img.width;
	img.pistolTex.loadFromFile("25I-0891_25I-0915_Assets/FioPistol.png"); img.hmgTex.loadFromFile("25I-0891_25I-0915_Assets/FioHmg.png");
	img.rockL.loadFromFile("25I-0891_25I-0915_Assets/FioRockL.png"); img.knife.loadFromFile("25I-0891_25I-0915_Assets/FioKnife.png");
	img.undead.loadFromFile("25I-0891_25I-0915_Assets/FioZombie.png"); isFio = true; img.fatso.loadFromFile("25I-0891_25I-0915_Assets/FioFat.png");
	img.las.loadFromFile("25I-0891_25I-0915_Assets/FioLaser.png"); img.flame.loadFromFile("25I-0891_25I-0915_Assets/FioFlame.png");
}
void Fiolina::switchWeapSp(int type) {
	if (isFat()) {
		img.sp.setTexture(img.fatso, true);
		img.raw_img_x = 205; img.raw_img_y = 191;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (effect == 1) {
		img.sp.setTexture(img.undead, true);
		img.raw_img_x = 326; img.raw_img_y = 313;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (effect == 2) {
		img.sp.setTexture(img.mummy, true);
		img.raw_img_x = 293; img.raw_img_y = 317;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 1) {
		img.sp.setTexture(img.pistolTex, true);
		img.raw_img_x = 225; img.raw_img_y = 276;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 2) {
		img.sp.setTexture(img.hmgTex, true);
		img.raw_img_x = 195; img.raw_img_y = 248;
		img.scale_y = 0.5f; img.scale_x = 0.4f;
	}
	else if (type == 3) {
		img.sp.setTexture(img.rockL, true);
		img.raw_img_x = 315; img.raw_img_y = 276;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == -1) {//for knife
		img.sp.setTexture(img.knife, true);
		img.raw_img_x = 280; img.raw_img_y = 203;
		img.scale_y = 0.6f; img.scale_x = 0.6f;
	}
	else if (type == 4) {
		img.sp.setTexture(img.flame, true);
		img.raw_img_x = 519; img.raw_img_y = 481;
		img.scale_y = 0.3f; img.scale_x = 0.3f;
	}
	else if (type == 5) {
		img.sp.setTexture(img.las, true);
		img.raw_img_x = 544; img.raw_img_y = 458;
		img.scale_y = 0.3f; img.scale_x = 0.3f;
	}
	img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
}
EriKasamoto::EriKasamoto() {
	img.tex.loadFromFile("25I-0891_25I-0915_Assets/EriWalk1.png");
	img.sp.setTexture(img.tex); img.sp.setScale(img.scale_x, img.scale_y);
	img.raw_img_x = 158; img.raw_img_y = 252;
	img.scale_y = 0.45f; img.scale_x = 0.35f;
	img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
	posY = 19 * 64 - img.height; posX = 6 * 64 - img.width;
	grenades = 20;
	img.pistolTex.loadFromFile("25I-0891_25I-0915_Assets/EriPistol.png"); img.hmgTex.loadFromFile("25I-0891_25I-0915_Assets/EriHmg.png");
	img.rockL.loadFromFile("25I-0891_25I-0915_Assets/EriRockL.png"); img.undead.loadFromFile("25I-0891_25I-0915_Assets/EriZombie.png");
	img.fatso.loadFromFile("25I-0891_25I-0915_Assets/EriFat.png");
	img.las.loadFromFile("25I-0891_25I-0915_Assets/EriLaser.png"); img.flame.loadFromFile("25I-0891_25I-0915_Assets/EriFlame.png");
}
void EriKasamoto::switchWeapSp(int type) {
	if (isFat()) {
		img.sp.setTexture(img.fatso, true);
		img.raw_img_x = 295; img.raw_img_y = 273;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (effect == 1) {
		img.sp.setTexture(img.undead, true);
		img.raw_img_x = 318; img.raw_img_y = 303;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (effect == 2) {
		img.sp.setTexture(img.mummy, true);
		img.raw_img_x = 293; img.raw_img_y = 317;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 1) {
		img.sp.setTexture(img.pistolTex, true);
		img.raw_img_x = 288; img.raw_img_y = 311;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 2) {
		img.sp.setTexture(img.hmgTex, true);
		img.raw_img_x = 299; img.raw_img_y = 295;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 3) {
		img.sp.setTexture(img.rockL, true);
		img.raw_img_x = 324; img.raw_img_y = 305;
		img.scale_y = 0.4f; img.scale_x = 0.3f;
	}
	else if (type == 4) {
		img.sp.setTexture(img.flame, true);
		img.raw_img_x = 529; img.raw_img_y = 472;
		img.scale_y = 0.3f; img.scale_x = 0.3f;
	}
	else if (type == 5) {
		img.sp.setTexture(img.las, true);
		img.raw_img_x = 717; img.raw_img_y = 648;
		img.scale_y = 0.3f; img.scale_x = 0.3f;
	}
	img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
}
void Character::setRight(bool a) { isRight = a; }
bool Character::faceRight()const { return isRight; }
bool areEqual(const char* str1, const char* str2) {
	if (str1[0] == '\0' || str2[0] == '\0')
		return false;
	int i = 0;
	while (*(str1 + i) != '\0' && *(str2 + i) != '\0') {
		if (*(str1 + i) != *(str2 + i)) {
			return false;
		}
		i++;
	}
	if (str1[i] == '\0' && str2[i] == '\0')return true;
	return false;
}
void Character::takeDamage(bool immortal) {
	if (stateFrameCounter > 45 && !areEqual(healthState, "CRITICAL"))return;//this is to prevent one hit counting as more than one
	if (areEqual(healthState, "HEALTHY")) {//after taking one hit it transitions to injured
		if (isTar) {//as Tarma has 20% less hp ot goes staright to critical when hit once
			healthState[0] = 'C'; healthState[1] = 'R'; healthState[2] = 'I'; healthState[3] = 'T'; healthState[4] = 'I';
			healthState[5] = 'C'; healthState[6] = 'A'; healthState[7] = 'L'; healthState[8] = '\0';
		}
		else {
			healthState[0] = 'I'; healthState[1] = 'N'; healthState[2] = 'J'; healthState[3] = 'U'; healthState[4] = 'R';
			healthState[5] = 'E'; healthState[6] = 'D'; healthState[7] = '\0';
		}
		stateFrameCounter = 60;//stays in this state for 1 second
	}
	else if (areEqual(healthState, "INJURED")) {//transition to critical if already injured
		healthState[0] = 'C'; healthState[1] = 'R'; healthState[2] = 'I'; healthState[3] = 'T'; healthState[4] = 'I';
		healthState[5] = 'C'; healthState[6] = 'A'; healthState[7] = 'L'; healthState[8] = '\0';
		stateFrameCounter = 60;//60 frames per second
	}
	else if (areEqual(healthState, "CRITICAL")) {//dies if hit while critical
		lives--;
		if (lives > 0) {
			stateFrameCounter = 120;//2 seconds of invulnerability
			healthState[0] = 'H'; healthState[1] = 'E'; healthState[2] = 'A'; healthState[3] = 'L'; healthState[4] = 'T';
			healthState[5] = 'H'; healthState[6] = 'Y'; healthState[7] = '\0';//respawns with perfect health
			//start
			//posX = 380;posY=610;//respawns to sstarting poition i.e row 11 col 6
			//end
		}
		else if (!immortal)alive = false;//when lives become 0 
	}
}
void Character::updateStateTimer() {
	if (stateFrameCounter > 0)stateFrameCounter--;//decrease this every frame as game shows 60 frames per second
	if (stateFrameCounter == 0 && alive) {//if not hit in the 1 second window and stll alive
		healthState[0] = 'H'; healthState[1] = 'E'; healthState[2] = 'A'; healthState[3] = 'L'; healthState[4] = 'T';
		healthState[5] = 'H'; healthState[6] = 'Y'; healthState[7] = '\0';//goes back to being healthy if not hit in one second
	}
	if (effect != 0) {//when its either undead or a mumy
		statusTimer -= (1.0f / 60.0f);
		if (statusTimer <= 0) {
			effect = 0;
			switchWeapSp(currentWeapon->getType());//to convert back to original sprite
		}
	}
	//bool wasFat = isFat();
	//if (isFat()) {
	//	saturation -= 0.01f;//decrease slowly
	//	if (saturation < 10)saturation = 10;
	//}
	//if (wasFat && !isFat())switchWeapSp(currentWeapon->getType());//if just turned non fat then switch back
}
const char* Character::getHealth() { return healthState; }
void Character::setWeapon(Weapon* w) { currentWeapon = w; }
Character::~Character() {
	delete currentWeapon;//only thing made on heap for character
	currentWeapon = nullptr;
}
void Character::setSaturation(int s) { saturation = s; }
int Character::getSaturation()const { return saturation; }
void Character::setGrenadeCount(int g) { grenades = g; }
void Character::setFireBombCount(int f) { fireBombs = f; }
int Character::getGerC()const { return grenades; }
int Character::getFireC()const { return fireBombs; }
float Character::getBaseSp()const { return baseSp; }
float Character::getAcc()const { return acce; }
bool Character::getTar()const { return isTar; }
bool Character::getMarc()const { return isMarc; }
bool Character::getFio()const { return isFio; }
FusionCompanion::FusionCompanion(Character** src, int count, float x, float y) {
	img.tex.loadFromFile("25I-0891_25I-0915_Assets/FusionComp.png"); img.sp.setTexture(img.tex);
	img.pistolTex.loadFromFile("25I-0891_25I-0915_Assets/FusCompPist.png"); img.hmgTex.loadFromFile("25I-0891_25I-0915_Assets/FusCompHMG.png");
	lifeTime = 30.0f; img.raw_img_x = 451; img.raw_img_y = 553;
	img.scale_y = 0.15f; img.scale_x = 0.15f; img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
	fireRateMult = 1.0f; meleeDamMult = 1.0f; blasMult = 1.0f; igShield = false; noMelee = false;
	float totalSp = 0, totalGer = 0, totalAcc = 0;
	for (int i = 0; i < count; i++) {
		totalSp += src[i]->getBaseSp(); totalGer += src[i]->getGerC(); totalAcc += src[i]->getAcc();
		if (src[i]->getTar())totalSp *= 0.8f;
		else if (src[i]->getMarc() || src[i]->getFio())totalGer -= 2;
	}
	baseSp = totalSp / count; grenades = totalGer / count; acce = totalAcc / count; currentWeapon = new Pistol();
	posX = x; posY = y; curSp = 0.0f; alive = true;
}
void FusionCompanion::switchWeapSp(int type) {
	if (type == 1) {
		img.sp.setTexture(img.pistolTex, true);
		img.raw_img_x = 522; img.raw_img_y = 478;
		img.scale_y = 0.15f; img.scale_x = 0.15f;
	}
	else {
		img.sp.setTexture(img.hmgTex, true);
		img.raw_img_x = 534; img.raw_img_y = 467;
		img.scale_y = 0.15f; img.scale_x = 0.15f;
	}
	img.height = img.raw_img_y * img.scale_y; img.width = img.raw_img_x * img.scale_x;
}
void FusionCompanion::update(char** lvl, float pX, Enemy** enemies, int enCount, Bullet* bullets, Texture* bul, float y) {
	if (lifeTime <= 0) { alive = false; return; }
	lifeTime -= (1.0f / 60.0f);
	//REUSE PLAYER KI MOVEMENT FUNCTIONS
	if (pX - posX > 120.0f)right();
	else if (posX - pX > 120.0f)left();
	else stopRun();
	moveX(lvl);
	//also add jumping if the player is jumping
	if (!jumping) {
		if (posY - y > 100.0f)startJump();//if player is a bit higher than comp it jumps as well
		else if (!onPlat(lvl))startDrop();//applying gravity if it isnt alr jumping
	}
	if (jumping || dropping)jump(lvl);
	//just kills enemies in a straight line infront of it
	currentWeapon->updateTimer();
	for (int i = 0; i < enCount; i++) {
		if (enemies[i]->getAlive()) {
			float dist = abs(posX - enemies[i]->getX()), yD = abs(posY - enemies[i]->getY());
			if (dist <= 300.0f && yD <= 100.0f) {
				isRight = (posX - enemies[i]->getX() < 0 ? true : false);
				if (currentWeapon->canSh()) {
					currentWeapon->shoot(posX, posY, isRight, 0, bullets, bul, false); currentWeapon->resetTimer();
					this->switchWeapSp(currentWeapon->getType());
				}
				break;
			}
		}
	}
}

//GAME.H STUFF
Game::Game(int l, int ch) {
	allCharacters[0] = new MarcoRossi(); allCharacters[1] = new Tarma(); allCharacters[2] = new EriKasamoto();
	allCharacters[3] = new Fiolina(); active = 0;//default character is Marco
	player = allCharacters[0]; switched = false;
	gameFont.loadFromFile("25I-0891_25I-0915_Assets/Fixedsys62.ttf"); red.loadFromFile("25I-0891_25I-0915_Assets/RedHue.png");
	redHue.setTexture(red); redHue.setScale(50.0f, 30.0f);
	bulTex.loadFromFile("25I-0891_25I-0915_Assets/PistolBullet.png");//load texture only once as all have same
	for (int i = 0; i < 30; i++) {
		bullets[i].active = false;
		bullets[i].bul.setTexture(bulTex);
	}
	verBul.loadFromFile("25I-0891_25I-0915_Assets/VerPistol.png");

	// start 
	levelNo = l;
	if (levelNo == 1)
		level = new Level1;  // assifning levels based on the game object init 
	else if (levelNo == 2)
		level = new Level2;
	else if (levelNo == 3)
		level = new Level3;
	else if (levelNo == 4)
		level = new BossLevel;
	level->makeLevel();
	level->spawnEnemies();
	// end
	gerTex.loadFromFile("25I-0891_25I-0915_Assets/Grenade.png");
	for (int i = 0; i < 15; i++) { grenades[i].setActive(false); grenades[i].addTex(&gerTex); }
	poolTex.loadFromFile("25I-0891_25I-0915_Assets/PoolFire.png"); grenadeKeyReleased = true;
	turkTex.loadFromFile("25I-0891_25I-0915_Assets/Turkey.png"); fruTex.loadFromFile("25I-0891_25I-0915_Assets/Fruits.png");
	crateTex.loadFromFile("25I-0891_25I-0915_Assets/SupplyReward.png");
	verBulSlug.loadFromFile("25I-0891_25I-0915_Assets/verBulSlug.png"); 
	bulSlug.loadFromFile("25I-0891_25I-0915_Assets/bulSlug.png"); compUsed = false;
	companion = nullptr; if (ch == 2)devMode = true; else devMode = false;
	liveT.loadFromFile("25I-0891_25I-0915_Assets/LiveSp.png"); liveSp.setTexture(liveT); liveSp.setScale(0.2f, 0.2f);
}
void BossLevel::makeLevel()
{
	for (int i = 0; i < 60; i++)
	{
		lvl[height - 1][i] = 'g';
	}
	for (int i = 60; i < 120; i++)
	{
		lvl[height - 1][i] = 'h';
	}
	for (int i = 120; i < width; i++)
	{
		lvl[height - 1][i] = 'i';
	}
	mSlug = new Slug001();
	mSlug->spawn(29 * 64, 19 * 64 - (303 * 0.4f));
	flyer = new SlugFlyer();
	flyer->spawn(126 * 64, 11 * 64 - (245 * 0.4f));
	mariner = new SlugMariner();
	mariner->spawn(65 * 64, 18 * 64 - (0.3f * 484));
	prisonerAerial = 1; prisonerAquatic = 1; prisonerPlains = 1;
	crates = new SupplyCrate[prisonerAerial + prisonerAquatic + prisonerPlains];
	prisoners = new Prisoner[prisonerAerial + prisonerAquatic + prisonerPlains];
	prisoners[0].spawn(128 * 64, 13 * 64 - (134), &prisTex);//aerial
	prisoners[1].spawn(31 * 64, 11 * 64 - (134), &prisTex);//plains
	prisoners[2].spawn(90 * 64, 11 * 64 - (134), &prisTex);//aquatc
	amphSlug = new AmphibiousSlug();
	amphSlug->spawn(57 * 64, 16 * 64 - (370 * 0.3f));
}

void BossLevel::drawLevel(RenderWindow& window, float camera_x, float camera_y)
{
	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{
			if (lvl[i][j] == 'g')
				blockSprite.setTexture(plainBlockTex);
			else if (lvl[i][j] == 'h')
				blockSprite.setTexture(aquaticBlockTex);
			else if (lvl[i][j] == 'i')
				blockSprite.setTexture(aerialBlockTex);
			else
				continue;

			blockSprite.setPosition(j * cell_size - camera_x, i * cell_size - camera_y);
			window.draw(blockSprite);
		}
	}
}

void BossLevel::spawnEnemies()
{
	enemyCount = 3;
	enemies = new Enemy * [enemyCount];
	int count = 0;

	// plains 
	enemies[count] = new IronNokona(0, 0);
	enemies[count]->setPosition(12 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new HairBuster(0, 0);
	enemies[count]->setPosition(68 * 64, (height - 5) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new SeaSatan(0, 0);
	enemies[count]->setPosition(135 * 64, (height - 10) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);
}
Level* Game::getLevel() const { return level; }
int Level::getPAerial()const { return prisonerAerial; }
int Level::getPAquatic()const { return prisonerAquatic; }
int Level::getPPlains()const { return prisonerPlains; }
void Game::applyFannumTax(Bullet& bull) {
	if (!bull.fromEnemy || !bull.active)return;
	float ith = player->getX() - bull.posX, jth = player->getY() - bull.posY;
	float dist = sqrt(ith * ith + jth * jth);
	if (dist < 300) {//if bullet within radius of 300 pixels apply force on it
		//to curve the bullet add the difference of theur possitions to know which direction to move in
		//divide the diff by the magnitude so that the value added isnt v big
		bull.velX += (ith / dist) * 0.5f;//multiply by 0.1 to make it a smooth curve
		bull.velY += (jth / dist) * 0.5f;
	}
}
Character* Game::getPlayer()const { return player; }
Enemy** Level::getEnemies() { return enemies; }
int Level::getEnCount()const { return enemyCount; }
SlugFlyer* Level::getFlyer() { return flyer; }
SlugMariner* Level::getMariner() { return mariner; }
Slug001* Level::getMetalSlug() { return mSlug; }
bool Game::isLevelComplete()
{
	for (int i = 0; i < level->getEnemyCount(); i++)
	{
		if (level->getEnemy(i) != nullptr && level->getEnemy(i)->getAlive()) // returns false if any of the enemies is alive
			// level is completed if all enemies are dead 
		{
			return false;
		}
	}
	for (int i = 0; i < level->getEnemyVehicleCount(); i++)
	{
		if (level->getEnemyVehicle(i) != nullptr && level->getEnemyVehicle(i)->getAlive())
			return false;
	}
	return true;
}
Level::Level() : width(180), height(20), cell_size(64)
{
	lvl = new char* [height];
	for (int i = 0; i < height; i++)
	{
		lvl[i] = new char[width];
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			lvl[i][j] = ' ';
		}
	}
	enemyCount = 0;
	enemyVehicleCount = 0;
	enemies = nullptr;
	enemyVehicles = nullptr;
	plain.raw_img_x = 735;
	plain.raw_img_y = 490;
	aquatic.raw_img_x = 658;
	aquatic.raw_img_y = 317;
	aerial.raw_img_x = 750;
	aerial.raw_img_y = 417;
	plain.width = plain.scale_x * plain.raw_img_x;
	plain.height = plain.scale_y * plain.raw_img_y;
	aquatic.width = aquatic.scale_x * aquatic.raw_img_x;
	aquatic.height = aquatic.scale_y * aquatic.raw_img_y;
	aerial.width = aerial.scale_x * aerial.raw_img_x;
	aerial.height = aerial.scale_y * aerial.raw_img_y;
	plainTex.loadFromFile("25I-0891_25I-0915_Assets/plTemp.png");
	aquaticTex.loadFromFile("25I-0891_25I-0915_Assets/aquatic.png");
	aerialTex.loadFromFile("25I-0891_25I-0915_Assets/temp.png");
	plainBlockTex.loadFromFile("25I-0891_25I-0915_Assets/plB.png");
	aerialBlockTex.loadFromFile("25I-0891_25I-0915_Assets/temp2.png");
	aquaticBlockTex.loadFromFile("25I-0891_25I-0915_Assets/aquaticBlock.png");
	plainStart = 0;
	plainEnd = (width / 3) * cell_size;
	aquaticStart = plainEnd;
	aquaticEnd = ((2 * width) / 3) * cell_size;
	aerialStart = aquaticEnd;
	aerialEnd = (width - 1) * cell_size;
	prisTex.loadFromFile("25I-0891_25I-0915_Assets/Prisoner.png"); prisoners = nullptr; crates = nullptr;
}

EnemyVehicle* Level::getEnemyVehicle(int i)
{
	return enemyVehicles[i];
}

int Level::getEnemyVehicleCount()
{
	return enemyVehicleCount;
}

Level::~Level()
{
	if (enemies != nullptr)
	{
		for (int i = 0; i < enemyCount; i++)
		{
			if (enemies[i] != nullptr) {
				delete enemies[i];
				enemies[i] = nullptr;
			}
		}
		delete[] enemies;
		enemies = nullptr;
	}

	if (enemyVehicles != nullptr)
	{
		for (int i = 0; i < enemyVehicleCount; i++)
		{
			delete enemyVehicles[i];
			enemyVehicles[i] = nullptr;
		}
		delete[] enemyVehicles;
		enemyVehicles = nullptr;
	}

	if (lvl != nullptr)
	{
		for (int i = 0; i < height; i++)
		{
			delete[] lvl[i];
			lvl[i] = nullptr;
		}
		delete[] lvl;
		lvl = nullptr;
	}
	if (mSlug != nullptr)delete mSlug;
	if (flyer != nullptr)delete flyer;
	if (mariner != nullptr)delete mariner;
	if (prisoners != nullptr)delete[] prisoners; if (crates != nullptr)delete[] crates;
}

void Level::drawBg(RenderWindow& window, float camera_x, float camera_y)
{
	// for plains 
	bgSprite.setTexture(plainTex);
	float start = 0 * cell_size;
	float end = (width / 3) * cell_size; // firt third of the screen 
	plain.scale_x = (float)1280 / plain.raw_img_y;
	plain.scale_y = (float)1280 / plain.raw_img_y;  // screen_y = 1280
	bgSprite.setScale(plain.scale_x, plain.scale_y);

	for (float i = start; i < end; i += 1600) // screen_x = 1600
	{
		bgSprite.setPosition(i - camera_x, -camera_y * 0.3f); // this makes sure the image is covering screen and if more is required  then it places image again 
		window.draw(bgSprite);
	}

	// for aquatic 
	bgSprite.setTexture(aquaticTex);
	start = width / 3 * cell_size;
	end = (2 * width / 3) * cell_size;
	aquatic.scale_x = (float)1280 / aquatic.raw_img_y;
	aquatic.scale_y = (float)1280 / aquatic.raw_img_y;

	bgSprite.setScale(aquatic.scale_x, aquatic.scale_y);
	for (float i = start; i < end; i += 1600) // screen_x = 1600
	{
		bgSprite.setPosition(i - camera_x, -camera_y * 0.3f); // this makes sure the image is covering screen and if more is required  then it places image again 
		window.draw(bgSprite);
	}

	// for aerial 
	bgSprite.setTexture(aerialTex);
	start = (2 * width / 3) * cell_size;
	end = width * cell_size;
	aerial.scale_x = (float)1280 / aerial.raw_img_y;
	aerial.scale_y = (float)1280 / aerial.raw_img_y;

	bgSprite.setScale(aerial.scale_x, aerial.scale_y);
	for (float i = start; i < end; i += 1600) // screen_x = 1600
	{
		bgSprite.setPosition(i - camera_x, -camera_y * 0.3f); // this makes sure the image is covering screen and if more is required  then it places image again 
		window.draw(bgSprite);
	}
}
AmphibiousSlug* Level::getAmph() { return amphSlug; }
// start 
void Level1::makeLevel()
{
	int pStart = 0, pEnd = 60, aqStart = 60, aqEnd = 120, arStart = 120, arEnd = width;

	// plains 
	for (int i = pStart; i < pEnd; i++)
	{
		lvl[height - 1][i] = 'g';
	}
	for (int i = 10; i < 13; i++)
	{
		lvl[height - 2][i] = 'g';
		lvl[height - 3][i] = 'g';
	}
	for (int i = 22; i < 25; i++)
	{
		lvl[height - 2][i] = 'g';
		lvl[height - 3][i] = 'g';
		lvl[height - 4][i] = 'g';
		lvl[height - 5][i] = 'g';
	}
	for (int i = 28; i < 34; i++)
	{
		lvl[height - 9][i] = 'g';
	}
	for (int i = 35; i < 40; i++)
	{
		lvl[height - 2][i] = 'g';
	}
	for (int i = 42; i < 50; i++)
	{
		lvl[height - 6][i] = 'g';
	}
	for (int i = 56; i < 60; i++)
	{
		lvl[height - 2][i] = 'g';
		lvl[height - 3][i] = 'g';
	}

	// aquatic 
	for (int i = aqStart; i < aqEnd; i++)
	{
		lvl[height - 1][i] = 'h';
	}
	for (int i = aqStart + 10; i < aqStart + 13; i++)
	{
		lvl[height - 2][i] = 'h';
		lvl[height - 3][i] = 'h';
	}
	for (int i = aqStart + 22; i < aqStart + 25; i++)
	{
		lvl[height - 2][i] = 'h';
		lvl[height - 3][i] = 'h';
	}
	for (int i = aqStart + 28; i < aqStart + 35; i++)
	{
		lvl[height - 9][i] = 'h';
	}
	for (int i = aqStart + 56; i < aqStart + 60; i++)
	{
		lvl[height - 2][i] = 'h';
		lvl[height - 3][i] = 'h';
	}

	// aerial 
	for (int i = arStart; i < arEnd; i++)
	{
		lvl[height - 1][i] = 'i';
	}
	// peaks
	for (int i = arStart + 0; i < arStart + 14; i++)
	{
		lvl[height - 3][i] = 'i';
	}
	for (int i = arStart + 2; i < arStart + 12; i++)
	{
		lvl[height - 5][i] = 'i';
	}
	for (int i = arStart + 4; i < arStart + 10; i++)
	{
		lvl[height - 7][i] = 'i';
	}
	for (int i = arStart + 6; i < arStart + 8; i++)
	{
		lvl[height - 9][i] = 'i';
	}

	for (int i = arStart + 18; i < arStart + 26; i++)
	{
		lvl[height - 13][i] = 'i';
	}

	// peak 2 
	for (int i = arStart + 35; i < arStart + 45; i++)
	{
		lvl[height - 4][i] = 'i';
	}
	for (int i = arStart + 33; i < arStart + 46; i++)
	{
		lvl[height - 6][i] = 'i';
	}

	for (int i = arStart + 36; i < arStart + 44; i++)
	{
		lvl[height - 8][i] = 'i';
	}

	for (int i = arStart + 38; i < arStart + 42; i++)
	{
		lvl[height - 10][i] = 'i';
	}

	for (int i = arStart + 50; i < arStart + 58; i++)
	{
		lvl[height - 2][i] = 'i';
		lvl[height - 3][i] = 'i';
	}

	mSlug = new Slug001();
	mSlug->spawn(29 * 64, 19 * 64 - (303 * 0.4f));
	flyer = new SlugFlyer();
	flyer->spawn(126 * 64, 11 * 64 - (245 * 0.4f));
	mariner = new SlugMariner();
	mariner->spawn(65 * 64, 18 * 64 - (0.3f * 484));
	prisonerAerial = 1; prisonerAquatic = 1; prisonerPlains = 1;
	crates = new SupplyCrate[prisonerAerial + prisonerAquatic + prisonerPlains];
	prisoners = new Prisoner[prisonerAerial + prisonerAquatic + prisonerPlains];
	prisoners[0].spawn(128 * 64, 13 * 64 - (134), &prisTex);//aerial
	prisoners[1].spawn(31 * 64, 11 * 64 - (134), &prisTex);//plains
	prisoners[2].spawn(90 * 64, 11 * 64 - (134), &prisTex);//aquatc
	amphSlug = new AmphibiousSlug();
	amphSlug->spawn(57 * 64, 16 * 64 - (370 * 0.3f));
}
// end 

void Level1::drawLevel(RenderWindow& window, float camera_x, float camera_y)
{
	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{
			if (lvl[i][j] == 'g')
				blockSprite.setTexture(plainBlockTex);
			else if (lvl[i][j] == 'h')
				blockSprite.setTexture(aquaticBlockTex);
			else if (lvl[i][j] == 'i')
				blockSprite.setTexture(aerialBlockTex);
			else
				continue;

			blockSprite.setPosition(j * cell_size - camera_x, i * cell_size - camera_y);
			window.draw(blockSprite);
		}
	}
}

void Level::updateEnemies(const int cell_size, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	for (int i = 0; i < enemyCount; i++)
	{
		if (enemies[i] != nullptr)
			enemies[i]->update(cell_size, lvl, width, height, window, player_x, player_y, camera_x, bullets, bulTex, grenades, greTex, camera_y, player);
	}

	for (int i = 0; i < enemyVehicleCount; i++)
	{
		if (enemyVehicles[i] != nullptr && enemyVehicles[i]->getAlive())
			enemyVehicles[i]->update(cell_size, lvl, width, height, window, player_x, player_y, camera_x, bullets, bulTex, grenades, greTex, camera_y, player);
	}
}

void Level::checkEnemyCollisions(Bullet* bullets, Character* player, float camera_x, Grenade* grenades, RenderWindow& window
	, Texture* gerTex, Texture* poolTex, Food* foods, Texture* turkTex, Texture* fruTex, float camera_y, int active, Texture* crateTex
	, int& score, int& killed) {
	for (int i = 0; i < 30; i++) {
		if (bullets[i].active) {
			bullets[i].moveBullet(lvl, camera_x, camera_y);//all bullets are separate entities
			if (!bullets[i].active && bullets[i].isRocket && !bullets[i].fromEnemy) {//if it os set to inactive in func i.e it hit a wall and it explodes
				for (int j = 0; j < enemyCount; j++)
					if (enemies[j]->getAlive()) {
						float xDist = abs(enemies[j]->getX() - bullets[i].posX), yDist = abs(enemies[j]->getY() - bullets[i].posY);
						float dist = xDist * xDist + yDist * yDist;
						if (dist <= 192 * 192)enemies[j]->takeDamage(bullets[i].damage, bullets[i].sourceName, bullets[i].posX);
						if (enemies[j]->getHp() <= 0) {
							killed++; score += enemies[j]->getScore();
						}
					}
				for (int j = 0; j < enemyVehicleCount; j++)
					if (enemyVehicles[j]->getAlive()) {
						float xDist = abs(enemyVehicles[j]->getX() - bullets[i].posX), yDist = abs(enemyVehicles[j]->getY() - bullets[i].posY);
						float dist = xDist * xDist + yDist * yDist;
						if (dist <= 192 * 192)enemyVehicles[j]->takeDamage(bullets[i].damage, bullets[i].sourceName, bullets[i].posX);
					}
			}
		}
	}
	// checking collision between bullets and enemies to call takedamage()
	for (int i = 0; i < 30; i++) {
		if (bullets[i].active && !bullets[i].fromEnemy) {// if coming from player and active
			for (int j = 0; j < enemyCount; j++)
				if (enemies[j]->getAlive()) {
					if (bullets[i].isLaser) {//for laser DOESNOT REQUIRE COLLISION
						//check if it is on the same level (y) as the beam
						if (abs(bullets[i].posY - enemies[j]->getY()) <= 80) {
							if (player->faceRight()) {//enemy should be in front of it
								if (enemies[j]->getX() >= player->getX())enemies[j]->die();
							}
							else if (enemies[j]->getX() <= player->getX())enemies[j]->die();
							score += enemies[j]->getScore();
						}
						continue;
					}
					int hor_dist = abs(enemies[j]->getX() - bullets[i].posX);
					int ver_dist = abs(enemies[j]->getY() - bullets[i].posY);
					if (hor_dist <= 25 && ver_dist <= 25) {// attack range = 5 
						if (bullets[i].isRocket) {//for rocketLauncher
							for (int k = 0; k < enemyCount; k++)//check all enemies after hitting target
								if (enemies[k]->getAlive()) {
									float xDist = abs(enemies[k]->getX() - bullets[i].posX), yDist = abs(enemies[k]->getY() - bullets[i].posY);
									float dist = xDist * xDist + yDist * yDist;//dist is hypotemuse
									if (dist <= (192 * 192)) //192 bcs 3*64 as radius is 3 pixels
										enemies[k]->takeDamage(bullets[i].damage, bullets[i].sourceName, bullets[i].posX);
									if (enemies[k]->getHp() <= 0) {
										killed++; score += enemies[k]->getScore();
									}
								}
							bullets[i].active = false;
							break;//one bullet needs to hit one enemy so now move on to next bullet
						}
						else if (bullets[i].isFlame) {//for flamehsot
							if (enemies[j]->getBurnTime() <= 0.0f) {
								enemies[j]->takeDamage(bullets[i].damage, bullets[i].sourceName, bullets[i].posX);
								enemies[j]->setBurnTime(1.0f);
							}//no break; as it can hit more than one enemy at once
							//else enemies[j]->setBurnTime(enemies[j]->getBurnTime() - 1.0f / 60.0f);
							if (enemies[j]->getHp() <= 0) {
								score += enemies[j]->getScore();
							}
						}
						else { //for pistol and hmg
							enemies[j]->takeDamage(bullets[i].damage, bullets[i].sourceName, bullets[i].posX); // this gives damage to the enemy on collision with the bullet 
							bullets[i].active = false;
							if (enemies[j]->getHp() <= 0) {
								score += enemies[j]->getScore();
							}
							break;//one bullet needs to hit one enemy so now move on to next bullet
						}
					}
					else if (bullets[i].isFlame)enemies[j]->setBurnTime(0.0f);//when it moves out range reset timer
				}
		}
	}
	//checking collisions between enemy vehicles and bullets to give damage
	for (int i = 0; i < 30; i++) {
		if (bullets[i].active && !bullets[i].fromEnemy) {// if coming from player and active
			for (int j = 0; j < enemyVehicleCount; j++)
				if (enemyVehicles[j]->getAlive()) {
					if (bullets[i].isLaser) {//for laser DOESNOT REQUIRE COLLISION
						//check if it is on the same level (y) as the beam
						if (abs(bullets[i].posY - enemyVehicles[j]->getY()) <= 80) {
							if (player->faceRight()) {//enemy should be in front of it
								if (enemyVehicles[j]->getX() >= player->getX())enemyVehicles[j]->die();
							}
							else if (enemyVehicles[j]->getX() <= player->getX())enemyVehicles[j]->die();
						}
						continue;
					}
					int hor_dist = abs(enemyVehicles[j]->getX() - bullets[i].posX);
					int ver_dist = abs(enemyVehicles[j]->getY() - bullets[i].posY);
					if (hor_dist <= 25 && ver_dist <= 25) {// attack range = 5 
						if (bullets[i].isRocket) {//for rocketLauncher
							for (int k = 0; k < enemyVehicleCount; k++)//check all enemies after hitting target
								if (enemyVehicles[k]->getAlive()) {
									float xDist = abs(enemyVehicles[k]->getX() - bullets[i].posX), yDist = abs(enemyVehicles[k]->getY() - bullets[i].posY);
									float dist = xDist * xDist + yDist * yDist;//dist is hypotemuse
									if (dist <= (192 * 192)) //192 bcs 3*64 as radius is 3 pixels
										enemyVehicles[k]->takeDamage(bullets[i].damage, bullets[i].sourceName, bullets[i].posX);
								}
							bullets[i].active = false;
							break;//one bullet needs to hit one enemy so now move on to next bullet
						}
						else if (bullets[i].isFlame) {//for flamehsot
							if (enemyVehicles[j]->getBurnTime() <= 0.0f) {
								enemyVehicles[j]->takeDamage(bullets[i].damage, bullets[i].sourceName, bullets[i].posX);
								enemyVehicles[j]->setBurnTime(1.0f);
							}//no break; as it can hit more than one enemy at once
							//else enemies[j]->setBurnTime(enemies[j]->getBurnTime() - 1.0f / 60.0f);
						}
						else { //for pistol and hmg
							enemyVehicles[j]->takeDamage(bullets[i].damage, bullets[i].sourceName, bullets[i].posX); // this gives damage to the enemy on collision with the bullet 
							bullets[i].active = false;
							break;//one bullet needs to hit one enemy so now move on to next bullet
						}
					}
					else if (bullets[i].isFlame)enemyVehicles[j]->setBurnTime(0.0f);//when it moves out range reset timer
				}
		}
	}
	//GRENADES
	for (int i = 0; i < 15; i++)
		if (grenades[i].isActive()) {
			grenades[i].update(lvl);
			if (grenades[i].getFromEnemy())  // so grenade skips enemies 
			{
				if (grenades[i].getExploded())grenades[i].addTex(poolTex);
				else grenades[i].addTex(gerTex);
				grenades[i].getSp().setPosition(grenades[i].getX() - camera_x, grenades[i].getY() - camera_y);
				window.draw(grenades[i].getSp());
				continue;
			}
			if (!grenades[i].getExploded()) {
				for (int j = 0; j < enemyCount; j++) {
					if (enemies[j]->getAlive() && enemies[j] != grenades[i].getOwner()) {
						float xDist = abs(enemies[j]->getX() - grenades[i].getX()), yDist = abs(enemies[j]->getY() - grenades[i].getY());
						if (xDist <= 30 && yDist <= 30) {
							grenades[i].collided(); break;
						}
					}
				}
			}
			if (grenades[i].getExploded()) {
				for (int j = 0; j < enemyCount; j++) {
					if (enemies[j]->getAlive() && enemies[j] != grenades[i].getOwner()) { // dont attack itself 
						float xDist = abs(enemies[j]->getX() - grenades[i].getX()), yDist = abs(enemies[j]->getY() - grenades[i].getY());
						float dist = xDist * xDist + yDist * yDist;
						if (dist <= (192 * 192)) {
							if (grenades[i].getPoolTime() <= (1.0f / 60.0f)) {//this means that it just exploded and only one frame counted above in upadte()
								enemies[j]->takeDamage(5, (char*)"Grenade", grenades[i].getX());//immediately upon explosion both grenades act the same
								if (!grenades[i].getFire())grenades[i].setActive(false);//normal grenade disaapears after explosion
							}
							else {//poolTimer only increases for fireBomb
								if (enemies[j]->getBurnTime() <= 0) {
									enemies[j]->takeDamage(2, (char*)"FlameShot", grenades[i].getX());
									enemies[j]->setBurnTime(1.0f);
								}
							}
							if (enemies[j]->getHp() <= 0) {
								killed++; score += enemies[j]->getScore();
							}
						}
					}
				}
			}
			if (grenades[i].getExploded())grenades[i].addTex(poolTex);
			else grenades[i].addTex(gerTex);
			grenades[i].getSp().setPosition(grenades[i].getX() - camera_x, grenades[i].getY() - camera_y);
			window.draw(grenades[i].getSp());
		}
	//FOOD COLLECTIBLES
	for (int i = 0; i < enemyCount; i++) {
		if (enemies[i]->getHp() <= 0 && !enemies[i]->getDropped()) {
			enemies[i]->setDropped(true);
			int prob = (rand() % 100);//randomly get any number from 0 - 99
			if (prob <= 30) //food dropping has a 30% chance onl when enemy dies
				for (int j = 0; j < 10; j++) //find any inactive food and spawn it
					if (!foods[j].isActive()) {
						foods[j].spawn(enemies[i]->getX(), enemies[i]->getY(), turkTex, fruTex);
						break;
					}
		}
	}
	//POW PRISONER
	for (int i = 0; i < prisonerAquatic + prisonerAerial + prisonerPlains; i++) {
		if (prisoners[i].isActive()) {
			prisoners[i].update(lvl, camera_x);
			prisoners[i].getSp().setPosition(prisoners[i].getX() - camera_x, prisoners[i].getY() - camera_y);
			window.draw(prisoners[i].getSp());
			if (prisoners[i].collision(player->getX(), player->getY()) && Keyboard::isKeyPressed(Keyboard::E)) {//press E to interact w prisoner
				prisoners[i].effectPlayer(player);
				//now spawn a crate at that spot
				crates[i].spawn(prisoners[i].getX(), prisoners[i].getY(), crateTex);//ith crate bcs every prisoner has its own crate
			}
		}
	}
	//SUPPLY CRATE
	for (int i = 0; i < prisonerAquatic + prisonerAerial + prisonerPlains; i++)
		if (crates[i].isActive()) {
			crates[i].update(lvl, camera_x);
			crates[i].getSp().setPosition(crates[i].getX() - camera_x, crates[i].getY() - camera_y);
			window.draw(crates[i].getSp());
			if (crates[i].collision(player->getX(), player->getY())) {
				if (Keyboard::isKeyPressed(Keyboard::I)) {//press I to replace weapon
					delete player->getWeapon();
					if (crates[i].getType() == 1) player->setWeapon(new HMG());
					else if (crates[i].getType() == 2) player->setWeapon(new RocketLauncher());
					else if (crates[i].getType() == 3) player->setWeapon(new FlameShot());
					else if (crates[i].getType() == 4) player->setWeapon(new LaserGun());
					if (active != 2 && crates[i].isBomb())player->setFireBombCount(player->getFireC() + crates[i].getGrenadeC());
					else player->setGrenadeCount(player->getGerC() + crates[i].getGrenadeC());//eri uses grenade count all the time 
					crates[i].deactivate();
				}
				else if ((player->getX() - crates[i].getX()) >= 25) {//if player moves too right w/o interacting
					if (active != 2 && crates[i].isBomb())player->setFireBombCount(player->getFireC() + crates[i].getGrenadeC());
					else player->setGrenadeCount(player->getGerC() + crates[i].getGrenadeC());//eri's fire count is supposed to remain at 0 throughout
					crates[i].effectPlayer(player);//only replenishes ammo
				}
			}
		}
}

char** Level::getLvl() const
{
	return lvl;
}

Enemy* Level::getEnemy(int i)
{
	return enemies[i];
}

int Level::getEnemyCount()
{
	return enemyCount;
}

// start
void Level1::spawnEnemies()
{
	enemyCount = 26;
	enemies = new Enemy * [enemyCount];
	int count = 0;

	// plains 
	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(12 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(18 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new ShieldedSoldier(0, 0);
	enemies[count]->setPosition(26 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new GrenadeSoldier(0, 0);
	enemies[count]->setPosition(30 * 64, (height - 9) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new BazookaSoldier(0, 0);
	enemies[count]->setPosition(30 * 64, (height - 6) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new MummyWarrior(0, 0);
	enemies[count]->setPosition(48 * 64, (height - 6) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new MummyWarrior(0, 0);
	enemies[count]->setPosition(54 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(50 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(52 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(55 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	// aquatic 
	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(66 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new ShieldedSoldier(0, 0);
	enemies[count]->setPosition(82 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(88 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(92 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(96 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(107 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(110 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(91 * 64, (height - 9) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	// aerial 
	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(132 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new GrenadeSoldier(0, 0);
	enemies[count]->setPosition(138 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new BazookaSoldier(0, 0);
	enemies[count]->setPosition(142 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(130 * 64, (height - 13) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(136 * 64, (height - 13) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(146 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(162 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Martian(0, 0);
	enemies[count]->setPosition(162 * 64, (height - 15) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	// enemy vehicles 
	enemyVehicleCount = 4;
	enemyVehicles = new EnemyVehicle * [enemyVehicleCount];
	int vcount = 0;

	enemyVehicles[vcount] = new Bradley(plainStart + 1800, (height - 1) * 64 - 147);
	enemyVehicles[vcount++]->setMoves(plainStart, plainEnd);

	enemyVehicles[vcount] = new Sub(100 * 64, (height - 5) * 64 - 112);
	enemyVehicles[vcount++]->setMoves(aquaticStart, aquaticEnd);

	enemyVehicles[vcount] = new FlyingTara(aerialStart + 500, 250);
	enemyVehicles[vcount++]->setMoves(aerialStart, aerialEnd);

	enemyVehicles[vcount] = new FlyingTara(aerialStart + 1750, 220);
	enemyVehicles[vcount++]->setMoves(aerialStart, aerialEnd);
}
// end 

void Game::switchCharacter() {
	float ogX = player->getX(), ogY = player->getY();//save prev coords
	int old = active;
	bool r = player->faceRight();
	active++; active = (active > 3 ? 0 : active);
	bool choosen = false; int k = 0;
	bool oldVeh = player->getVeh();
	while (!choosen && k < 4) {//check if chosen character is alive
		k++;
		if (allCharacters[active]->isAlive())choosen = true;
		else {
			active++; active = (active > 3 ? 0 : active);
		}
	}
	if (!choosen) {
		active = old; return;
	}
	player = allCharacters[active];//switch to next 
	player->setX(ogX); player->setY(ogY);
	player->setRight(r); player->setVeh(oldVeh);
}
Game::~Game() {
	for (int i = 0; i < 4; i++)
	{
		delete allCharacters[i];
		allCharacters[i] = nullptr;
	}

	delete level;
	level = nullptr;
}
void Game::drawPlayer(RenderWindow& window, int camera_x, float camera_y) {
	if (!player->isAlive())return;
	player->getImg().sp.setOrigin(player->getImg().raw_img_x / 2.0f, player->getImg().raw_img_y);//set origin to center of the 
	//image so flipping doesnt move it center is where topLeft drawn so need to adjust setPosition
	player->getImg().sp.setPosition(player->getX() - camera_x + (player->getImg().width / 2.0f), player->getY() - camera_y + player->getImg().height);
	//now now origin placed at correct space and not topleft(posX and posY)
	if (!player->faceRight())player->getImg().sp.setScale(-player->getImg().scale_x, player->getImg().scale_y);
	else player->getImg().sp.setScale(player->getImg().scale_x, player->getImg().scale_y);
	if (!player->getVeh())	window.draw(player->getImg().sp);
}
void numToStr(int value, char* str) {
	if (value == 0) {
		str[0] = '0';
		str[1] = '\0';
		return;
	}
	int temp = value;
	int digitCount = 0;
	while (temp > 0) {
		temp /= 10;
		digitCount++;
	}
	str[digitCount] = '\0';
	temp = value;
	for (int i = digitCount - 1; i >= 0; i--) {
		str[i] = (temp % 10) + '0';
		temp /= 10;
	}
}
void Game::drawHue(RenderWindow& window) {
	Text sc; sc.setFont(gameFont); sc.setString("SCORE:");
	sc.setCharacterSize(20); sc.setPosition(screen_x / 2.0f + 300.0f, 20.0f);
	window.draw(sc);
	Text scoreT; char sco[7]; numToStr(score, sco); scoreT.setFont(gameFont);
	scoreT.setString(sco); scoreT.setCharacterSize(25); scoreT.setPosition(screen_x / 2.0f + 370.0f, 14.0f);
	window.draw(scoreT);
	if (!player->isAlive())return;
	for (int i = 0; i < player->getLives(); i++) {
		liveSp.setPosition(20 + (i * 120), 20.0f);
		window.draw(liveSp);
	}
	Text status;
	status.setFont(gameFont);
	status.setString(player->getHealth());
	status.setCharacterSize(20);
	status.setPosition(screen_x / 2.0f - 150.0f, 20.0f);
	window.draw(status);
	Text satStr;
	satStr.setFont(gameFont); satStr.setString("SATURATION:");
	satStr.setCharacterSize(20); satStr.setPosition(screen_x / 2.0f - 10.0f, 20.0f);
	window.draw(satStr);
	char satNum[5]; numToStr(player->getSaturation(), satNum);
	Text sat; sat.setFont(gameFont); sat.setString(satNum);
	sat.setCharacterSize(25); sat.setPosition(screen_x / 2.0f + 120.0f, 14.0f);
	window.draw(sat);
	if (areEqual(player->getHealth(), "HEALTHY")) return;
	window.draw(redHue);
}

//WEAPON.H STUFF
// helper comparison function 
void strCopy(char* dest, const char* src)
{
	int i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

int strCmp(char str[], const char* ptr)
{
	int i = 0;
	while (ptr[i] != '\0' && str[i] != '\0')
	{
		if (ptr[i] < str[i]) return -1;
		if (ptr[i] > str[i]) return 1;
		i++;
	}
	if (ptr[i] == '\0' && str[i] == '\0') return 0;
	if (ptr[i] == '\0') return -1; // if first is shorter it is smaller 

	return 1;
}
void Bullet::moveBullet(char** lvl, float camera_x, float camera_y) {//keeps the bullet moving in the direction it was shot
	if (active) {
		if (isLaser) {
			bul.setPosition(posX - camera_x, posY - camera_y);
			laserTimer += 1.0f / 60.0f;
			if (laserTimer >= 1.0f)  active = false;
			return;
		}
		// start
		if (isBeam)
		{
			posY += velY;
			bul.setPosition(posX - camera_x, posY - camera_y);

			if (posY > screen_y)
			{
				active = false;
			}
			return;
		}
		// end
		if (isRocket)velY += gravity;//vel y is only constant in non projectile motion otherwise gravity adds to the vel y
		posX += velX; posY += velY;
		bul.setPosition(posX - camera_x, posY - camera_y);//adds posX of bullet to velX and posY to velY hence updating x and y of bullet in direction of vel
		if (isFlame) {//cannot remain active for more than 5 blocks
			if (abs(posX - startX) >= 5 * 64 || abs(posY - startY) >= 5 * 64) {
				active = false;                  //for when shooting at 90 deg
				return;
			}
		}
		if (posX > 180 * 64 || posX < 0 || posY > 20 * 64 || posY < 0) {
			active = false;//stops moving when it leaves screen
			return;
		}
		int c = (posX + 5) / 64, r = (posY + 5) / 64;
		if (r >= 0 && r < 20 && c >= 0 && c < 180 && (lvl[r][c] == 'g' || lvl[r][c] == 'h' || lvl[r][c] == 'i'))  active = false;
	}
}
Weapon::Weapon() {
	shootTimer = 0; ammo = 0; infinityAmmo = false; reloadTime = 0; damage = 0; type = 0;
}
int Weapon::getType()const { return type; }
void Weapon::resetTimer() { shootTimer = 0.0; }
void Weapon::updateTimer() {
	if (shootTimer < reloadTime)shootTimer += (1.0f / 60.0f);//adding this in each frame as 60 fps
	if (shootTimer > reloadTime)shootTimer = reloadTime;//clamp
}
bool Weapon::canSh()const { return shootTimer >= reloadTime; }
Pistol::Pistol() {
	infinityAmmo = true; damage = 3; reloadTime = 1.0f / 4.0f;//secs/shots per sec
	shootTimer = reloadTime;//so pistol is ready to shoot at start
	type = 1;
}
void Pistol::shoot(float x, float y, bool right, float ang, Bullet* bullets, Texture* tex, bool fromEnemy) {
	int i = 0;
	while (i < 30 && bullets[i].active)i++;//finds first inactive bullet
	if (i >= 30) return;
	bullets[i].bul.setTexture(*tex, true);
	bullets[i].bul.setScale(0.3f, 0.3f);
	bullets[i].active = true;
	bullets[i].isRocket = false;
	bullets[i].isFlame = false;
	bullets[i].isLaser = false;
	bullets[i].isBeam = false;
	float theta = ang * (3.14159265358979323846f / 180.0f);//covert to rad
	bullets[i].velX = 11.0f * cos(theta);//13 is the launch speed of bullet
	bullets[i].velY = -11.0f * sin(theta);//negative vel to move up
	strCopy(bullets[i].sourceName, "Pistol");
	bullets[i].damage = damage;
	bullets[i].fromEnemy = fromEnemy;
	if (!right) {
		bullets[i].velX = -bullets[i].velX;//moving left needs -ve vel
		bullets[i].bul.setScale(-0.3f, 0.3f);
	}
	bullets[i].posX = x; bullets[i].posY = y + 10;//starting pos is the same as guns
	bullets[i].bul.setPosition(bullets[i].posX, bullets[i].posY);
	bullets[i].gravity = 0.5f;
}
bool Weapon::infAmmo()const { return infinityAmmo; }
HMG::HMG() {
	damage = 3; reloadTime = 1.0f / 8.0f;//2 times shots per sec of pistol
	shootTimer = reloadTime; ammo = 25;//finite ammo unlike pistol
	type = 2;
}
void HMG::shoot(float x, float y, bool right, float ang, Bullet* bullets, Texture* tex, bool fromEnemy) {
	if (ammo <= 0)return;
	ammo--;//ammo decreases everytime the player shoots
	int i = 0;
	while (i < 30 && bullets[i].active) i++;
	if (i >= 30) return;
	bullets[i].bul.setTexture(*tex, true);
	bullets[i].bul.setScale(0.3f, 0.3f);
	bullets[i].active = true;
	bullets[i].isRocket = false;//acts as a bullet
	bullets[i].isFlame = false;
	bullets[i].isLaser = false;
	bullets[i].isBeam = false;
	float theta = ang * (3.14159265358979323846f / 180.0f);
	bullets[i].velX = 14.0f * cos(theta);//slightly faster bullet than pistol
	bullets[i].velY = -14.0f * sin(theta);
	strCopy(bullets[i].sourceName, "HMG");
	bullets[i].damage = damage;
	bullets[i].fromEnemy = fromEnemy;
	if (!right) {
		bullets[i].velX = -bullets[i].velX;
		bullets[i].bul.setScale(-0.3f, 0.3f);
	}
	bullets[i].posX = x; bullets[i].posY = y + 10;
	bullets[i].bul.setPosition(bullets[i].posX, bullets[i].posY);
	bullets[i].gravity = 0.5f;
}
RocketLauncher::RocketLauncher() {
	reloadTime = 2; shootTimer = reloadTime; damage = 5; type = 3; ammo = 10;
}
void RocketLauncher::shoot(float x, float y, bool right, float ang, Bullet* bullets, Texture* tex, bool fromEnemy) {
	if (ammo <= 0)return;
	ammo--;
	int i = 0;
	while (i < 30 && bullets[i].active) i++;
	if (i >= 30) return;
	bullets[i].bul.setTexture(*tex, true);
	bullets[i].bul.setScale(0.3f, 0.3f);
	bullets[i].active = true;
	bullets[i].isRocket = true;//so moveBullet() knows to implement projectile motion for this bullet
	bullets[i].isFlame = false;
	bullets[i].isLaser = false;
	bullets[i].isBeam = false;
	float theta = ang * (3.14159265358979323846f / 180.0f);
	bullets[i].velX = 15.0f * cos(theta);//launch speed more than of HMG & Pistol
	bullets[i].velY = -15.0f * (sin(theta) + 0.5f);//add 3.0 so that even when theta is 0 the vertical vel isnt 0 so that we get some steepness in motion
	strCopy(bullets[i].sourceName, "RocketLauncher");
	bullets[i].damage = damage;
	bullets[i].fromEnemy = fromEnemy;
	if (!right) {
		bullets[i].velX = -bullets[i].velX;
		bullets[i].bul.setScale(-0.3f, 0.3f);
	}
	bullets[i].posX = x; bullets[i].posY = y + 10;
	bullets[i].bul.setPosition(bullets[i].posX, bullets[i].posY);
	bullets[i].gravity = 0.5f;
}
FlameShot::FlameShot() {
	range = 5 * 64;//range is in pixels so as 1 block is 64 pixels range is 5 blocks so 5*64
	damage = 2; reloadTime = 0.005;//vsmall reloadTime o show key trigger
	ammo = 500; shootTimer = reloadTime; type = 4;
}
void FlameShot::shoot(float x, float y, bool right, float ang, Bullet* bullets, Texture* tex, bool fromEnemy) {
	if (ammo <= 0)return;
	ammo--;
	int i = 0;
	while (i < 30 && bullets[i].active) i++;
	if (i >= 30) return;
	bullets[i].bul.setTexture(*tex, true);
	bullets[i].bul.setScale(0.6f, 0.3f);
	bullets[i].active = true;
	bullets[i].isRocket = false;//so moveBullet() knows to implement projectile motion for this bullet
	bullets[i].isFlame = true;
	bullets[i].isLaser = false;
	bullets[i].isBeam = false;
	bullets[i].startX = x;//save starting position used in comparison
	bullets[i].startY = y;
	float theta = ang * (3.14159265358979323846f / 180.0f);
	bullets[i].velX = 9.0f * cos(theta);
	bullets[i].velY = -9.0f * sin(theta);
	strCopy(bullets[i].sourceName, "FlameShot");
	bullets[i].damage = damage;
	bullets[i].fromEnemy = fromEnemy;
	if (!right) {
		bullets[i].velX = -bullets[i].velX;
		bullets[i].bul.setScale(-0.6f, 0.3f);
	}
	bullets[i].posX = x; bullets[i].posY = y + 10;
	bullets[i].bul.setPosition(bullets[i].posX, bullets[i].posY);
}
LaserGun::LaserGun() {
	type = 5; reloadTime = 3;//more reload time as it is v powerful
	ammo = 9;//Dis much power cannot be given to one being 
	shootTimer = reloadTime; beamLength = 0;//temp leng to stop w da annoying warnings
}
void LaserGun::shoot(float x, float y, bool right, float ang, Bullet* bullets, Texture* tex, bool fromEnemy) {
	if (ammo <= 0)return;
	ammo--;
	int i = 0;
	while (i < 30 && bullets[i].active) i++;
	if (i >= 30) return;
	bullets[i].active = true;
	bullets[i].isRocket = false;
	bullets[i].isFlame = false;
	bullets[i].isLaser = true;
	bullets[i].isBeam = false;
	bullets[i].velX = 0;
	bullets[i].velY = 0;
	bullets[i].bul.setTexture(*tex, true);
	beamLength = (right ? screen_x - (x + 20) : x + 20);
	//increase length of the bullet so it reaches end of the screen so scale it
	float scale = (beamLength / tex->getSize().x);//we need a num that when we multiply it w ogSize we get the beam lgeth
	strCopy(bullets[i].sourceName, "LaserGun");
	bullets[i].damage = damage;
	bullets[i].fromEnemy = fromEnemy;
	if (right)bullets[i].bul.setScale(scale, 0.6f);
	else bullets[i].bul.setScale(-scale, 0.6f);
	bullets[i].laserTimer = 0;
	bullets[i].posX = x + 20; bullets[i].posY = y + 30;
	bullets[i].bul.setPosition(bullets[i].posX, bullets[i].posY);
	bullets[i].gravity = 0.5f;
}
Grenade::Grenade() {
	gravity = 0.5f; active = 0; exploded = false; fireBomb = false;
	poolTimer = 0.0f;
	owner = nullptr;
	fromEnemy = false;
}
void Grenade::throwGrenade(float x, float y, bool right, bool isEri) {
	if (isEri) fireBomb = true;//for the pool of fire
	posX = x; posY = y;
	velY = -15.0f;//give it a higher vertical velocity so that goes high before gravity brings it back down
	if (right)velX = 8.0f;//not as high as verti as we want it to go high not far
	else velX = -8.0f;
	active = true; exploded = false;
	ger.setScale(0.5f, 0.5f);
	owner = nullptr;
}
void Grenade::update(char** lvl) {
	if (!active)return;
	if (!exploded) {
		posY += velY;
		velY += gravity;//its projectile motion
		posX += velX;
		//ger.setPosition(posX, posY);
		int c = (posX + 32) / 64, r = (posY + 16) / 64;
		if (r >= 0 && r < 20 && c >= 0 && c < 180 && (lvl[r][c] == 'g' || lvl[r][c] == 'h' || lvl[r][c] == 'i')) {
			exploded = true;
			posY = r * 64 - 150;//to bring pool of fir on top of platform
			ger.setPosition(posX, posY);
			if (!fireBomb)active = false;
		}
	}
	else if (fireBomb) {//if grenade has exploded and is a fire bomb it maintains a pool of fore for 10 seconds
		poolTimer += 1.0f / 60.0f;
		if (poolTimer >= 10.0f) {
			active = false; poolTimer = 0; exploded = false;
		}
	}
}
void Grenade::addTex(Texture* tex) {
	ger.setTexture(*tex, true);
}
void Grenade::setActive(bool a) { active = a; }
bool Grenade::isActive()const { return active; }
Sprite& Grenade::getSp() { return ger; }
bool Grenade::getExploded()const { return exploded; }
float Grenade::getX()const { return posX; }
float Grenade::getY()const { return posY; }
int Weapon::getAmmo() const { return ammo; }
void Weapon::setAmmo(int a) { ammo = a; }
void Grenade::collided() {
	exploded = true;
}
bool Grenade::getFire()const { return fireBomb; }
float Grenade::getPoolTime()const { return poolTimer; }
void Grenade::setPool(float p) { poolTimer = p; }

void Grenade::dropGrenade(float x, float y)
{
	posX = x;
	posY = y;
	velY = 5.0; // down
	velX = 0; // no horizontal move
	active = true;
	exploded = false;
	fireBomb = false;
	poolTimer = 0.0f;
	ger.setScale(0.5f, 0.5f);
	ger.setPosition(posX, posY);
	owner = nullptr;
}

void Grenade::setOwner(Enemy* o)
{
	owner = o;
}

Enemy* Grenade::getOwner() const
{
	return owner;
}

void Grenade::setFromEnemy(bool e)
{
	fromEnemy = e;
}

bool Grenade::getFromEnemy() const
{
	return fromEnemy;
}

//ENEMY.H STUFF
Enemy::Enemy()
{
	hp = 0;
	speed = 0.0;
	positionX = 0.0;
	positionY = 0.0;
	batchMin = 0;
	batchMax = 0;
	gravity = 0.0;
	velocityY = 0.0;
	facingRight = false;;
	onGround = true;
	enemyDamage = 0; // 
	attackRange = 0;
	explosionHit = false;
	isAlive = true;
	score = 0;  // the increase in score player will get when it kills a certain enemy 
	playerState = 0; // idle by default 
	spritedata.raw_img_x = 0;
	spritedata.raw_img_y = 0;
	spritedata.height = 0;
	spritedata.width = 0;
	name = nullptr;
	gravity = 2.0;
	isJumping = false; dropped = false; burnTimer = 0.0f;
	texCounter = 0;
	texFrame = 0;
	frameLimit = 6;
	deathTimer = 0;
}

double Enemy::getX()
{
	return positionX;
}

double Enemy::getY()
{
	return positionY;
}

int Enemy::getWidth()
{
	return spritedata.width;
}

int Enemy::getHeight()
{
	return spritedata.height;
}

bool Enemy::getAlive()
{
	return isAlive;
}

void Enemy::setPosition(double x, double y)
{
	positionX = x;
	positionY = y;
}

void Enemy::setMoves(float min, float max)
{
	minMove = min;
	maxMove = max;
}

bool Enemy::isRange(float player_x) const
{
	return player_x >= minMove && player_x <= maxMove;
}

Enemy::~Enemy()
{
	delete[] name;
}

Infantry::Infantry()
{
	speed = 5.0;
	attackRange = 200;
}

// INFANTRY IMPLEMENTATION
void Infantry::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isAlive) return;

	// HORIZONTAL MOVEMENT 
	// making it move left or right on basis of the player 
	double offset_x;
	if (player_x < positionX) // left 
	{
		offset_x = positionX - speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size; // only the lower body not the feet or else it will jump continuously 
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + 35) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		// jump one block 
		if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
		{
			if (onGround && !isJumping) // continous jumps not allowed (one at a time)
			{
				velocityY = -15;
				onGround = false;
				isJumping = true;
			}
			else if (velocityY < 0 && isJumping) // to allow jumping even when block is there 
			{
				positionX = offset_x;
			}
		}
		else
		{
			// setting value 
			positionX = offset_x;
		}
		facingRight = false;
		if (playerState != 1)
		{
			texCounter = 0;
			texFrame = 0;
		}
		playerState = 1;  // 1 represents movement 
	}
	else if (player_x > positionX)
	{
		offset_x = positionX + speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size;
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + spritedata.width - 1) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		// jump one block 
		if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
		{
			if (onGround && !isJumping)
			{
				velocityY = -22;
				onGround = false;
				isJumping = true;
			}
			else if (velocityY < 0 && isJumping) // to allow jumping even when block is there 
			{
				positionX = offset_x;
			}
		}
		else
		{
			// setting value 
			positionX = offset_x;
		}
		facingRight = true;
		if (playerState != 1)
		{
			texCounter = 0;
			texFrame = 0;
		}
		playerState = 1;  // 1 represents movement 
	}

	// VERTICAL MOVEMENT 
	double terminal_velocity = 20.0;

	velocityY += gravity;
	if (velocityY > terminal_velocity) velocityY = terminal_velocity; // clamping so valocity doesnt increase beyond certain point 
	double offset_y = velocityY + positionY;

	// now we check its bottom to see if it is ground if not it will start falling 
	int x1 = positionX / cell_size;
	int x2 = (positionX + spritedata.width / 2) / cell_size;
	int x3 = (positionX + spritedata.width) / cell_size;
	int y = (offset_y + spritedata.height) / cell_size;

	if (x1 < 0) x1 = 0; if (x1 >= Lwidth) x1 = Lwidth - 1;
	if (x2 < 0) x2 = 0; if (x2 >= Lwidth) x2 = Lwidth - 1;
	if (x3 < 0) x3 = 0; if (x3 >= Lwidth) x3 = Lwidth - 1;
	if (y < 0) y = 0;  if (y >= Lheight) y = Lheight - 1;

	if (lvl[y][x1] != ' ' || lvl[y][x2] != ' ' || lvl[y][x3] != ' ')
	{
		// then there is ground below the sprite 
		isJumping = false;
		onGround = true;
		velocityY = 0;
		positionY = (y * cell_size) - spritedata.height;    // marking down as the position where it has settled 
	}
	else
	{
		onGround = false;
		positionY = offset_y;
	}
}
void Infantry::die()
{
	isAlive = false;
	speed = 0;
	velocityY = 0;
	playerState = 3; // 3 signifies death 
	texCounter = 0;
	texFrame = 0;
}

void Infantry::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive) return;

	if (strCmp(weaponName, "LaserGun") == 0)
	{
		die();
	}

	hp -= d;
	if (hp <= 0)
	{
		die();
	}
}

// REBEL SOLDIER IMPLEMENTATION 
RebelSoldier::RebelSoldier(double x, double y)
{
	name = new char[13];
	const char* target = "RebelSoldier";
	int i = 0;
	while (i < 12)
	{
		name[i] = target[i];
		i++;
	}
	name[i] = '\0';
	pistol = new Pistol();  // pistol for shooting 
	hp = 2;
	batchMin = 2;
	batchMax = 4;
	score = 50;
	positionX = x;
	positionY = y;
	spritedata.scale_x = 0.7;
	spritedata.scale_y = 0.7;
	spritedata.raw_img_x = 134;
	spritedata.raw_img_y = 117;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/rebelSoldierIdle.png");
	walkTex[0].loadFromFile("25I-0891_25I-0915_Assets/rebelSoldier_walk1.png");
	walkTex[1].loadFromFile("25I-0891_25I-0915_Assets/rebelSoldier_walk2.png");
	attackTex.loadFromFile("25I-0891_25I-0915_Assets/rebelSoldier_attack.png");
	dieTex.loadFromFile("25I-0891_25I-0915_Assets/rebelSoldier_die.png"); score = 50;
}

void Infantry::draw(RenderWindow& window, float camera_x, float camera_y)
{

	if (playerState == 0)
		sprite.setTexture(idleTex);
	else if (playerState == 1)
	{
		texCounter++;
		if (texCounter >= frameLimit)
		{
			texFrame++;
			texCounter = 0;

			if (texFrame >= 2)
				texFrame = 0;
		}
		sprite.setTexture(walkTex[texFrame]);
	}
	else if (playerState == 2)
	{
		texCounter = 0;
		texFrame = 0;
		sprite.setTexture(attackTex);
	}
	else if (playerState == 3)
	{
		texCounter = 0;
		texFrame = 0;
		sprite.setTexture(dieTex);
	}

	if (facingRight)
	{
		sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
	}
	else
	{
		sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x, positionY - camera_y);
	}
	window.draw(sprite);
}

void RebelSoldier::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	if (!isAlive)
	{
		if (deathTimer < 60)  // prints the dead sprite for one sec 
		{
			playerState = 3;
			draw(window, camera_x, camera_y);
			deathTimer++;
		}
		return;
	}

	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping
	if (isRange(player_x))
	{
		pistol->updateTimer();  // time will increase until it can shoot 

		float stop = 50.0;
		if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= attackRange)
		{
			playerState = 2; // 2 represents attacking 

			if (player_x < positionX)
				facingRight = false;
			else
				facingRight = true;

			if (pistol->canSh())
			{
				pistol->shoot(positionX, positionY, facingRight, 0.0, bullets, bulTex, true); // true means from enemy 
				pistol->resetTimer();
			}
		}

		// then goes to movement 
		if (abs(player_x - positionX) > stop)
			Infantry::move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement of infantry enemies 

		// LASTLY DRAW 
		Infantry::draw(window, camera_x, camera_y);
	}
}

// SHIELDED SOLDIER IMPLEMENTATION 
ShieldedSoldier::ShieldedSoldier(double x, double y)
{
	name = new char[16];
	const char* target = "ShieldedSoldier";
	int i = 0;
	while (i < 15)
	{
		name[i] = target[i];
		i++;
	}
	name[i] = '\0';
	pistol = new Pistol;
	hp = 5;
	batchMin = 1;
	batchMax = 2;
	score = 75;
	shieldActive = true;
	positionX = x;
	positionY = y;
	spritedata.scale_x = 0.5;
	spritedata.scale_y = 0.5;
	spritedata.raw_img_x = 166;
	spritedata.raw_img_y = 195;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/shieldedSoldier_idle.png");
	walkTex[0].loadFromFile("25I-0891_25I-0915_Assets/shieldedSoldier_walk1.png");
	walkTex[1].loadFromFile("25I-0891_25I-0915_Assets/shieldedSoldier_walk2.png");
	attackTex.loadFromFile("25I-0891_25I-0915_Assets/shieldedSoldier_attack.png");
	dieTex.loadFromFile("25I-0891_25I-0915_Assets/shieldedSoldier_die.png"); score = 75;
}

void ShieldedSoldier::takeDamage(int d, char* weaponName, float damage_x, bool canPierce)
{
	explosionHit = (strCmp(weaponName, "Grenade") == 0);

	bool damage_right;
	if (damage_x < positionX) // on left side 
		damage_right = false;
	else   // right side 
		damage_right = true;

	bool direction = damage_right && !facingRight; // if one is right and the other is left then true and hits 


	if (shieldActive && !direction && !explosionHit && !canPierce)
	{
		return;
	}
	else if (explosionHit)
	{
		shieldActive = false; // shield will be destroyed 
	}
	else
		Infantry::takeDamage(d, weaponName, facingRight);
}

void ShieldedSoldier::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	if (!isAlive)
	{
		if (deathTimer < 60)  // prints the dead sprite for one sec 
		{
			playerState = 3;
			draw(window, camera_x, camera_y);
			deathTimer++;
		}
		return;
	}

	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping
	if (isRange(player_x))
	{
		pistol->updateTimer();  // time will increase until it can shoot 

		float stop = 50.0;
		if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= attackRange)
		{
			playerState = 2; // 2 represents attacking 

			if (player_x < positionX)
				facingRight = false;
			else
				facingRight = true;

			if (pistol->canSh())
			{
				pistol->shoot(positionX, positionY, facingRight, 0.0, bullets, bulTex, true); // true means from enemy 
				pistol->resetTimer();
			}
		}

		// then goes to movement 
		if (abs(player_x - positionX) > stop)
			Infantry::move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement of infantry enemies 

		// LASTLY DRAW 
		Infantry::draw(window, camera_x, camera_y);
	}
}

//// BAZOOKA SOLDIER 
BazookaSoldier::BazookaSoldier(double x, double y)
{
	name = new char[15];
	const char* target = "BazookaSoldier";
	int i = 0;
	while (i < 14)
	{
		name[i] = target[i];
		i++;
	}
	name[i] = '\0';
	rocketLauncher = new RocketLauncher;
	hp = 2;
	batchMin = 1;
	batchMax = 2;
	score = 100;
	positionX = x;
	positionY = y;
	spritedata.scale_x = 0.7;
	spritedata.scale_y = 0.7;
	spritedata.raw_img_x = 189;
	spritedata.raw_img_y = 174;
	attackRange = 730;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/bazookaSoldier_idle.png");
	walkTex[0].loadFromFile("25I-0891_25I-0915_Assets/bazookaSoldier_walk1.png");
	walkTex[1].loadFromFile("25I-0891_25I-0915_Assets/bazookaSoldier_walk2.png");
	attackTex.loadFromFile("25I-0891_25I-0915_Assets/bazookaSoldier_attack.png");
	dieTex.loadFromFile("25I-0891_25I-0915_Assets/rebelSoldier_die.png"); score = 100;
}

void BazookaSoldier::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	if (!isAlive)
	{
		if (deathTimer < 60)  // prints the dead sprite for one sec 
		{
			playerState = 3;
			draw(window, camera_x, camera_y);
			deathTimer++;
		}
		return;
	}

	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping
	float stop = 730.0;
	if (isRange(player_x))
	{
		rocketLauncher->updateTimer();  // time will increase until it can shoot 

		if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= 120)
		{
			playerState = 2; // 2 represents attacking 

			if (player_x < positionX)
				facingRight = false;
			else
				facingRight = true;

			if (rocketLauncher->canSh())
			{
				rocketLauncher->shoot(positionX, positionY, facingRight, 45.0, bullets, bulTex, true);
				rocketLauncher->resetTimer(); // resets for cooldown
			}
		}

		// then goes to movement 
		if (abs(player_x - positionX) > stop)
			Infantry::move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement of infantry enemies 

		// LASTLY DRAW 
		Infantry::draw(window, camera_x, camera_y);
	}
}

// GRENADE SOLDIER (FINALLYYY)
GrenadeSoldier::GrenadeSoldier(double x, double y)
{
	name = new char[15];
	strCopy(name, "GrenadeSoldier");
	hp = 2;
	score = 100;
	speed = 5.0;
	attackRange = 350;
	facingRight = false;
	playerState = 0;
	positionX = x;
	positionY = y;
	spritedata.scale_x = 0.3;
	spritedata.scale_y = 0.3;
	spritedata.raw_img_x = 232;
	spritedata.raw_img_y = 312;
	cooldown = 0.0f;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/GrenadeSoldier_idle.png");
	walkTex[0].loadFromFile("25I-0891_25I-0915_Assets/GrenadeSoldier_walk1.png");
	walkTex[1].loadFromFile("25I-0891_25I-0915_Assets/GrenadeSoldier_walk2.png");
	attackTex.loadFromFile("25I-0891_25I-0915_Assets/GrenadeSoldier_attack.png");
	dieTex.loadFromFile("25I-0891_25I-0915_Assets/rebelSoldier_die.png"); score = 100;
}

void GrenadeSoldier::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{

	if (!isAlive)
	{
		if (deathTimer < 60)  // prints the dead sprite for one sec 
		{
			playerState = 3;
			draw(window, camera_x, camera_y);
			deathTimer++;
		}
		return;
	}

	if (cooldown > 0)
	{
		cooldown -= 1.0f / 60.0f; // 60 fps 
	}

	float stop = 120.0;

	if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= 120)
	{
		playerState = 2;
		if (player_x < positionX)
			facingRight = false;
		else if (player_x > positionX)
			facingRight = true;

		if (cooldown <= 0)
		{
			int i = 0;
			while (i < 15 && grenades[i].isActive())
			{
				i++;
			}

			if (i < 15)
			{
				grenades[i].addTex(greTex);
				grenades[i].throwGrenade(positionX, positionY, facingRight, false);
				grenades[i].setOwner(this);
				grenades[i].setFromEnemy(true);
			}

			cooldown = 1.8f;
		}
	}

	if (abs(player_x - positionX) > stop)
	{
		playerState = 1;
		Infantry::move(cell_size, lvl, Lwidth, Lheight, player_x);
	}

	Infantry::draw(window, camera_x, camera_y);
}

// UNDEAD 
Undead::Undead()
{
	speed = 2.0; // walks slowly 
}

void Undead::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isAlive) return;

	// HORIZONTAL MOVEMENT 
	// making it move left or right on basis of the player 
	double offset_x;
	if (player_x < positionX) // left 
	{
		offset_x = positionX - speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size; // only the lower body not the feet or else it will jump continuously 
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + 35) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		// jump one block 
		if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
		{
			if (onGround && !isJumping) // continous jumps not allowed (one at a time)
			{
				velocityY = -15;
				onGround = false;
				isJumping = true;
			}
			else if (velocityY < 0 && isJumping) // to allow jumping even when block is there 
			{
				positionX = offset_x;
			}
		}
		else
		{
			// setting value 
			positionX = offset_x;
		}
		facingRight = false;
		playerState = 1;  // 1 represents movement 
	}
	else if (player_x > positionX)
	{
		offset_x = positionX + speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size;
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + spritedata.width - 1) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		// jump one block 
		if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
		{
			if (onGround && !isJumping)
			{
				velocityY = -22;
				onGround = false;
				isJumping = true;
			}
			else if (velocityY < 0 && isJumping) // to allow jumping even when block is there 
			{
				positionX = offset_x;
			}
		}
		else
		{
			// setting value 
			positionX = offset_x;
		}
		facingRight = true;
		playerState = 1;
	}

	// VERTICAL MOVEMENT 
	double terminal_velocity = 20.0;

	velocityY += gravity;
	if (velocityY > terminal_velocity) velocityY = terminal_velocity; // clamping so valocity doesnt increase beyond certain point 
	double offset_y = velocityY + positionY;

	// now we check its bottom to see if it is ground if not it will start falling 
	int x1 = positionX / cell_size;
	int x2 = (positionX + spritedata.width / 2) / cell_size;
	int x3 = (positionX + spritedata.width) / cell_size;
	int y = (offset_y + spritedata.height) / cell_size;

	if (x1 < 0) x1 = 0; if (x1 >= Lwidth) x1 = Lwidth - 1;
	if (x2 < 0) x2 = 0; if (x2 >= Lwidth) x2 = Lwidth - 1;
	if (x3 < 0) x3 = 0; if (x3 >= Lwidth) x3 = Lwidth - 1;
	if (y < 0) y = 0;  if (y >= Lheight) y = Lheight - 1;

	if (lvl[y][x1] != ' ' || lvl[y][x2] != ' ' || lvl[y][x3] != ' ')
	{
		// then there is ground below the sprite 
		isJumping = false;
		onGround = true;
		velocityY = 0;
		positionY = (y * cell_size) - spritedata.height;    // marking down as the position where it has settled 
	}
	else
	{
		onGround = false;
		positionY = offset_y;
	}
}

void Undead::die()
{
	isAlive = false;
	speed = 0;
	velocityY = 0;
	playerState = 3;
}

void Undead::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive) return;

	hp -= d;
	if (hp <= 0)
	{
		die();
	}
}

// MUMMY 
MummyWarrior::MummyWarrior(double x, double y)
{
	isCrumbling = false;
	resurrectTime = 3;
	name = new char[13];
	const char* target = "MummyWarrior";
	int i = 0;
	while (i < 12)
	{
		name[i] = target[i];
		i++;
	}
	name[i] = '\0';
	hp = 5;
	batchMin = 1;
	batchMax = 1;
	score = 150;
	positionX = x;
	positionY = y;
	attackRange = 50; // the collison range in this case 
	spritedata.scale_x = 0.7;
	spritedata.scale_y = 0.7;
	spritedata.raw_img_x = 94;
	spritedata.raw_img_y = 118;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/mummy_idle.png");
	walkTex[0].loadFromFile("25I-0891_25I-0915_Assets/mummy_walk1.png");
	walkTex[1].loadFromFile("25I-0891_25I-0915_Assets/mummy_walk2.png");
	dieTex.loadFromFile("25I-0891_25I-0915_Assets/mummy_crumb.png");
}

void MummyWarrior::draw(RenderWindow& window, float camera_x, float camera_y)
{
	if (playerState == 0)
		sprite.setTexture(idleTex);
	else if (playerState == 1)
	{
		texCounter++;
		if (texCounter >= frameLimit)
		{
			texFrame++;
			texCounter = 0;

			if (texFrame >= 2)
				texFrame = 0;
		}
		sprite.setTexture(walkTex[texFrame]);
	}
	else if (isCrumbling)
	{
		texCounter = 0;
		texFrame = 0;
		sprite.setTexture(dieTex);
	}

	if (facingRight)
	{
		sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
	}
	else
	{
		sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x, positionY - camera_y);
	}
	window.draw(sprite);
}

void MummyWarrior::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (strCmp(weaponName, "Grenade") == 0 || strCmp(weaponName, "FlameShot") == 0)
	{
		die();
	}
	else
	{
		isCrumbling = true;
		playerState = 3; // crumbling 
	}
}

void MummyWarrior::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	if (!isAlive)
	{
		playerState = 3;
		draw(window, camera_x, camera_y);
		return;
	}
	float stop = 40.0;
	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping
	if (isRange(player_x))
	{
		if (isCrumbling)
		{
			resurrectTime -= (1 / 60.0);
			if (resurrectTime <= 0)
			{
				resurrectTime = 3.0;
			}
			else
			{
				draw(window, camera_x, camera_y);
				return;
			}

		}

		if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) < attackRange)
		{
			player->applyEffect(2);
		}

		playerState = 1;
		if (abs(player_x - positionX) > stop)
		{
			Undead::move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement
		}
		draw(window, camera_x, camera_y);
	}
}

// ZOMBIE 
Zombie::Zombie(double x, double y)
{
	name = new char[7];
	const char* target = "Zombie";
	int i = 0;
	while (i < 6)
	{
		name[i] = target[i];
		i++;
	}
	name[i] = '\0';
	pistol = new Pistol;
	hp = 5;
	batchMin = 3;
	batchMax = 5;
	score = 100;
	positionX = x;
	positionY = y;
	spritedata.scale_x = 0.8;
	spritedata.scale_y = 0.8;
	spritedata.raw_img_x = 112;
	spritedata.raw_img_y = 128;
	attackRange = 350;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/zombie_idle.png");
	walkTex[0].loadFromFile("25I-0891_25I-0915_Assets/zombie_walk1.png");
	walkTex[1].loadFromFile("25I-0891_25I-0915_Assets/zombie_walk2.png");
	attackTex.loadFromFile("25I-0891_25I-0915_Assets/zombie_attack.png");
	dieTex.loadFromFile("25I-0891_25I-0915_Assets/zombie_die.png"); score = 100;
}

void Zombie::draw(RenderWindow& window, float camera_x, float camera_y)
{
	if (playerState == 0)
		sprite.setTexture(idleTex);
	else if (playerState == 1)
	{
		texCounter++;
		if (texCounter >= frameLimit)
		{
			texFrame++;
			texCounter = 0;

			if (texFrame >= 2)
				texFrame = 0;
		}
		sprite.setTexture(walkTex[texFrame]);
	}
	else if (playerState == 2)
		sprite.setTexture(attackTex);
	else if (playerState == 3)
		sprite.setTexture(dieTex);

	if (facingRight)
	{
		sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
	}
	else
	{
		sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x, positionY - camera_y);
	}
	window.draw(sprite);
}

void Zombie::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	Undead::takeDamage(d, weaponName, damage_x);
}

void Zombie::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	if (!isAlive)
	{
		if (deathTimer < 60)  // prints the dead sprite for one sec 
		{
			playerState = 3;
			draw(window, camera_x, camera_y);
			deathTimer++;
		}
		return;
	}

	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping
	float stop = 50.0;
	if (isRange(player_x))
	{
		pistol->updateTimer();  // time will increase until it can shoot 


		if (abs(player_x - positionX) <= (attackRange - 300) && abs(player_y - positionY) < (attackRange - 300))
		{
			player->applyEffect(1);
		}
		else if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= 120)
		{
			playerState = 2; // 2 represents attacking 

			if (player_x < positionX)
				facingRight = false;
			else
				facingRight = true;

			if (pistol->canSh())
			{
				pistol->shoot(positionX, positionY, facingRight, 0.0, bullets, bulTex, true);
				pistol->resetTimer();
			}
		}

		playerState = 1;
		if (abs(player_x - positionX) > stop)
			Undead::move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement 
		draw(window, camera_x, camera_y);
	}
}
int Enemy::getScore()const { return score; }
bool Enemy::getJumping()const { return isJumping; }
// PARATROOPER
Aerial::Aerial(double x, double y)
{
	isLanded = false;
	gravity = 0.2;
	soldierType = rand() % 4;

	if (soldierType == 0)
		soldier = new RebelSoldier(x, y);
	else if (soldierType == 1)
		soldier = new ShieldedSoldier(x, y);
	else if (soldierType == 2)
		soldier = new GrenadeSoldier(x, y);
	else if (soldierType == 3)
		soldier = new BazookaSoldier(x, y);
	if (soldier != nullptr)score = 25 + soldier->getScore();
}

void Aerial::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isLanded)
	{
		double terminal_velocity = 4.0;

		velocityY += gravity;
		if (velocityY > terminal_velocity) velocityY = terminal_velocity; // clamping so valocity doesnt increase beyond certain point 
		double offset_y = velocityY + positionY;

		// now we check its bottom to see if it is ground if not it will start falling 
		int x1 = positionX / cell_size;
		int x2 = (positionX + spritedata.width / 2) / cell_size;
		int x3 = (positionX + spritedata.width) / cell_size;
		int y = (offset_y + spritedata.height) / cell_size;

		if (x1 < 0) x1 = 0; if (x1 >= Lwidth) x1 = Lwidth - 1;
		if (x2 < 0) x2 = 0; if (x2 >= Lwidth) x2 = Lwidth - 1;
		if (x3 < 0) x3 = 0; if (x3 >= Lwidth) x3 = Lwidth - 1;
		if (y < 0) y = 0;  if (y >= Lheight) y = Lheight - 1;

		if (lvl[y][x1] != ' ' || lvl[y][x2] != ' ' || lvl[y][x3] != ' ')
		{
			// then there is ground below the sprite 
			onGround = true;
			isLanded = true;
			velocityY = 0;
			positionY = y * cell_size - spritedata.height; // marking down as the position where it has settled 
			if (soldier != nullptr)
				soldier->setPosition(positionX, (y * cell_size) - soldier->getHeight());  // position for when it lands (use soldier height when lands)
		}
		else
		{
			onGround = false;
			positionY = offset_y;
			soldier->setPosition(positionX + 20, positionY + 90); // slighlt below the parachute 
		}
	}
	else
	{
		soldier->move(cell_size, lvl, Lwidth, Lheight, player_x);
		positionX = soldier->getX();
		positionY = soldier->getY();
	}
}

void Aerial::die()
{

	isAlive = false;
	speed = 0;
	velocityY = 0;

	if (soldier != nullptr)
		soldier->die();
}

void Aerial::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive || soldier == nullptr) return;

	soldier->takeDamage(d, weaponName, damage_x);

	if (!soldier->getAlive())
	{
		isAlive = false;
		velocityY = 0;
	}
}

Paratrooper::Paratrooper(double x, double y) : Aerial(x, 0)
{
	name = new char[12];
	const char* target = "Paratrooper";
	int i = 0;
	while (i < 11)
	{
		name[i] = target[i];
		i++;
	}
	positionX = x;
	positionY = 0;
	name[i] = '\0';
	spritedata.raw_img_x = 236;
	spritedata.raw_img_y = 196;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/paratrooper_flying.png");
}

void Paratrooper::draw(RenderWindow& window, float camera_x, float camera_y)
{
	if (!isLanded)
	{
		sprite.setTexture(idleTex);
		sprite.setPosition(positionX - camera_x, positionY - 30 - camera_y);
		window.draw(sprite);

		soldier->draw(window, camera_x, camera_y);
	}
	else
	{
		soldier->draw(window, camera_x, camera_y);
	}
}

void Paratrooper::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	if (!isAlive)
	{
		playerState = 3;
		draw(window, camera_x, camera_y);
		return;
	}
	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping
	if (isRange(player_x))
	{
		if (!isLanded)
		{
			move(cell_size, lvl, Lwidth, Lheight, player_x);
			draw(window, camera_x, camera_y);
		}
		else
		{
			soldier->update(cell_size, lvl, Lwidth, Lheight, window, player_x, player_y, camera_x, bullets, bulTex, grenades, greTex, camera_y, player);
		}
	}
}

void Paratrooper::setMoves(float min, float max)
{
	minMove = min;
	maxMove = max;

	if (soldier != nullptr)
	{
		soldier->setMoves(min, max); // so the infantry can spawn in aerial withput og restriction 
	}
}

// MARTIAN 
Martian::Martian(double x, double y)
{
	name = new char[8];
	const char* target = "Martian";
	int i = 0;
	while (i < 7)
	{
		name[i] = target[i];
		i++;
	}
	name[i] = '\0';
	hp = 3;
	batchMin = 1;
	batchMax = 2;
	score = 200;
	positionX = x;
	speed = 5.0;
	positionY = 0;
	spritedata.raw_img_x = 302;
	spritedata.raw_img_y = 346;
	attackRange = 350;
	spritedata.scale_x = 0.3;
	spritedata.scale_y = 0.3;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	floating.loadFromFile("25I-0891_25I-0915_Assets/martian_flying.png");
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/martian_idle.png");
	walkTex[0].loadFromFile("25I-0891_25I-0915_Assets/martian_walk1.png");
	walkTex[1].loadFromFile("25I-0891_25I-0915_Assets/martian_walk2.png");
	attackTex.loadFromFile("25I-0891_25I-0915_Assets/martian_attack.png");
	dieTex.loadFromFile("25I-0891_25I-0915_Assets/martian_die.png");
	phase1 = true;
	podDestroyed = false;
	pistol = new Pistol();
	beamCooldown = 0; score = 200;
}

void Alien::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isAlive) return;

	if (phase1)
	{
		float falling = 1.5;
		double stop = 350.0;
		if (positionY < stop)
		{
			positionY += falling;
			return;
		}

		positionY = stop;

		if (player_x < positionX)
		{
			positionX -= speed;
			facingRight = false;
		}
		else if (player_x > positionX)
		{
			positionX += speed;
			facingRight = true;
		}
	}
	else
	{
		if (!isAlive) return;

		// HORIZONTAL MOVEMENT 
		// making it move left or right on basis of the player 
		double offset_x;
		if (player_x < positionX) // left 
		{
			offset_x = positionX - speed;
			int y1 = (positionY) / cell_size;
			int y2 = (positionY + spritedata.height - 5) / cell_size; // only the lower body not the feet or else it will jump continuously 
			int y3 = (positionY + spritedata.height / 2) / cell_size;
			int x = (offset_x + 35) / cell_size;

			if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
			if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
			if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
			if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

			// jump one block 
			if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
			{
				if (onGround && !isJumping) // continous jumps not allowed (one at a time)
				{
					velocityY = -15;
					onGround = false;
					isJumping = true;
				}
				else if (velocityY < 0 && isJumping) // to allow jumping even when block is there 
				{
					positionX = offset_x;
				}
			}
			else
			{
				// setting value 
				positionX = offset_x;
			}
			facingRight = false;
			playerState = 1;  // 1 represents movement 
		}
		else if (player_x > positionX)
		{
			offset_x = positionX + speed;
			int y1 = (positionY) / cell_size;
			int y2 = (positionY + spritedata.height - 5) / cell_size;
			int y3 = (positionY + spritedata.height / 2) / cell_size;
			int x = (offset_x + spritedata.width - 1) / cell_size;

			if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
			if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
			if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
			if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

			// jump one block 
			if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
			{
				if (onGround && !isJumping)
				{
					velocityY = -22;
					onGround = false;
					isJumping = true;
				}
				else if (velocityY < 0 && isJumping) // to allow jumping even when block is there 
				{
					positionX = offset_x;
				}
			}
			else
			{
				// setting value 
				positionX = offset_x;
			}
			facingRight = true;
			playerState = 1;
		}

		// VERTICAL MOVEMENT 
		double terminal_velocity = 20.0;

		velocityY += gravity;
		if (velocityY > terminal_velocity) velocityY = terminal_velocity; // clamping so valocity doesnt increase beyond certain point 
		double offset_y = velocityY + positionY;

		// now we check its bottom to see if it is ground if not it will start falling 
		int x1 = positionX / cell_size;
		int x2 = (positionX + spritedata.width / 2) / cell_size;
		int x3 = (positionX + spritedata.width) / cell_size;
		int y = (offset_y + spritedata.height) / cell_size;

		if (x1 < 0) x1 = 0; if (x1 >= Lwidth) x1 = Lwidth - 1;
		if (x2 < 0) x2 = 0; if (x2 >= Lwidth) x2 = Lwidth - 1;
		if (x3 < 0) x3 = 0; if (x3 >= Lwidth) x3 = Lwidth - 1;
		if (y < 0) y = 0;  if (y >= Lheight) y = Lheight - 1;

		if (lvl[y][x1] != ' ' || lvl[y][x2] != ' ' || lvl[y][x3] != ' ')
		{
			// then there is ground below the sprite 
			isJumping = false;
			onGround = true;
			velocityY = 0;
			positionY = (y * cell_size) - spritedata.height;    // marking down as the position where it has settled 
		}
		else
		{
			onGround = false;
			positionY = offset_y;
		}
	}
}

void Alien::die()
{
	isAlive = false;
	speed = 0;
	velocityY = 0;
	playerState = 3;
}

void Martian::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive) return;

	hp -= d;
	if (hp <= 0 && phase1)
	{
		podDestroyed = true;
		phase1 = false;
		hp = 3; // moved to ground 
		velocityY = 5;
		onGround = false;
		isJumping = false;
		spritedata.raw_img_x = 282;
		spritedata.raw_img_y = 263;
		spritedata.scale_x = 0.3;
		spritedata.scale_y = 0.3;
		spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
		spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	}
	else if (hp <= 0)
	{
		die();
	}

}

void Martian::draw(RenderWindow& window, float camera_x, float camera_y)
{
	if (phase1)
	{
		sprite.setTexture(floating);
		if (facingRight)
			sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		else
			sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
		window.draw(sprite);
	}
	else
	{
		if (playerState == 0)
			sprite.setTexture(idleTex);
		else if (playerState == 1)
		{
			texCounter++;
			if (texCounter >= frameLimit)
			{
				texFrame++;
				texCounter = 0;

				if (texFrame >= 2)
					texFrame = 0;
			}
			sprite.setTexture(walkTex[texFrame]);
		}
		else if (playerState == 2)
			sprite.setTexture(attackTex);
		else if (playerState == 3)
			sprite.setTexture(dieTex);

		if (facingRight)
		{
			sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
			sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
		}
		else
		{
			sprite.setScale(spritedata.scale_x, spritedata.scale_y);
			sprite.setPosition(positionX - camera_x, positionY - camera_y);
		}
		window.draw(sprite);
	}
}

void Martian::fireBeam(float player_x, float player_y, Bullet* bullets, Texture* bulTex)
{
	// we add the shooting beams stuff here 
	if (beamCooldown > 0)
	{
		beamCooldown -= (1.0f / 60.0f);
		return;
	}

	// check martian position
	if (abs(player_x - positionX) <= 70 && player_y > positionY)
	{
		int i = 0;
		while (i < 30 && bullets[i].active)i++;//finds first inactive bullet
		if (i >= 30) return;

		bullets[i].bul.setTexture(*bulTex, true);
		bullets[i].bul.setScale(0.3f, 0.3f);
		bullets[i].active = true;
		bullets[i].isRocket = false;
		bullets[i].isFlame = false;
		bullets[i].isLaser = false;
		bullets[i].isBeam = true;

		// start 
		strCopy(bullets[i].sourceName, "EnergyBeam");
		bullets[i].damage = hp;
		bullets[i].fromEnemy = true;
		// end 

		bullets[i].velX = 0; // no horiz movement 
		bullets[i].velY = 10; // quickly moves down
		bullets[i].posX = positionX; bullets[i].posY = positionY;//starting pos is the same as guns
		bullets[i].bul.setPosition(bullets[i].posX, bullets[i].posY);

		beamCooldown = 0.25f;
	}
}

void Martian::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	if (!isAlive)
	{
		if (deathTimer < 60)  // prints the dead sprite for one sec 
		{
			playerState = 3;
			draw(window, camera_x, camera_y);
			deathTimer++;
		}
		return;
	}

	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping
	if (isRange(player_x))
	{
		if (phase1)
		{
			fireBeam(player_x, player_y, bullets, bulTex);
			Alien::move(cell_size, lvl, Lwidth, Lheight, player_x);
			draw(window, camera_x, camera_y);
		}
		else
		{
			// then goes to movement 

			pistol->updateTimer();  // time will increase until it can shoot 

			// have to call vertical motion so it doesnt stay stranded in air 
			// VERTICAL MOVEMENT 
			double terminal_velocity = 20.0;

			velocityY += gravity;
			if (velocityY > terminal_velocity) velocityY = terminal_velocity; // clamping so valocity doesnt increase beyond certain point 
			double offset_y = velocityY + positionY;

			// now we check its bottom to see if it is ground if not it will start falling 
			int x1 = positionX / cell_size;
			int x2 = (positionX + spritedata.width / 2) / cell_size;
			int x3 = (positionX + spritedata.width) / cell_size;
			int y = (offset_y + spritedata.height) / cell_size;

			if (x1 < 0) x1 = 0; if (x1 >= Lwidth) x1 = Lwidth - 1;
			if (x2 < 0) x2 = 0; if (x2 >= Lwidth) x2 = Lwidth - 1;
			if (x3 < 0) x3 = 0; if (x3 >= Lwidth) x3 = Lwidth - 1;
			if (y < 0) y = 0;  if (y >= Lheight) y = Lheight - 1;

			if (lvl[y][x1] != ' ' || lvl[y][x2] != ' ' || lvl[y][x3] != ' ')
			{
				// then there is ground below the sprite 
				isJumping = false;
				onGround = true;
				velocityY = 0;
				positionY = (y * cell_size) - spritedata.height;    // marking down as the position where it has settled 
			}
			else
			{
				onGround = false;
				positionY = offset_y;
			}

			float stop = 50.0;
			if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= attackRange)
			{
				playerState = 2; // 2 represents attacking 

				if (player_x < positionX)
					facingRight = false;
				else
					facingRight = true;

				if (pistol->canSh())
				{
					pistol->shoot(positionX, positionY, facingRight, 0.0, bullets, bulTex, true);
					pistol->resetTimer();
				}
			}

			if (abs(player_x - positionX) > stop)
				Alien::move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement of infantry enemies

			// LASTLY DRAW 
			draw(window, camera_x, camera_y);
		}
	}
}
bool Enemy::getDropped()const { return dropped; }
void Enemy::setDropped(bool d) { dropped = d; }
int Enemy::getHp()const { return hp; }
float Enemy::getBurnTime()const { return burnTimer; }
void Enemy::setBurnTime(float b) { burnTimer = b; }

//COLLECTIBLE.H STUFF
Collectible::Collectible() {
	active = false;
}
void Collectible::activate(float x, float y) { posX = x; posY = y; active = true; }//when an enemy dies food spawn where it died
void Collectible::deactivate() { active = false; }
bool Collectible::isActive()const { return active; }
float Collectible::getX()const { return posX; }
float Collectible::getY()const { return posY; }
void Food::spawn(float x, float y, Texture* turk, Texture* fru) {
	replenishAmount = 2 + (rand() % 2);//random num b/w 0 or 1, adding 2 makes it b/w 2 and 3
	if (replenishAmount == 2) //iTS A FRUIT!!!
		sp.setTexture(*fru, true);
	else //ITZ A TURKEY!!
		sp.setTexture(*turk, true);
	this->activate(x, y);
}
void Food::effectPlayer(Character* player) {
	player->setSaturation(player->getSaturation() + replenishAmount);
	player->switchWeapSp(player->getWeapon()->getType());//to switch sprites if it becomes fat
	this->deactivate();//once it effects the player it is no more
}
void Food::update(char** lvl, float camera_x) {
	if (!active) return;
	int r = (posY + 64) / 64;//= 20 to ckeck a lil more below
	int c = posX / 64;
	if (r >= 0 && c >= 0 && r < 20 && c < 180 && lvl[r][c] != 'g' && lvl[r][c] != 'h' && lvl[r][c] != 'i')posY += 6.5f;//falls with this speed
	if (posX < camera_x - 70)this->deactivate();//deactivate food if it gets left behind
}
bool Collectible::collision(float pX, float pY) {
	float base = abs(posX - pX), perp = abs(posY - pY);
	float dist = (base) * (base)+(perp) * (perp);//using pythagoras we get distance between obj and player which wud be triangles hypotenuse
	if (dist < (100 * 100))return true;//50^2 means distance b/w them is 50 pixels as hyp is sqrt
	return false;
}
Sprite& Collectible::getSp() { return sp; }
Prisoner::Prisoner() { free = false; }
void Prisoner::update(char** lvl, float camera_x) {
	if (!active) return;
	if (posX < camera_x - 70)this->deactivate();
}
void Prisoner::effectPlayer(Character* player) {
	free = true;//has no such effect when player interacts w it only the prisoner is freed
	this->deactivate();
}
bool Prisoner::isFree()const { return free; }
void SupplyCrate::spawn(float x, float y, Texture* tex) {
	this->activate(x, y);
	sp.setTexture(*tex);
	int chance = 1 + rand() % 100;//get the probability between 1-100
	if (chance <= 10)weaponType = 4;//10% chance of a laser gun
	else weaponType = 1 + rand() % 3;//90% chance for hmg, rockL, or flameshit
	int numGer = (rand() % 100) + 1;//probability for hand grenades or fire bombs
	if (numGer <= 50) {//50% chance of hand grenades
		isFire = false;
		grenadeCount = (rand() % 3) + 3;//any value from 3 to 5
	}
	else {//50% chance of fire bombs
		isFire = true;
		grenadeCount = (rand() % 3) + 1;//any value from 1 to 3
	}
	sp.setScale(0.5f, 0.5f);
}
void SupplyCrate::update(char** lvl, float camera_x) {
	if (!active) return;
	int r = (posY + 50) / 64;
	int c = posX / 64;
	if (r >= 0 && c >= 0 && r < 20 && c < 180 && lvl[r][c] != 'g' && lvl[r][c] != 'h' && lvl[r][c] != 'i')posY += 6.5f;//falls with this speed
	if (posX < camera_x - 70)this->deactivate();
}
void SupplyCrate::effectPlayer(Character* player) {
	//this is when the ammo of the player is replenished by idk 10 ig
	player->getWeapon()->setAmmo(player->getWeapon()->getAmmo() + 10);
	this->deactivate();
}
int SupplyCrate::getGrenadeC()const { return grenadeCount; }
bool SupplyCrate::isBomb()const { return isFire; }
int SupplyCrate::getType()const { return weaponType; }
void Prisoner::spawn(float x, float y, Texture* tex) {
	sp.setTexture(*tex); this->activate(x, y); sp.setScale(0.5f, 0.5f);
}
//PLAYERVEHICLE.H STUFF
PlayerVehicle::PlayerVehicle() {
	hits = 0; active = true; occupied = false; velX = 8.2f;//faster than player
	velY = 0.0f; gravity = 0.4f;
}
void PlayerVehicle::spawn(float x, float y) { posX = x; posY = y; active = true; }
void PlayerVehicle::setOccupy(bool o) { occupied = o; }
float PlayerVehicle::getX()const { return posX; }
float PlayerVehicle::getY()const { return posY; }
Slug001::Slug001() {
	tex[0].loadFromFile("25I-0891_25I-0915_Assets/MSlugIdle.png"); sp.setTexture(tex[0]); tex[1].loadFromFile("25I-0891_25I-0915_Assets/MSlug1.png");
	tex[2].loadFromFile("25I-0891_25I-0915_Assets/MSlug2.png"); jumpTex.loadFromFile("25I-0891_25I-0915_Assets/MSlugJump.png"); isJump = false;
	spTimer = 0.0f; texHeights[0] = 303; texWidths[0] = 327; texHeights[1] = 312; texWidths[1] = 330;
	texHeights[2] = 299; texWidths[2] = 338; jumpHeight = 422; jumpWidth = 319;
	tiltTex[0].loadFromFile("25I-0891_25I-0915_Assets/MSlugTilt1.png"); tiltTex[1].loadFromFile("25I-0891_25I-0915_Assets/MSlugTilt2.png");
	tiltTex[2].loadFromFile("25I-0891_25I-0915_Assets/MSlugTilt3.png"); tiltWidth[0] = 327; tiltHeight[0] = 282;
	tiltWidth[1] = 329; tiltHeight[1] = 297; tiltWidth[2] = 334; tiltHeight[2] = 309; vehHeight = texHeights[0];
	vehWidth = texWidths[0]; aim = 0.0f;
}
void Slug001::movement(char** lvl, bool moving, bool right) {
	if (!occupied)return;
	isJump = false; bool climb = false;
	int col;
	if (moving) {
		float nex = posX + (right ? velX : -velX);
		if (right) col = (nex + 327 * 0.4f) / 64; //need const width
		else col = nex / 64;
		int rowFront = (posY + 303 * 0.4f - 20) / 64; //check a lil infront so minus 20   also 303 is heigh of idle mslug
		int rowTop = rowFront - 1;//checks row near da head
		if (col >= 0 && col < 180 && rowFront < 20 && rowFront >= 0 && rowTop >= 0) {
			if (lvl[rowFront][col] == 'g' || lvl[rowFront][col] == 'h' || lvl[rowFront][col] == 'i') {
				if (lvl[rowTop][col] != 'g' && lvl[rowTop][col] != 'i' && lvl[rowTop][col] != 'h') {
					posY -= 64; climb = true; posX = nex;
					isJump = true; //cout << "Jumped\n";
				}
				else return;
			}
			else posX = nex;
		}
	}
	int row = (posY + 303 * 0.4f) / 64;//adding height takes to feet 
	col = (posX + (327 * 0.4f) / 2.0f) / 64;
	if (row < 20 && col < 180 && col >= 0 && row >= 0) {
		if (lvl[row][col] != 'g' && lvl[row][col] != 'h' && lvl[row][col] != 'i') {
			isJump = true; velY += gravity; posY += velY; //cout << "Fall\n";
		}
		else if (!climb) {
			velY = 0.0f;
			posY = (row) * 64 - (303 * 0.4f); //use height of idle slug as we need const height
		}
	}
}
void Slug001::animate(bool right, float camera_x, bool moving, float camera_y) {
	int numTex = 0, h = texHeights[0], w = texWidths[0];
	if (isJump) { sp.setTexture(jumpTex, true); h = jumpHeight; w = jumpWidth; }
	else if (angle <= 0.0f && moving) {
		spTimer += (1.0f / 60.0f);
		if (spTimer >= 0.2f && spTimer < 0.4f) {//change tex every 0.2s   first move to tex1
			sp.setTexture(tex[1], true); numTex = 1;
		}
		else if (spTimer >= 0.4f && spTimer < 0.6f) {
			sp.setTexture(tex[2], true); numTex = 2;
		}
		else if (spTimer >= 0.6f) {
			sp.setTexture(tex[0], true); spTimer = 0.0f; numTex = 0;
		}
		h = texHeights[numTex]; w = texWidths[numTex];
	}
	else if (angle > 0) {
		if (angle <= 30.0f)numTex = 0;
		else if (angle <= 60.0f)numTex = 1;
		else numTex = 2;
		sp.setTexture(tiltTex[numTex], true);
		h = tiltHeight[numTex]; w = tiltWidth[numTex];
	}
	sp.setOrigin(0, 0);
	vehHeight = h; vehWidth = w;
	if (right) {
		sp.setScale(0.4f, 0.4f);
		sp.setPosition(posX - camera_x, posY - camera_y);
	}
	else {
		sp.setScale(-0.4f, 0.4f);
		sp.setPosition(posX - camera_x + w * 0.4f, posY - camera_y);
	}
}
void Slug001::aiming() {
	if (!occupied)return;
	if (Keyboard::isKeyPressed(Keyboard::W)) { angle += 2.0f; aim += 2.0f; }
	if (Keyboard::isKeyPressed(Keyboard::S)) {
		angle -= 2.0f; aim -= 2.0f;
	}
	if (angle > 90)angle = 90.0f;//clamp as shooting angle calculated laater
	if (angle < 0)angle = 0.0f;
	if (aim >= 360) aim -= 360;
	if (aim < 0) aim += 360;
}
Sprite PlayerVehicle::getSp()const { return sp; }
float Slug001::getAim()const { return aim; }
SlugFlyer::SlugFlyer() {
	tex[0].loadFromFile("25I-0891_25I-0915_Assets/SlugFlyer1.png"); tex[1].loadFromFile("25I-0891_25I-0915_Assets/SlugFlyer2.png");
	tex[2].loadFromFile("25I-0891_25I-0915_Assets/SlugFlyer3.png"); sp.setTexture(tex[0]); texHeights[0] = 245; texWidths[0] = 377;
	texHeights[1] = 245; texWidths[1] = 384; texHeights[2] = 245; texWidths[2] = 388; missileNum = 0; bulletTimer = 0.0f;
	velY = 10.0f; velX = 10.0f;//flys w this speed in all 4 direction
	animTimer = 0.0f; reloadTime = 1 / 4.0f;//same as a pistols
	shootTimer = reloadTime; vehHeight = texHeights[0]; vehWidth = texWidths[0];
}
void SlugFlyer::movement(char** lvl, bool right, bool moving) {
	if (moving) {
		if (right)posX += velX;
		else posX -= velY;
	}
	//add limits for posX as it cannot leave aerial biome
	if (Keyboard::isKeyPressed(Keyboard::W))posY -= velY;
	if (Keyboard::isKeyPressed(Keyboard::S))posY += velY;
	if (posX < 0)posX = 0;
	//if (posX>sumVal) posX=sumVal;
}
void SlugFlyer::animate(bool right, float camera_x, bool moving, float camera_y) {
	if (moving) {
		animTimer += (1.0f / 60.0f); int numTex = 0;
		if (animTimer < 0.2f) numTex = 0;
		else if (animTimer < 0.4f) numTex = 1;
		else if (animTimer < 0.6f) {
			numTex = 2; animTimer = 0.0f;
		}
		sp.setTexture(tex[numTex], true);
		vehHeight = texHeights[numTex];
		vehWidth = texWidths[numTex];
	}
	if (right) {
		sp.setScale(0.4f, 0.4f); sp.setPosition(posX - camera_x, posY - camera_y);
	}
	else {
		sp.setScale(-0.4f, 0.4f); sp.setPosition(posX - camera_x + vehWidth * 0.4f, posY - camera_y);
	}
}
void SlugFlyer::updateTimer() { if (shootTimer < reloadTime)shootTimer += (1.0f / 60.0f); }
void SlugFlyer::fire(Bullet* bullets, bool right, Texture* tex, float camera_x) {
	if (missileNum == 4 || shootTimer < reloadTime)return;
	for (int i = 0; i < 30; i++) {
		if (!bullets[i].active) {
			bullets[i].active = true;
			bullets[i].isRocket = false; bullets[i].isFlame = false; bullets[i].isLaser = false; bullets[i].isBeam = false;
			bullets[i].fromEnemy = false;
			float sp = velX / 2.0f;
			bullets[i].velY = 0; bullets[i].velX = (right ? sp : -sp); bullets[i].damage = 4;//missile shud damage more
			bullets[i].posX = posX + (0.4f * vehWidth) / 2.0f; bullets[i].posY = posY + (0.4f * vehHeight) / 2.0f;
			bullets[i].bul.setTexture(*tex, true); bullets[i].bul.setPosition(bullets[i].posX - camera_x, posY);
			if (right)bullets[i].bul.setScale(0.3f, 0.3f); else bullets[i].bul.setScale(-0.3f, 0.3f);
			missileNum++; shootTimer = 0.0f;
			break;
		}
	}
}
bool PlayerVehicle::getOccupy()const { return occupied; }
SlugMariner::SlugMariner() {
	amVert = 3; amHor = 3; amRev = 3; tex[0].loadFromFile("25I-0891_25I-0915_Assets/SlugMariner.png");
	texHeights[0] = 484; texWidths[0] = 516; velY = 10.0f; velX = 10.0f; sp.setTexture(tex[0]);
	reloadTime = 0.2f; shootTimer = reloadTime; vehWidth = texWidths[0]; vehHeight = texHeights[0];
}
void SlugMariner::movement(char** lvl, bool right, bool moving) {
	if (moving) {
		if (right)posX += velX; else posX -= velX;
	}
	if (Keyboard::isKeyPressed(Keyboard::W))posY -= velY;
	if (Keyboard::isKeyPressed(Keyboard::S))posY += velY;
	//add limits of aquatic biome
	//if (posY < );
	//if(posY>)
}
void SlugMariner::animate(bool right, float camera_x, bool moving, float camera_y) {
	if (right) {
		sp.setScale(0.3f, 0.3f); sp.setPosition(posX - camera_x, posY - camera_y);
	}
	else {
		sp.setScale(-0.3f, 0.3f); sp.setPosition(posX - camera_x + vehWidth * 0.3f, posY - camera_y);
	}
}
void SlugMariner::updateTimer() { if (shootTimer < reloadTime)shootTimer += (1.0f / 60.0f); }
void SlugMariner::fire(Bullet* bullets, bool right, Texture* tex, float camera_x, int type) {
	if (shootTimer < reloadTime)return;
	if (type == 1 && amVert == 0)return; if (type == 2 && amHor == 0)return; if (type == 3 && amRev == 0)return;
	for (int i = 0; i < 30; i++) {
		if (!bullets[i].active) {
			bullets[i].active = true; bullets[i].isRocket = false; bullets[i].isFlame = false; bullets[i].isLaser = false;
			bullets[i].isBeam = false;
			bullets[i].fromEnemy = false; bullets[i].damage = 6; bullets[i].posX = posX + (vehWidth * 0.3f) / 2.0f; bullets[i].posY = posY;
			if (type == 1) {//goes straight vertical
				amVert--;
				bullets[i].velY = -10.0f; bullets[i].velX = 0.0f;
			}
			else if (type == 2) {//straight hprizontal
				amHor--;
				bullets[i].velY = 0; bullets[i].velX = (right ? 10.0f : -10.0f);
			}
			else if (type == 3) {//rev proj +ve parabola inc mag of velY
				amRev--; bullets[i].isRocket = true;//to get the proj motion
				bullets[i].velX = (right ? 10.0f : -10.0f); bullets[i].gravity = -0.5f; bullets[i].velY = -5.0f;
			}
			bullets[i].bul.setTexture(*tex, true); bullets[i].bul.setPosition(bullets[i].posX - camera_x, posY);
			if (right)bullets[i].bul.setScale(0.3f, 0.3f); else bullets[i].bul.setScale(-0.3f, 0.3f);
			shootTimer = 0.0f;
			break;
		}
	}
}
AmphibiousSlug::AmphibiousSlug() {
	mariner = new SlugMariner(); flyer = new SlugFlyer(); normal = new Slug001();
	//all these will exist inside the amphbious slug and it wil choose which one to use
	tex[0].loadFromFile("25I-0891_25I-0915_Assets/AmphSlug.png"); texHeights[0] = 370; texWidths[0] = 674;
	sp.setTexture(tex[0]); occupied = false; current = nullptr;
}
void AmphibiousSlug::updateMode() {
	if (!occupied) {
		current = nullptr; return;
	}
	int col = (posX) / 64;
	if (col >= 0 && col < 60)current = normal;//plains are 0 to 60
	else if (col >= 60 && col < 120)current = mariner;//aquatic comes second
	else if (col >= 120 && col < 180)current = flyer;//last is aerial
	if (current != nullptr) {
		current->spawn(posX, posY);//the changing must result in both vehicles having same posX and posY
		current->setOccupy(true);
	}
}
void AmphibiousSlug::movement(char** lvl, bool right, bool moving) {
	updateMode();//check biome every frame
	if (occupied && current != nullptr) {
		if (current != normal)current->movement(lvl, right, moving);//calls movement of what ever vehicle it is pointing to rn
		else current->movement(lvl, moving, right);
		posX = current->getX(); posY = current->getY();
	}
	else {//if its empty and not used just apply gravity so it remains on the ground
		int row = (posY + (texHeights[0] * 0.3f) / 64);
		int col = (posX + (texWidths[0] * 0.3f) / 2.0f) / 64;
		if (row < 20 && col < 180 && (lvl[row][col] != 'g' && lvl[row][col] != 'h' && lvl[row][col] != 'i')) {
			velY += gravity;
			posY += velY;
		}
		else
			velY = 0;
	}
}
void AmphibiousSlug::animate(bool right, float camera_x, bool moving, float camera_y) {
	if (occupied && current != nullptr)current->animate(right, camera_x, moving, camera_y);
	else {
		if (right) {
			sp.setScale(0.3f, 0.3f); sp.setPosition(posX - camera_x, posY - camera_y);
		}
		else {
			sp.setScale(-0.3f, 0.3f); sp.setPosition(posX - camera_x + vehWidth * 0.3f, posY - camera_y);
		}
	}
}
void AmphibiousSlug::updateTimer() {
	int col = posX / 64; if (col >= 60 && col < 120)mariner->updateTimer();
	if (col >= 120 && col < 180)flyer->updateTimer();
}
void AmphibiousSlug::fire(Bullet* bullets, bool right, Texture* tex, float camera_x, int type) {
	if (!occupied || current == nullptr)return;
	int col = posX / 64; if (col >= 60 && col < 120)mariner->fire(bullets, right, tex, camera_x, type);
	if (col >= 120 && col < 180)flyer->fire(bullets, right, tex, camera_x);
}
Sprite AmphibiousSlug::getSp()const {
	if (occupied && current != nullptr)return current->getSp();
	return sp;
}
AmphibiousSlug::~AmphibiousSlug() { delete flyer; delete normal; delete mariner; }
void AmphibiousSlug::aiming() {
	if (!occupied) return;
	int col = posX / 64;
	if (col >= 0 && col < 60) normal->aiming();
}
Slug001* AmphibiousSlug::getNorm() { return normal; }
void PlayerVehicle::takeDamage(bool immortal, Character* player) {
	hits++;
	if (hits >= 20) {
		active = false; occupied = false;
		player->setVeh(false); if (!immortal) {
			player->setAlive(false); player->setLives(0);
		}
	}
}
bool PlayerVehicle::getActive()const { return active; }



void Game::gamePlay(RenderWindow& window, char** lvl, const int cell_size, int Lwidth, int Lheight, float& camera_x, float& camera_y) {
	totalWorld_width = Lwidth * cell_size; // 180 * 64 = max world while screen is just a subset of it at any point 
	// level working 

	level->drawBg(window, camera_x, camera_y);
	level->drawLevel(window, camera_x, camera_y);
	killed = 0;//as it checks how many enemies killed in each frame
	//CHECK FOR ANY VEHICLE NEARBY
	if (level->getMetalSlug() != nullptr && level->getMariner() != nullptr && level->getFlyer() != nullptr && level->getAmph() != nullptr) {
		float x = abs(level->getMetalSlug()->getX() - player->getX()), y = abs(level->getMetalSlug()->getY() - player->getY());
		if (x < 50.0f && y < 120.0f && !player->getVeh() && level->getMetalSlug()->getActive()) {
			player->setVeh(true); level->getMetalSlug()->setOccupy(true);
		}
		if (!player->getVeh() && level->getFlyer()->getActive()) {
			x = abs(level->getFlyer()->getX() - player->getX()), y = abs(level->getFlyer()->getY() - player->getY());
			if (x < 80.0f && y < 120.0f) {
				player->setVeh(true); level->getFlyer()->setOccupy(true);
			}
		}
		if (!player->getVeh() && level->getMariner()->getActive()) {
			x = abs(level->getMariner()->getX() - player->getX()), y = abs(level->getMariner()->getY() - player->getY());
			if (x < 80.0f && y < 120.0f) {
				player->setVeh(true); level->getMariner()->setOccupy(true);
			}
		}
		if (!player->getVeh() && level->getAmph()->getActive()) {
			x = abs(level->getAmph()->getX() - player->getX()), y = abs(level->getAmph()->getY() - player->getY());
			if (x < 80.0f && y < 120.0f) {
				player->setVeh(true); level->getAmph()->setOccupy(true);
			}
		}
	}
	// run + walk + jump + switch
	if (Keyboard::isKeyPressed(Keyboard::Up) && player->getVeh()) {
		player->setVeh(false); level->getMetalSlug()->setOccupy(false); level->getFlyer()->setOccupy(false);
		level->getMariner()->setOccupy(false); level->getAmph()->setOccupy(false);
	}
	if (!player->getVeh()) {//PLAYER MOVEMENT FUNCTIONS CALLED ONLY WHEN IT IS NOT IN VEHICLE
		if (Keyboard::isKeyPressed(Keyboard::Right) && player->isAlive()) {
			player->right();
			if (Keyboard::isKeyPressed(Keyboard::LShift))//shift + right = run right
				player->run();//running speed over writes the right walk speed
		}
		else if (Keyboard::isKeyPressed(Keyboard::Left) && player->isAlive()) {
			player->left();
			if (Keyboard::isKeyPressed(Keyboard::LShift))
				player->run();//running speed overwrites the left walk speed
		}
		else player->stopRun();//if neither left nor right then speed becomes 0 again
		player->moveX(lvl);//apply the new velocity to the position
		if (!player->isJump()) {
			if (Keyboard::isKeyPressed(Keyboard::Up) && player->isAlive())//jump up
				player->startJump();//here velocity starts as negative
			else if (Keyboard::isKeyPressed(Keyboard::Down) && player->isAlive())//jump down
				player->startDrop();//here velocity starts at 0
			else if (!player->onPlat(lvl))
				player->startDrop();
		}
		if (player->isJump())player->jump(lvl);//jump adds velocity to positiion
		//aiming of the weapon
		if (Keyboard::isKeyPressed(Keyboard::W))//pressing W increases the angle
			player->setAngle(player->getAngle() + 2);//increase by 2 degrees
		if (Keyboard::isKeyPressed(Keyboard::S))//pressing S decreases the angle
			player->setAngle(player->getAngle() - 2);
		if (player->getAngle() < 0)player->setAngle(0);//clamp
		if (player->getAngle() > 90)player->setAngle(90);
		//shooting the weapon
		player->getWeapon()->updateTimer();//shooting timer updates every frame
		player->updateMeleeTimer();//for knife
		if (Keyboard::isKeyPressed(Keyboard::Space) && player->isAlive()) {
			bool knife = false;
			for (int i = 0; i < level->getEnemyCount(); i++) {
				if (level->getEnemy(i)->getAlive()) {
					float xDist = abs(player->getX() - level->getEnemy(i)->getX()), yDist = abs(player->getY() - level->getEnemy(i)->getY());
					if (yDist <= 50 && xDist <= 50 && player->getMelee() <= 0.0f) {
						knife = true;
						bool canPierce = active == 0; // only marco can pierce through the soldiers shield with knife 
						player->setMelee(0.5f);//starts the countdown from 0.5s
						if (active == 3) level->getEnemy(i)->takeDamage(2 * 0.75f, (char*)"Knife", player->getX(), canPierce);
						else level->getEnemy(i)->takeDamage(2, (char*)"Knife", player->getX(), canPierce);
						player->switchWeapSp(-1);
						if (level->getEnemy(i)->getHp() <= 0) {
							score += 50 + level->getEnemy(i)->getScore();
							if (level->getEnemy(i)->getJumping())score += 100;
						}
						break;
					}
				}
			}
			if (active == 2)knife = false;//eri cannot have melee attcaks
			if (player->getEffect() == 2)knife = true;//in mummy mode cannot use weapons
			if (player->getWeapon()->canSh() && !knife) {
				Texture* setting = (player->getAngle() <= 45 ? &bulTex : &verBul);//ptr used to avoid copying bcs that gives white rectangles
				player->switchWeapSp(player->getWeapon()->getType());//switch sprite of weapon
				player->getWeapon()->shoot(player->getX(), player->getY(), player->faceRight(), player->getAngle(), bullets, setting, false);
				player->getWeapon()->resetTimer();//after shooting reset as cannot immediately shoot
				if (!player->getWeapon()->infAmmo() && player->getWeapon()->getAmmo() <= 0) {//once ammo finishes cannot use that weapon again
					//swtich back to default pistol 
					delete player->getWeapon();//delete old weapon to make it inaccesible
					player->setWeapon(new Pistol());//give pointer of new pistol to func
				}
			}
		}
		if (devMode) {
			if (Keyboard::isKeyPressed(Keyboard::Num1)) {//1 PISTOL
				delete player->getWeapon();
				player->setWeapon(new Pistol());
			}
			if (Keyboard::isKeyPressed(Keyboard::Num2)) {//2 HMG
				delete player->getWeapon();
				player->setWeapon(new HMG());
			}
			if (Keyboard::isKeyPressed(Keyboard::Num3)) {//3 FLAMESHOT
				delete player->getWeapon();
				player->setWeapon(new FlameShot());
			}
			if (Keyboard::isKeyPressed(Keyboard::Num4)) {//4 RocketLauncher
				delete player->getWeapon();
				player->setWeapon(new RocketLauncher());
			}
			if (Keyboard::isKeyPressed(Keyboard::Num5)) {//5 LASER
				delete player->getWeapon();
				player->setWeapon(new LaserGun());
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::G) && player->isAlive() && player->getEffect() != 2) {
			if (grenadeKeyReleased) {
				if (player->getGerC() > 0 || player->getFireC() > 0) {//if player has any grenades
					for (int i = 0; i < 15; i++)
						if (!grenades[i].isActive()) {//find forst inactive bullet and throw that
							bool isEri = (active == 2 ? true : false);//eri is at index 2 in allCharacters array
							isEri = isEri || player->getFireC() > 0;//fire bombs thrwon only if player has firebombs or is Eri
							grenades[i].throwGrenade(player->getX(), player->getY(), player->faceRight(), isEri);
							grenadeKeyReleased = false;
							if (player->getFireC() > 0)player->setFireBombCount(player->getFireC() - 1);//if firebombs exist those were used
							else player->setGrenadeCount(player->getGerC() - 1);//otherwise grenades were used
							//grenadeCount is used for eri as well even tho she uses fire bombs
							break;
						}
				}
			}
		}
		else grenadeKeyReleased = true;
		level->getMetalSlug()->animate(true, camera_x, false, camera_y);
		level->getFlyer()->animate(true, camera_x, false, camera_y);
		level->getMariner()->animate(true, camera_x, false, camera_y);
		if (level->getMetalSlug()->getActive())window.draw(level->getMetalSlug()->getSp());
		if (level->getFlyer()->getActive())window.draw(level->getFlyer()->getSp());
		if (level->getMariner()->getActive())window.draw(level->getMariner()->getSp());
		level->getAmph()->animate(true, camera_x, false, camera_y);
		level->getAmph()->movement(lvl, true, false);
		if (level->getAmph()->getActive())window.draw(level->getAmph()->getSp());
	}
	else {//CALL VEHICLE MOVEMENTS FUNCTION IF NOT ON FOOT
		bool moving = false, right = player->faceRight();
		if (Keyboard::isKeyPressed(Keyboard::Right)) { moving = true; right = true; player->setRight(true); }
		if (Keyboard::isKeyPressed(Keyboard::Left)) { moving = true; right = false; player->setRight(false); }
		if (level->getMetalSlug()->getOccupy()) {
			level->getMetalSlug()->movement(lvl, moving, right);
			level->getMetalSlug()->aiming();
			level->getMetalSlug()->animate(right, camera_x, true, camera_y);
			player->setX(level->getMetalSlug()->getX());//player and veh have same coords
			player->setY(level->getMetalSlug()->getY());
			window.draw(level->getMetalSlug()->getSp());
			level->getFlyer()->animate(true, camera_x, false, camera_y);
			if (level->getFlyer()->getActive())window.draw(level->getFlyer()->getSp());
			level->getMariner()->animate(true, camera_x, false, camera_y);
			if (level->getMariner()->getActive())window.draw(level->getMariner()->getSp());
			if (Keyboard::isKeyPressed(Keyboard::Space)) {//rapid fire so no timer
				Texture* ty = (abs(level->getMetalSlug()->getAim() <= 45) ? &bulSlug : &verBulSlug);
				player->getWeapon()->shoot(level->getMetalSlug()->getX(), level->getMetalSlug()->getY(), player->faceRight(), level->getMetalSlug()->getAim(), bullets, ty, false);
			}
			level->getAmph()->animate(true, camera_x, false, camera_y);
			level->getAmph()->movement(lvl, true, false);//for gravity
			if (level->getAmph()->getActive())window.draw(level->getAmph()->getSp());
		}
		else if (level->getFlyer()->getOccupy()) {
			level->getFlyer()->movement(lvl, right, moving);
			level->getFlyer()->animate(right, camera_x, moving, camera_y);
			level->getFlyer()->updateTimer();//for the shppting
			if (Keyboard::isKeyPressed(Keyboard::Space))level->getFlyer()->fire(bullets, right, &bulSlug, camera_x);
			player->setX(level->getFlyer()->getX()); player->setY(level->getFlyer()->getY());
			window.draw(level->getFlyer()->getSp());
			level->getMetalSlug()->animate(true, camera_x, false, camera_y);
			if (level->getMetalSlug()->getActive())window.draw(level->getMetalSlug()->getSp());
			level->getMariner()->animate(true, camera_x, false, camera_y);
			if (level->getMariner()->getActive())window.draw(level->getMariner()->getSp());
			level->getAmph()->animate(true, camera_x, false, camera_y);
			level->getAmph()->movement(lvl, true, false);//for gravity
			if (level->getAmph()->getActive())window.draw(level->getAmph()->getSp());
		}
		else if (level->getMariner()->getOccupy()) {
			level->getMariner()->movement(lvl, right, moving);
			level->getMariner()->animate(right, camera_x, moving, camera_y);
			level->getMariner()->updateTimer();
			if (Keyboard::isKeyPressed(Keyboard::Space))//Space does vert shooting
				level->getMariner()->fire(bullets, right, &verBulSlug, camera_x, 1);
			else if (Keyboard::isKeyPressed(Keyboard::LShift))//shoft does hori shooting
				level->getMariner()->fire(bullets, right, &bulSlug, camera_x, 2);
			else if (Keyboard::isKeyPressed(Keyboard::R))//R does rev projec
				level->getMariner()->fire(bullets, right, &bulSlug, camera_x, 3);
			player->setX(level->getMariner()->getX()); player->setY(level->getMariner()->getY());
			level->getMetalSlug()->animate(true, camera_x, false, camera_y);
			if (level->getMetalSlug()->getActive())window.draw(level->getMetalSlug()->getSp());
			level->getFlyer()->animate(true, camera_x, false, camera_y);
			if (level->getFlyer()->getActive())window.draw(level->getFlyer()->getSp());
			window.draw(level->getMariner()->getSp());
			level->getAmph()->animate(true, camera_x, false, camera_y);
			level->getAmph()->movement(lvl, true, false);//for gravity
			if (level->getAmph()->getActive())window.draw(level->getAmph()->getSp());
		}
		else if (level->getAmph()->getOccupy()) {
			level->getAmph()->movement(lvl, right, moving);
			level->getAmph()->aiming();
			level->getAmph()->animate(right, camera_x, moving, camera_y);
			level->getAmph()->updateTimer();
			player->setX(level->getAmph()->getX());
			player->setY(level->getAmph()->getY());
			window.draw(level->getAmph()->getSp());
			level->getMetalSlug()->animate(true, camera_x, false, camera_y);
			if (level->getMetalSlug()->getActive())window.draw(level->getMetalSlug()->getSp());
			level->getFlyer()->animate(true, camera_x, false, camera_y);
			if (level->getFlyer()->getActive())window.draw(level->getFlyer()->getSp());
			level->getMariner()->animate(true, camera_x, false, camera_y);
			if (level->getMariner()->getActive())window.draw(level->getMariner()->getSp());
			int col = level->getAmph()->getX() / 64;
			if (Keyboard::isKeyPressed(Keyboard::Space)) {
				if (col < 60) {
					Texture* ty = (abs(level->getAmph()->getNorm()->getAim() <= 45) ? &bulSlug : &verBulSlug);
					player->getWeapon()->shoot(level->getAmph()->getX(), level->getAmph()->getY(), player->faceRight(), level->getAmph()->getNorm()->getAim(), bullets, ty, false);
				}
				else {
					level->getAmph()->fire(bullets, right, &bulSlug, camera_x, 1);
				}
			}
			if (col >= 60 && col < 120) {
				if (Keyboard::isKeyPressed(Keyboard::LShift)) level->getAmph()->fire(bullets, right, &bulSlug, camera_x, 2);
				if (Keyboard::isKeyPressed(Keyboard::R)) level->getAmph()->fire(bullets, right, &bulSlug, camera_x, 3);
			}
		}
	}
	player->updateStateTimer();//upadtes the state timer of player after every hit

	if (player->isJump())player->jump(lvl);//jump adds velocity to positiion
	if (Keyboard::isKeyPressed(Keyboard::Z) && !switched) {
		switched = true; switchCharacter();
	}
	else if (!Keyboard::isKeyPressed(Keyboard::Z))switched = false;
	//fusion comp lpgic
	if (Keyboard::isKeyPressed(Keyboard::F) && !compUsed) {
		compUsed = true;
		Character* src[3];//fuse all other characters except the current player
		for (int i = 0, j = 0; i < 4; i++)
			if (allCharacters[i] != player) {//ptrs are compared as player points to any one of allCharacters indices
				src[j] = allCharacters[i]; j++;
			}
		companion = new FusionCompanion(src, 3, player->getX(), player->getY());
	}
	if (companion != nullptr && companion->isAlive()) {
		companion->update(lvl, player->getX(), level->getEnemies(), level->getEnCount(), bullets, &bulTex, player->getY());
		if (companion->faceRight()) {
			companion->getImg().sp.setScale(companion->getImg().scale_x, companion->getImg().scale_y);
			companion->getImg().sp.setPosition(companion->getX() - camera_x, companion->getY() - camera_y);
		}
		else {
			companion->getImg().sp.setScale(-companion->getImg().scale_x, companion->getImg().scale_y);
			companion->getImg().sp.setPosition(companion->getX() - camera_x + companion->getImg().width, companion->getY() - camera_y);
		}
		window.draw(companion->getImg().sp);
	}
	else if (companion != nullptr && !companion->isAlive()) { delete companion; companion = nullptr; }

	// now for the level logic (so that one level starts after the other)
	if (isLevelComplete()) // checks if all enemy forces are dead 
	{
		static int framesTillLevel = 0;
		framesTillLevel++;

		// displying the level complete text
		Text completion;
		completion.setFont(gameFont);
		completion.setString("LEVEL COMPLETE");
		completion.setCharacterSize(75);
		completion.setFillColor(Color::Magenta);
		completion.setPosition(500, 350);
		window.draw(completion);

		if (framesTillLevel >= 120) // 120 frames means 2 seconds as we have 60fps so this text is there for 2 seconds and then second level is displayed 
		{
			delete level;
			level = nullptr;
			if (levelNo == 1)score += 1000;//level clear for survival mode
			if (levelNo < 3)
				levelNo++;

			level = new Level1;  // for now 
			if (levelNo == 2)        //// makes a new level and spwans its components 
				level = new Level2;
			else if (levelNo == 3)
				level = new Level3;

			level->makeLevel();
			level->spawnEnemies();

			framesTillLevel = 0;
		}

		return; // so the rest of the game doesnt continue 
	}

	level->updateEnemies(cell_size, window, player->getX(), player->getY(), camera_x, bullets, &bulTex, grenades, &gerTex, camera_y, player);

	for (int i = 0; i < 30; i++)
		if (bullets[i].active) { window.draw(bullets[i].bul); }
	for (int i = 0; i < 30; i++) {
		if (bullets[i].active && bullets[i].fromEnemy) {
			float xDist = abs(bullets[i].posX - player->getX()), yDist = abs(bullets[i].posY - player->getY());
			if (xDist <= 25 && yDist <= 40) {
				if (!player->getVeh())player->takeDamage(devMode);
				else if (level->getAmph()->getOccupy())level->getAmph()->takeDamage(devMode || active == 1, player);
				else if (level->getMariner()->getOccupy())level->getMariner()->takeDamage(devMode || active == 1, player);
				else if (level->getFlyer()->getOccupy())level->getFlyer()->takeDamage(devMode || active == 1, player);
				else if (level->getMetalSlug()->getOccupy())level->getMetalSlug()->takeDamage(devMode || active == 1, player);
				bullets[i].active = false;
			}
		}
	}
	if (player->isFat()) {
		for (int i = 0; i < 30; i++) {
			if (bullets[i].active && bullets[i].fromEnemy)applyFannumTax(bullets[i]);
		}
	}
	level->checkEnemyCollisions(bullets, player, camera_x, grenades, window, &gerTex, &poolTex, foods, &turkTex, &fruTex, camera_y,
		active, &crateTex, score, killed);//upadtes and moves bullets as well
	if (killed == 2)score += 200;//multikill
	else if (killed >= 3)score += 300 + (killed - 3) * 50;//massacre
	for (int i = 0; i < 10; i++) {
		if (foods[i].isActive()) {//update the active food items abd check for their collisions
			foods[i].update(lvl, camera_x);//this makes it fall and checks if it got left behind
			foods[i].getSp().setPosition(foods[i].getX() - camera_x, foods[i].getY() - camera_y);
			foods[i].getSp().setScale(0.3f, 0.3f);
			window.draw(foods[i].getSp());
			if (foods[i].collision(player->getX(), player->getY()))
				foods[i].effectPlayer(player);//deactivates and changes satur
		}
	}
}

// ENEMY VEHICLES 
FlyingTara::FlyingTara(double x, double y)
{
	name = new char[11];
	strCopy(name, "FlyingTara");
	hp = 5;
	positionX = x;
	positionY = y;
	speed = 3.0;
	attackRange = 200;
	facingRight = false;
	isAlive = true;
	playerState = 0;
	gravity = 0;
	velocityY = 0;
	onGround = false;
	isJumping = false;
	spritedata.raw_img_x = 239;
	spritedata.raw_img_y = 119;
	spritedata.scale_x = 0.7;
	spritedata.scale_y = 0.7;
	cooldown = 0.0f;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/FlyingTara_idle.png");
	walkTex[0].loadFromFile("25I-0891_25I-0915_Assets/FlyingTara_walk1.png");
	walkTex[1].loadFromFile("25I-0891_25I-0915_Assets/FlyingTara_walk2.png");
	attackTex.loadFromFile("25I-0891_25I-0915_Assets/FlyingTara_idle.png");
	dieTex.loadFromFile("25I-0891_25I-0915_Assets/FlyingTara_die.png");
}

void FlyingTara::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isAlive) return;
	float stop = 12.0;

	if (abs(player_x - positionX) <= stop)
	{
		playerState = 0;
		return;
	}

	if (player_x < positionX)
	{
		positionX -= speed;
		facingRight = false;
	}
	else if (player_x > positionX)
	{
		positionX += speed;
		facingRight = true;
	}

	playerState = 1;
}

void FlyingTara::die()
{
	isAlive = false;
	playerState = 3;
	speed = 0;
}

void FlyingTara::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive) return;
	hp -= d;
	if (hp <= 0)
	{
		die();
	}
}

void FlyingTara::draw(RenderWindow& window, float camera_x, float camera_y)
{
	if (playerState == 0)
		sprite.setTexture(idleTex);
	else if (playerState == 1)
	{
		texCounter++;
		if (texCounter >= frameLimit)
		{
			texFrame++;
			texCounter = 0;

			if (texFrame >= 2)
				texFrame = 0;
		}
		sprite.setTexture(walkTex[texFrame]);
	}
	else if (playerState == 2)
	{
		texCounter = 0;
		texFrame = 0;
		sprite.setTexture(attackTex);
	}
	else if (playerState == 3)
	{
		texCounter = 0;
		texFrame = 0;
		sprite.setTexture(dieTex);
	}

	if (facingRight)
	{
		sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
	}
	else
	{
		sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x, positionY - camera_y);
	}
	window.draw(sprite);
}

void FlyingTara::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* pllayer)
{
	if (!isAlive) return;

	if (burnTimer > 0.0f) burnTimer -= 1.0f / 60.0f;
	if (burnTimer < 0.0f) burnTimer = 0.0f;

	if (cooldown > 0)
	{
		cooldown -= 1.0f / 60.0f; // 60 fps 
	}

	if (isRange(player_x))
	{
		if (abs(player_x - positionX) <= attackRange && cooldown <= 0)
		{
			int i = 0;
			while (i < 15 && grenades[i].isActive())
			{
				i++;
			}

			if (i < 15)
			{
				grenades[i].addTex(greTex);
				grenades[i].dropGrenade(player_x, positionY + spritedata.height + 80); // at the player's middle directly
				grenades[i].setFromEnemy(true);
			} // + 80 so it doesnt blwo itself up 
			cooldown = 1.8f;
		}

		move(cell_size, lvl, Lwidth, Lheight, player_x);
		draw(window, camera_x, camera_y);
	}
}

RebelSoldier::~RebelSoldier()
{
	delete pistol;
	pistol = nullptr;
}

ShieldedSoldier::~ShieldedSoldier()
{
	delete pistol;
	pistol = nullptr;
}

BazookaSoldier::~BazookaSoldier()
{
	delete rocketLauncher;
	rocketLauncher = nullptr;
}

Zombie::~Zombie()
{
	delete pistol;
	pistol = nullptr;
}

Martian::~Martian()
{
	delete pistol;
	pistol = nullptr;
}

Aerial::~Aerial()
{
	delete soldier;
	soldier = nullptr;
}

Bradley::Bradley(double x, double y)
{
	name = new char[8];
	strCopy(name, "Bradley");
	hp = 5;
	positionX = x;
	positionY = y;
	speed = 3.0;
	attackRange = 800;
	facingRight = false;
	isAlive = true;
	playerState = 0;
	gravity = 2.0;
	velocityY = 0;
	onGround = false;
	isJumping = false;
	launcher = new RocketLauncher();
	spritedata.raw_img_x = 506;
	spritedata.raw_img_y = 493;
	spritedata.scale_x = 0.3;
	spritedata.scale_y = 0.3;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/bradley_idle.png");
}

void Bradley::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isAlive) return;

	// HORIZONTAL MOVEMENT 
	// making it move left or right on basis of the player 
	double offset_x;
	if (player_x < positionX) // left 
	{
		offset_x = positionX - speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size; // only the lower body not the feet or else it will jump continuously 
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + 35) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
		{
		}
		else
		{
			// setting value 
			positionX = offset_x;
		}
		facingRight = false;
	}
	else if (player_x > positionX)
	{
		offset_x = positionX + speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size;
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + spritedata.width - 1) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
		{
			// do nothing 
		}
		else
		{
			// setting value 
			positionX = offset_x;
		}
		facingRight = true;
	}
}

void Bradley::die()
{
	isAlive = false;
	speed = 0;
}

void Bradley::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive) return;
	hp -= d;
	if (hp <= 0)
	{
		die();
	}
}

void Bradley::draw(RenderWindow& window, float camera_x, float camera_y)
{
	sprite.setTexture(idleTex);

	if (facingRight)
	{
		sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
	}
	else
	{
		sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x, positionY - camera_y);
	}
	window.draw(sprite);
}

void Bradley::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	if (!isAlive) return;
	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping

	// VERTICAL MOVEMENT 
	double terminal_velocity = 20.0;

	velocityY += gravity;
	if (velocityY > terminal_velocity) velocityY = terminal_velocity; // clamping so valocity doesnt increase beyond certain point 
	double offset_y = velocityY + positionY;

	// now we check its bottom to see if it is ground if not it will start falling 
	int x1 = positionX / cell_size;
	int x2 = (positionX + spritedata.width / 2) / cell_size;
	int x3 = (positionX + spritedata.width) / cell_size;
	int y = (offset_y + spritedata.height) / cell_size;

	if (x1 < 0) x1 = 0; if (x1 >= Lwidth) x1 = Lwidth - 1;
	if (x2 < 0) x2 = 0; if (x2 >= Lwidth) x2 = Lwidth - 1;
	if (x3 < 0) x3 = 0; if (x3 >= Lwidth) x3 = Lwidth - 1;
	if (y < 0) y = 0;  if (y >= Lheight) y = Lheight - 1;

	if (lvl[y][x1] != ' ' || lvl[y][x2] != ' ' || lvl[y][x3] != ' ')
	{
		// then there is ground below the sprite 
		isJumping = false;
		onGround = true;
		velocityY = 0;
		positionY = (y * cell_size) - spritedata.height;    // marking down as the position where it has settled 
	}
	else
	{
		onGround = false;
		positionY = offset_y;
	}

	float stop = 650.0;
	if (isRange(player_x))
	{
		launcher->updateTimer();  // time will increase until it can shoot 

		if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= 120)
		{
			playerState = 2; // 2 represents attacking 

			if (player_x < positionX)
				facingRight = false;
			else
				facingRight = true;

			if (launcher->canSh())
			{
				launcher->shoot(positionX, positionY, facingRight, 45.0, bullets, bulTex, true);
				launcher->resetTimer(); // resets for cooldown
			}
		}

		// then goes to movement 
		if (abs(player_x - positionX) > stop)
			move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement of infantry enemies 

		// LASTLY DRAW 
		draw(window, camera_x, camera_y);
	}
}

Sub::Sub(double x, double y)
{
	name = new char[4];
	strCopy(name, "Sub");
	hp = 5;
	positionX = x;
	positionY = y;
	speed = 3.0;
	attackRange = 400;
	facingRight = false;
	isAlive = true;
	playerState = 0;
	gravity = 0;
	velocityY = 0;
	onGround = false;
	isJumping = false;
	launcher = new RocketLauncher();
	spritedata.raw_img_x = 670;
	spritedata.raw_img_y = 373;
	spritedata.scale_x = 0.3;
	spritedata.scale_y = 0.3;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/sub_idle.png");
}

void Sub::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isAlive) return;

	// HORIZONTAL MOVEMENT 
	// making it move left or right on basis of the player 
	double offset_x;
	if (player_x < positionX) // left 
	{
		offset_x = positionX - speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size; // only the lower body not the feet or else it will jump continuously 
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + 35) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		if (lvl[y1][x] == ' ' || lvl[y2][x] == ' ' || lvl[y3][x] == ' ')
		{
			positionX = offset_x;
		}
		facingRight = false;
	}
	else if (player_x > positionX)
	{
		offset_x = positionX + speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size;
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + spritedata.width - 1) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		if (lvl[y1][x] == ' ' || lvl[y2][x] == ' ' || lvl[y3][x] == ' ')
		{
			positionX = offset_x;
		}
		facingRight = true;
	}
}

void Sub::die()
{
	isAlive = false;
	speed = 0;
}

void Sub::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive) return;
	hp -= d;
	if (hp <= 0)
	{
		die();
	}
}

void Sub::draw(RenderWindow& window, float camera_x, float camera_y)
{
	sprite.setTexture(idleTex);

	if (facingRight)
	{
		sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
	}
	else
	{
		sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x, positionY - camera_y);
	}
	window.draw(sprite);
}

void Sub::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character* player)
{
	if (!isAlive) return;
	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping
	float stop = 100.0;
	if (isRange(player_x))
	{
		launcher->updateTimer();  // time will increase until it can shoot 

		if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= 120)
		{
			playerState = 2; // 2 represents attacking 

			if (player_x < positionX)
				facingRight = false;
			else
				facingRight = true;

			if (launcher->canSh())
			{
				launcher->shoot(positionX, positionY, facingRight, 45.0, bullets, bulTex, true);
				launcher->resetTimer(); // resets for cooldown
			}
		}

		// then goes to movement 
		if (abs(player_x - positionX) > stop)
			move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement of infantry enemies 

		// LASTLY DRAW 
		draw(window, camera_x, camera_y);
	}
}

Bradley::~Bradley()
{
	delete launcher;
	launcher = nullptr;
}

Sub::~Sub()
{
	delete launcher;
	launcher = nullptr;
}

// start 
void Level2::makeLevel()
{
	int pStart = 0, pEnd = 60, aqStart = 60, aqEnd = 120, arStart = 120, arEnd = width;

	// plains 
	for (int i = pStart; i < pEnd; i++)
	{
		lvl[height - 1][i] = 'g';
	}
	// obstacles 
	for (int i = 5; i < 8; i++)
	{
		lvl[height - 2][i] = 'g';
	}
	for (int i = 5; i < 8; i++)
	{
		lvl[height - 3][i] = 'g';
	}
	for (int i = 5; i < 8; i++)
	{
		lvl[height - 4][i] = 'g';
	}
	for (int i = 5; i < 8; i++)
	{
		lvl[height - 4][i] = 'g';
	}

	for (int i = 20; i < 24; i++)
	{
		lvl[height - 2][i] = 'g';
	}
	for (int i = 20; i < 24; i++)
	{
		lvl[height - 3][i] = 'g';
	}
	for (int i = 20; i < 24; i++)
	{
		lvl[height - 4][i] = 'g';
	}
	for (int i = 20; i < 24; i++)
	{
		lvl[height - 4][i] = 'g';
	}

	for (int i = 35; i < 45; i++)
	{
		lvl[height - 7][i] = 'g';
	}

	for (int i = 55; i < pEnd; i++)
	{
		lvl[height - 9][i] = 'g';
	}

	// aquatic 
	for (int i = aqStart; i < aqEnd; i++)
	{
		lvl[height - 1][i] = 'h';
	}
	for (int i = aqStart + 4; i < aqStart + 12; i++)
	{
		lvl[height - 3][i] = 'h';
	}
	for (int i = aqStart + 20; i < aqStart + 30; i++)
	{
		lvl[height - 10][i] = 'h';
	}
	for (int i = aqStart + 12; i < aqStart + 22; i++)
	{
		lvl[height - 10][i] = 'h';
	}
	for (int i = aqStart + 40; i < aqStart + 55; i++)
	{
		lvl[height - 5][i] = 'h';
	}
	for (int i = aqStart + 50; i < aqStart + 58; i++)
	{
		lvl[height - 12][i] = 'h';
	}
	for (int i = aqStart + 4; i < aqStart + 12; i++)
	{
		lvl[height - 15][i] = 'h';
	}
	for (int i = aqStart + 20; i < aqStart + 30; i++)
	{
		lvl[height - 17][i] = 'h';
	}


	// aerial 
	for (int i = arStart; i < arEnd; i++)
	{
		lvl[height - 1][i] = 'i';
	}

	for (int i = arStart + 20; i < arStart + 30; i++)
	{
		lvl[height - 9][i] = 'i';
	}

	for (int i = arStart + 35; i < arStart + 47; i++)
	{
		lvl[height - 14][i] = 'i';
	}


	// peak 2 
	for (int i = arStart + 35; i < arStart + 45; i++)
	{
		lvl[height - 4][i] = 'i';
	}
	for (int i = arStart + 33; i < arStart + 46; i++)
		lvl[height - 6][i] = 'i';

	for (int i = arStart + 36; i < arStart + 44; i++)
		lvl[height - 8][i] = 'i';

	for (int i = arStart + 38; i < arStart + 42; i++)
		lvl[height - 10][i] = 'i';

	// peak 3 
	for (int i = arStart + 52; i < arStart + 60; i++)
		lvl[height - 13][i] = 'i';

	for (int i = arStart + 54; i < arStart + 58; i++)
		lvl[height - 15][i] = 'i';
	mSlug = new Slug001();
	mSlug->spawn(40 * 64, 13 * 64 - (303 * 0.4f));
	flyer = new SlugFlyer();
	flyer->spawn(175 * 64, 5 * 64 - (245 * 0.4f));
	mariner = new SlugMariner();
	mariner->spawn(65 * 64, 17 * 64 - (0.3f * 484));
	prisonerAerial = 1; prisonerAquatic = 1; prisonerPlains = 2;
	crates = new SupplyCrate[prisonerAerial + prisonerAquatic + prisonerPlains];
	prisoners = new Prisoner[prisonerAerial + prisonerAquatic + prisonerPlains];
	prisoners[0].spawn(160 * 64, 6 * 64 - (134), &prisTex);//aerial
	prisoners[1].spawn(22 * 64, 16 * 64 - (134), &prisTex);//plains
	prisoners[2].spawn(58 * 64, 11 * 64 - (134), &prisTex);//plains
	prisoners[3].spawn(75 * 64, 10 * 64 - (134), &prisTex);//aquatc
	amphSlug = new AmphibiousSlug();
	amphSlug->spawn(100 * 64, 19 * 64 - (370 * 0.3f));
}
// end 

void Level2::drawLevel(RenderWindow& window, float camera_x, float camera_y)
{
	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{
			if (lvl[i][j] == 'g')
				blockSprite.setTexture(plainBlockTex);
			else if (lvl[i][j] == 'h')
				blockSprite.setTexture(aquaticBlockTex);
			else if (lvl[i][j] == 'i')
				blockSprite.setTexture(aerialBlockTex);
			else
				continue;

			blockSprite.setPosition(j * cell_size - camera_x, i * cell_size - camera_y);
			window.draw(blockSprite);
		}
	}
}

// start 
void Level2::spawnEnemies()
{
	int count = 0;
	enemyCount = 43;
	enemies = new Enemy * [enemyCount];

	// plains 
	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(10 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(16 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new ShieldedSoldier(0, 0);
	enemies[count]->setPosition(25 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new GrenadeSoldier(0, 0);
	enemies[count]->setPosition(38 * 64, (height - 7) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new BazookaSoldier(0, 0);
	enemies[count]->setPosition(38 * 64, (height - 9) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new MummyWarrior(0, 0);
	enemies[count]->setPosition(30 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new MummyWarrior(0, 0);
	enemies[count]->setPosition(41 * 64, (height - 7) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new MummyWarrior(0, 0);
	enemies[count]->setPosition(56 * 64, (height - 9) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(32 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(34 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(36 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(44 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(46 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(50 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(54 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(60 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(55 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	// aquatic 
	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(66 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(74 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new ShieldedSoldier(0, 0);
	enemies[count]->setPosition(82 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new ShieldedSoldier(0, 0);
	enemies[count]->setPosition(82 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new GrenadeSoldier(0, 0);
	enemies[count]->setPosition(85 * 64, (height - 10) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new BazookaSoldier(0, 0);
	enemies[count]->setPosition(107 * 64, (height - 5) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(88 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(90 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(92 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(100 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(102 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(104 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(110 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(112 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(114 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	// aerial 
	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(126 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(132 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new ShieldedSoldier(0, 0);
	enemies[count]->setPosition(138 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new GrenadeSoldier(0, 0);
	enemies[count]->setPosition(143 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new BazookaSoldier(0, 0);
	enemies[count]->setPosition(156 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(130 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(134 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(146 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(150 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Martian(0, 0);
	enemies[count]->setPosition(145 * 64, (height - 16) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Martian(0, 0);
	enemies[count]->setPosition(165 * 64, (height - 16) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	// enemy vehicle 
	enemyVehicleCount = 4;
	enemyVehicles = new EnemyVehicle * [enemyVehicleCount];
	int vcount = 0;
	enemyVehicles[vcount] = new Bradley(0, 0);
	enemyVehicles[vcount]->setPosition(8 * 64, (height - 1) * 64 - enemyVehicles[vcount]->getHeight());
	enemyVehicles[vcount++]->setMoves(plainStart, plainEnd);

	enemyVehicles[vcount] = new Sub(0, 0);
	enemyVehicles[vcount]->setPosition(98 * 64, (height - 1) * 64 - enemyVehicles[vcount]->getHeight());
	enemies[vcount++]->setMoves(aquaticStart, aquaticEnd);


	enemyVehicles[vcount] = new FlyingTara(0, 0);
	enemyVehicles[vcount]->setPosition(136 * 64, (height - 11) * 64 - enemyVehicles[vcount]->getHeight());
	enemies[vcount++]->setMoves(aerialStart, aerialEnd);

	enemyVehicles[vcount] = new FlyingTara(0, 0);
	enemyVehicles[vcount]->setPosition(172 * 64, (height - 13) * 64 - enemyVehicles[vcount]->getHeight());
	enemies[vcount++]->setMoves(aerialStart, aerialEnd);
}
// end 

// start 
void Level3::makeLevel()
{
	int pStart = 0, pEnd = 60, aqStart = 60, aqEnd = 120, arStart = 120, arEnd = width;

	// plains 
	for (int i = pStart; i < pEnd; i++)
	{
		lvl[height - 1][i] = 'g';
	}
	// obstacles 
	for (int i = 7; i < 10; i++)
	{
		lvl[height - 2][i] = 'g';
		lvl[height - 3][i] = 'g';
		lvl[height - 4][i] = 'g';
	}

	for (int i = 30; i < 34; i++)
	{
		lvl[height - 2][i] = 'g';
		lvl[height - 3][i] = 'g';
		lvl[height - 4][i] = 'g';
	}

	for (int i = 35; i < 45; i++)
	{
		lvl[height - 7][i] = 'g';
	}

	for (int i = 55; i < pEnd; i++)
	{
		lvl[height - 9][i] = 'g';
	}

	// aquatic 
	for (int i = aqStart; i < aqEnd; i++)
	{
		lvl[height - 1][i] = 'h';
	}
	for (int i = aqStart + 4; i < aqStart + 12; i++)
	{
		lvl[height - 3][i] = 'h';
	}
	for (int i = aqStart + 12; i < aqStart + 22; i++)
	{
		lvl[height - 10][i] = 'h';
	}
	for (int i = aqStart + 40; i < aqStart + 55; i++)
	{
		lvl[height - 5][i] = 'h';
	}
	for (int i = aqStart + 50; i < aqStart + 58; i++)
	{
		lvl[height - 12][i] = 'h';
	}
	for (int i = aqStart + 20; i < aqStart + 30; i++)
	{
		lvl[height - 17][i] = 'h';
	}


	// aerial 
	for (int i = arStart; i < arEnd; i++)
	{

		lvl[height - 1][i] = 'i';
	}

	for (int i = arStart + 20; i < arStart + 30; i++)
	{
		lvl[height - 9][i] = 'i';
	}

	for (int i = arStart + 35; i < arStart + 47; i++)
	{
		lvl[height - 14][i] = 'i';
	}


	// peak 2 
	for (int i = arStart + 35; i < arStart + 45; i++)
	{
		lvl[height - 4][i] = 'i';
	}
	for (int i = arStart + 33; i < arStart + 46; i++)
		lvl[height - 6][i] = 'i';

	for (int i = arStart + 36; i < arStart + 44; i++)
		lvl[height - 8][i] = 'i';

	for (int i = arStart + 38; i < arStart + 42; i++)
		lvl[height - 10][i] = 'i';

	mSlug = new Slug001();
	mSlug->spawn(25 * 64, 19 * 64 - (303 * 0.4f));
	flyer = new SlugFlyer();
	flyer->spawn(125 * 64, 19 * 64 - (245 * 0.4f));
	mariner = new SlugMariner();
	mariner->spawn(62 * 64, 19 * 64 - (0.3f * 484));
	prisonerAerial = 2; prisonerAquatic = 2; prisonerPlains = 2;
	crates = new SupplyCrate[prisonerAerial + prisonerAquatic + prisonerPlains];
	prisoners = new Prisoner[prisonerAerial + prisonerAquatic + prisonerPlains];
	prisoners[0].spawn(160 * 64, 10 * 64 - (134), &prisTex);//aerial
	prisoners[1].spawn(145 * 64, 11 * 64 - (134), &prisTex);//aerial
	prisoners[2].spawn(15 * 64, 19 * 64 - (134), &prisTex);//plains
	prisoners[3].spawn(57 * 64, 11 * 64 - (134), &prisTex);//plains
	prisoners[4].spawn(75 * 64, 10 * 64 - (134), &prisTex);//aquatc
	prisoners[5].spawn(85 * 64, 3 * 64 - (134), &prisTex);//aquatc
	amphSlug = new AmphibiousSlug();
	amphSlug->spawn(115 * 64, 19 * 64 - (370 * 0.3f));
}
// end 

void Level3::drawLevel(RenderWindow& window, float camera_x, float camera_y)
{
	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{
			if (lvl[i][j] == 'g')
				blockSprite.setTexture(plainBlockTex);
			else if (lvl[i][j] == 'h')
				blockSprite.setTexture(aquaticBlockTex);
			else if (lvl[i][j] == 'i')
				blockSprite.setTexture(aerialBlockTex);
			else
				continue;

			blockSprite.setPosition(j * cell_size - camera_x, i * cell_size - camera_y);
			window.draw(blockSprite);
		}
	}
}

// start 
void Level3::spawnEnemies()
{
	enemyCount = 54;
	enemies = new Enemy * [enemyCount];
	int count = 0;

	// plains 
	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(12 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(20 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new ShieldedSoldier(0, 0);
	enemies[count]->setPosition(28 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new GrenadeSoldier(0, 0);
	enemies[count]->setPosition(39 * 64, (height - 7) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new BazookaSoldier(0, 0);
	enemies[count]->setPosition(57 * 64, (height - 9) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new MummyWarrior(0, 0);
	enemies[count]->setPosition(18 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new MummyWarrior(0, 0);
	enemies[count]->setPosition(36 * 64, (height - 7) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new MummyWarrior(0, 0);
	enemies[count]->setPosition(42 * 64, (height - 7) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new MummyWarrior(0, 0);
	enemies[count]->setPosition(56 * 64, (height - 9) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(22 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(24 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(26 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(32 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(34 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(36 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(44 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(46 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(48 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(50 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(52 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(54 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(plainStart, plainEnd);

	// aquatic 
	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(66 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(74 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new ShieldedSoldier(0, 0);
	enemies[count]->setPosition(82 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new GrenadeSoldier(0, 0);
	enemies[count]->setPosition(88 * 64, (height - 10) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new BazookaSoldier(0, 0);
	enemies[count]->setPosition(108 * 64, (height - 5) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(84 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(86 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(88 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(94 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(96 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(98 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(102 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(104 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(106 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(112 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(114 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	enemies[count] = new Zombie(0, 0);
	enemies[count]->setPosition(116 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aquaticStart, aquaticEnd);

	// aerial 
	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(126 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new RebelSoldier(0, 0);
	enemies[count]->setPosition(134 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new ShieldedSoldier(0, 0);
	enemies[count]->setPosition(142 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new GrenadeSoldier(0, 0);
	enemies[count]->setPosition(146 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new BazookaSoldier(0, 0);
	enemies[count]->setPosition(160 * 64, (height - 1) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(144 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(154 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(166 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(170 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(150 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(164 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(172 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Paratrooper(0, 0);
	enemies[count]->setPosition(145 * 64, 0);
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Martian(0, 0);
	enemies[count]->setPosition(145 * 64, (height - 16) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Martian(0, 0);
	enemies[count]->setPosition(160 * 64, (height - 16) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);

	enemies[count] = new Martian(0, 0);
	enemies[count]->setPosition(175 * 64, (height - 16) * 64 - enemies[count]->getHeight());
	enemies[count++]->setMoves(aerialStart, aerialEnd);


	// enemy vehicles 
	enemyVehicleCount = 8;
	enemyVehicles = new EnemyVehicle * [enemyVehicleCount];
	int vcount = 0;

	enemyVehicles[vcount] = new Bradley(0, 0);
	enemyVehicles[vcount]->setPosition(plainStart + 1800, (height - 1) * 64 - enemyVehicles[vcount]->getHeight());
	enemyVehicles[vcount++]->setMoves(plainStart, plainEnd);

	enemyVehicles[vcount] = new Bradley(0, 0);
	enemyVehicles[vcount]->setPosition(176 * 64, (height - 1) * 64 - enemyVehicles[vcount]->getHeight());
	enemyVehicles[vcount++]->setMoves(aerialStart, aerialEnd);

	enemyVehicles[vcount] = new Sub(0, 0);
	enemyVehicles[vcount]->setPosition(92 * 64, (height - 4) * 64 - enemyVehicles[vcount]->getHeight());
	enemyVehicles[vcount++]->setMoves(aquaticStart, aquaticEnd);

	enemyVehicles[vcount] = new Sub(0, 0);
	enemyVehicles[vcount]->setPosition(110 * 64, (height - 4) * 64 - enemyVehicles[vcount]->getHeight());
	enemyVehicles[vcount++]->setMoves(aquaticStart, aquaticEnd);

	enemyVehicles[vcount] = new FlyingTara(0, 0);
	enemyVehicles[vcount]->setPosition(136 * 64, (height - 4) * 64 - enemyVehicles[vcount]->getHeight());
	enemyVehicles[vcount++]->setMoves(aerialStart, aerialEnd);

	enemyVehicles[vcount] = new FlyingTara(0, 0);
	enemyVehicles[vcount]->setPosition(166 * 64, (height - 4) * 64 - enemyVehicles[vcount]->getHeight());
	enemyVehicles[vcount++]->setMoves(aerialStart, aerialEnd);

	enemyVehicles[vcount] = new FlyingTara(0, 0);
	enemyVehicles[vcount]->setPosition(170 * 64, (height - 4) * 64 - enemyVehicles[vcount]->getHeight());
	enemyVehicles[vcount++]->setMoves(aerialStart, aerialEnd);

	enemyVehicles[vcount] = new FlyingTara(0, 0);
	enemyVehicles[vcount]->setPosition(154 * 64, (height - 4) * 64 - enemyVehicles[vcount]->getHeight());
	enemyVehicles[vcount++]->setMoves(aerialStart, aerialEnd);
}
// end 
// start 
IronNokona::IronNokona(double x, double y) {
	name = new char[11];
	strCopy(name, "IronNokona");
	hp = 30;
	positionX = x;
	positionY = y;
	speed = 3.0;
	attackRange = 900;
	facingRight = false;
	isAlive = true;
	playerState = 0;
	gravity = 2.0;
	velocityY = 0;
	onGround = false;
	isJumping = false;
	spritedata.raw_img_x = 578;
	spritedata.raw_img_y = 431;
	spritedata.scale_x = 0.4;
	spritedata.scale_y = 0.4;
	launcher = new RocketLauncher();
	flamethrower = new FlameShot();
	cooldown = 0.0;
	going = false;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/ironNokona_idle.png");
}

IronNokona::~IronNokona()
{
	delete launcher;
	delete flamethrower;
}

void IronNokona::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isAlive) return;

	if (going)
	{
		double offset_x = positionX + speed * 2.0;
		positionX = offset_x;
		facingRight = true;

		// in plain so when it reaches its edge it dies 
		if (positionX > (60 * cell_size))
			isAlive = false;

		return;
	}

	// HORIZONTAL MOVEMENT 
	// making it move left or right on basis of the player 
	double offset_x;
	if (player_x < positionX) // left 
	{
		offset_x = positionX - speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size; // only the lower body not the feet or else it will jump continuously 
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + 35) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
		{
		}
		else
		{
			// setting value 
			positionX = offset_x;
		}
		facingRight = false;
	}
	else if (player_x > positionX)
	{
		offset_x = positionX + speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size;
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + spritedata.width - 1) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		if (lvl[y1][x] != ' ' || lvl[y2][x] != ' ' || lvl[y3][x] != ' ')
		{
			// do nothing 
		}
		else
		{
			// setting value 
			positionX = offset_x;
		}
		facingRight = true;
	}
}

void IronNokona::die()
{
	isAlive = false;
	speed = 0;
}

void IronNokona::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive || going) return;

	hp -= d;
	if (!going && hp <= (hp / 2))
	{
		going = true;
		return;
	}
	if (hp <= 0)
		die();
}

void IronNokona::draw(RenderWindow& window, float camera_x, float camera_y)
{
	sprite.setTexture(idleTex);

	if (facingRight)
	{
		sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
	}
	else
	{
		sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x, positionY - camera_y);
	}
	window.draw(sprite);
}

void IronNokona::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*)
{
	if (!isAlive) return;
	if (burnTimer > 0.0f) burnTimer -= 1.0f / 60.0f;
	if (burnTimer < 0.0) burnTimer = 0.0;
	if (cooldown > 0.0) cooldown -= 1.0f / 60.0f;

	// VERTICAL MOVEMENT 
	double terminal_velocity = 20.0;

	velocityY += gravity;
	if (velocityY > terminal_velocity) velocityY = terminal_velocity; // clamping so valocity doesnt increase beyond certain point 
	double offset_y = velocityY + positionY;

	// now we check its bottom to see if it is ground if not it will start falling 
	int x1 = positionX / cell_size;
	int x2 = (positionX + spritedata.width / 2) / cell_size;
	int x3 = (positionX + spritedata.width) / cell_size;
	int y = (offset_y + spritedata.height) / cell_size;

	if (x1 < 0) x1 = 0; if (x1 >= Lwidth) x1 = Lwidth - 1;
	if (x2 < 0) x2 = 0; if (x2 >= Lwidth) x2 = Lwidth - 1;
	if (x3 < 0) x3 = 0; if (x3 >= Lwidth) x3 = Lwidth - 1;
	if (y < 0) y = 0;  if (y >= Lheight) y = Lheight - 1;

	if (lvl[y][x1] != ' ' || lvl[y][x2] != ' ' || lvl[y][x3] != ' ')
	{
		// then there is ground below the sprite 
		isJumping = false;
		onGround = true;
		velocityY = 0;
		positionY = (y * cell_size) - spritedata.height;    // marking down as the position where it has settled 
	}
	else
	{
		onGround = false;
		positionY = offset_y;
	}

	if (going)
	{
		move(cell_size, lvl, Lwidth, Lheight, player_x);
		if (isAlive) draw(window, camera_x, camera_y);
		return;
	}

	float stop = 650.0;
	if (isRange(player_x))
	{
		launcher->updateTimer();  // time will increase until it can shoot 
		flamethrower->updateTimer();

		if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= 200)
		{
			playerState = 2; // 2 represents attacking 

			if (player_x < positionX)
				facingRight = false;
			else
				facingRight = true;

			// for launcher shooting 
			if (launcher->canSh())
			{
				launcher->shoot(positionX, positionY, facingRight, 45.0, bullets, bulTex, true);
				launcher->resetTimer(); // resets for cooldown
			}

			// to use flameshot 
			if (flamethrower->canSh())
			{
				float flame = positionY + spritedata.height * 0.7f;
				flamethrower->shoot(positionX, flame, facingRight, 0.0, bullets, bulTex, true);
				flamethrower->resetTimer(); // resets for cooldown
			}

			// for throwing grenades sometimes 
			if (cooldown <= 0)
			{
				int i = 0;
				while (i < 15 && grenades[i].isActive())
				{
					i++;
				}

				if (i < 15)
				{
					grenades[i].addTex(greTex);
					grenades[i].throwGrenade(positionX, positionY, facingRight, false);
					grenades[i].setOwner(this);
					grenades[i].setFromEnemy(true);
				}

				cooldown = 4.0f; // every 4 secs 
			}
		}

		// then goes to movement 
		if (abs(player_x - positionX) > stop)
			move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement of infantry enemies 

		// LASTLY DRAW 
		draw(window, camera_x, camera_y);
	}
}

HairBuster::HairBuster(double x, double y) {
	name = new char[11];
	strCopy(name, "HairBuster");
	hp = 30;
	positionX = x;
	positionY = y;
	speed = 3.0;
	attackRange = 900;
	facingRight = false;
	isAlive = true;
	playerState = 0;
	gravity = 0.0;
	velocityY = 0;
	onGround = false;
	isJumping = false;
	spritedata.raw_img_x = 471;
	spritedata.raw_img_y = 303;
	spritedata.scale_x = 0.4;
	spritedata.scale_y = 0.4;
	launcher = new RocketLauncher();
	cooldown = 0.0;
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/hairBuster_idle.png");
}

HairBuster::~HairBuster() {
	delete launcher;
}

void HairBuster::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isAlive) return;
	float stop = 12.0;

	if (abs(player_x - positionX) <= stop)
	{
		playerState = 0;
		return;
	}

	if (player_x < positionX)
	{
		positionX -= speed;
		facingRight = false;
	}
	else if (player_x > positionX)
	{
		positionX += speed;
		facingRight = true;
	}

	playerState = 1;
}

void HairBuster::draw(RenderWindow& window, float camera_x, float camera_y)
{
	sprite.setTexture(idleTex);

	if (facingRight)
	{
		sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
	}
	else
	{
		sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x, positionY - camera_y);
	}
	window.draw(sprite);
}

void HairBuster::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*)
{
	if (!isAlive) return;

	if (burnTimer > 0.0f) burnTimer -= 1.0f / 60.0f;
	if (burnTimer < 0.0f) burnTimer = 0.0f;

	if (cooldown > 0)
	{
		cooldown -= 1.0f / 60.0f; // 60 fps 
	}

	float stop = 650.0;
	if (isRange(player_x))
	{
		if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= 200)
		{
			playerState = 2; // 2 represents attacking 

			if (player_x < positionX)
				facingRight = false;
			else
				facingRight = true;

			// for launcher shooting 
			if (launcher->canSh())
			{
				launcher->shoot(positionX, positionY, facingRight, 45.0, bullets, bulTex, true);
				launcher->resetTimer(); // resets for cooldown
			}

			// for throwing grenades sometimes 
			if (cooldown <= 0)
			{
				int i = 0;
				while (i < 15 && grenades[i].isActive())
				{
					i++;
				}

				if (i < 15)
				{
					grenades[i].addTex(greTex);
					grenades[i].throwGrenade(positionX, positionY, facingRight, false);
					grenades[i].setOwner(this);
					grenades[i].setFromEnemy(true);
				}

				cooldown = 4.0f; // every 4 secs 
			}
		}

		// then goes to movement 
		if (abs(player_x - positionX) > stop)
			move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement of infantry enemies 

		// LASTLY DRAW 
		draw(window, camera_x, camera_y);
	}
}

void HairBuster::die()
{
	isAlive = false;
	velocityY = 0;
}

void HairBuster::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive) return;

	hp -= d;
	if (hp <= 0)
		die();
}

SeaSatan::SeaSatan(double x, double y)
{
	name = new char[9];
	strCopy(name, "SeaSatan");
	hp = 30;
	positionX = x;
	positionY = y;
	speed = 3.0;
	attackRange = 900;
	facingRight = false;
	isAlive = true;
	playerState = 0;
	gravity = 0.0;
	velocityY = 0;
	onGround = false;
	isJumping = false;
	spritedata.raw_img_x = 634;
	spritedata.raw_img_y = 394;
	spritedata.scale_x = 0.4;
	spritedata.scale_y = 0.4;
	launcher = new RocketLauncher();
	gun1 = new HMG();
	gun2 = new HMG();
	spritedata.width = spritedata.raw_img_x * spritedata.scale_x;
	spritedata.height = spritedata.raw_img_y * spritedata.scale_y;
	idleTex.loadFromFile("25I-0891_25I-0915_Assets/hairBuster_idle.png");
}

void SeaSatan::move(const int cell_size, char** lvl, int Lwidth, int Lheight, float player_x)
{
	if (!isAlive) return;

	// HORIZONTAL MOVEMENT 
	// making it move left or right on basis of the player 
	double offset_x;
	if (player_x < positionX) // left 
	{
		offset_x = positionX - speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size; // only the lower body not the feet or else it will jump continuously 
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + 35) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		if (lvl[y1][x] == ' ' || lvl[y2][x] == ' ' || lvl[y3][x] == ' ')
		{
			positionX = offset_x;
		}
		facingRight = false;
	}
	else if (player_x > positionX)
	{
		offset_x = positionX + speed;
		int y1 = (positionY) / cell_size;
		int y2 = (positionY + spritedata.height - 5) / cell_size;
		int y3 = (positionY + spritedata.height / 2) / cell_size;
		int x = (offset_x + spritedata.width - 1) / cell_size;

		if (y1 < 0) y1 = 0; if (y1 >= Lheight) y1 = Lheight - 1;
		if (y2 < 0) y2 = 0; if (y2 >= Lheight) y2 = Lheight - 1;
		if (y3 < 0) y3 = 0; if (y3 >= Lheight) y3 = Lheight - 1;
		if (x < 0) x = 0; if (x >= Lwidth) x = Lwidth - 1;

		if (lvl[y1][x] == ' ' || lvl[y2][x] == ' ' || lvl[y3][x] == ' ')
		{
			positionX = offset_x;
		}
	}
}

void SeaSatan::die()
{
	isAlive = false;
	velocityY = 0;
}

void SeaSatan::takeDamage(int d, char* weaponName, float damage_x, bool)
{
	if (!isAlive) return;

	hp -= d;
	if (hp <= 0)
		die();
}

void SeaSatan::draw(RenderWindow& window, float camera_x, float camera_y)
{
	sprite.setTexture(idleTex);

	if (facingRight)
	{
		sprite.setScale(-1 * spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x + spritedata.width, positionY - camera_y);
	}
	else
	{
		sprite.setScale(spritedata.scale_x, spritedata.scale_y);
		sprite.setPosition(positionX - camera_x, positionY - camera_y);
	}
	window.draw(sprite);
}

void SeaSatan::update(const int cell_size, char** lvl, int Lwidth, int Lheight, RenderWindow& window, float player_x, float player_y, float camera_x, Bullet* bullets, Texture* bulTex, Grenade* grenades, Texture* greTex, float camera_y, Character*)
{
	if (!isAlive)
	{
		playerState = 3;
		draw(window, camera_x, camera_y);
		return;
	}
	if (burnTimer > 0.0f)burnTimer = burnTimer - 1.0f / 60.0f;//bring it to zero
	if (burnTimer < 0.0f)burnTimer = 0.0f;//clamping
	float stop = 730.0;
	if (isRange(player_x))
	{
		launcher->updateTimer();  // time will increase until it can shoot 

		if (abs(player_x - positionX) <= attackRange && abs(player_y - positionY) <= 120)
		{
			playerState = 2; // 2 represents attacking 

			if (player_x < positionX)
				facingRight = false;
			else
				facingRight = true;

			if (launcher->canSh())
			{
				launcher->shoot(positionX, positionY, facingRight, 45.0, bullets, bulTex, true);
				launcher->resetTimer(); // resets for cooldown
			}

			if (gun1->canSh())
			{
				gun1->shoot(positionX, positionY, facingRight, 0.0, bullets, bulTex, true); // true means from enemy 
				gun1->resetTimer();
			}

			if (gun2->canSh())
			{
				gun2->shoot(positionX, positionY, facingRight, 0.0, bullets, bulTex, true); // true means from enemy 
				gun2->resetTimer();
			}
		}

		// then goes to movement 
		if (abs(player_x - positionX) > stop)
			move(cell_size, lvl, Lwidth, Lheight, player_x); // generic function for movement of infantry enemies 

		// LASTLY DRAW 
		draw(window, camera_x, camera_y);
	}
}

SeaSatan::~SeaSatan()
{
	delete launcher;
	delete gun1;
	delete gun2;
}

int main() {
	RenderWindow window(VideoMode(screen_x, screen_y), "Metal Slug", Style::Close);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	srand(time(0));

	// adding the music 
	Music bgMusic;
	if (!bgMusic.openFromFile("25I-0891_25I-0915_Assets/bgMusic.ogg"))
	{
		cout << "music not opening" << endl;
	}
	bgMusic.setLoop(true);
	bgMusic.setVolume(50);
	bgMusic.play();

	Texture menuTex;
	menuTex.loadFromFile("25I-0891_25I-0915_Assets/startpage.png");
	Sprite menuSprite;
	menuSprite.setTexture(menuTex);

	int choice = 0;

	float camera_x = 0;
	float camera_y = 0;

	Event ev;
	// loop for the menu 
	while (window.isOpen() && choice == 0)
	{
		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed)
				window.close();
		}

		if (Keyboard::isKeyPressed(Keyboard::Num1))
			choice = 1;
		else if (Keyboard::isKeyPressed(Keyboard::Num2))
			choice = 2;

		window.clear();
		window.draw(menuSprite);
		window.display();
	}

	Game* MetalSlug = nullptr;
	if (choice == 1)      //level,mode
		MetalSlug = new Game(1, 1); // survival mode
	else if (choice == 2)
		MetalSlug = new Game(1, 2); // devmode 

	if (MetalSlug == nullptr)
		return 0;


	float totalWorld_width = MetalSlug->getWidth() * 64;
	float totalWorld_height = MetalSlug->getHeight() * 64;

	// the game loop 
	while (window.isOpen())
	{
		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed)
				window.close();
		}

		if (MetalSlug->getPlayer()->getX() >= screen_x / 2)
		{
			camera_x = MetalSlug->getPlayer()->getX() - screen_x / 2;
		}

		if (camera_x < 0)
			camera_x = 0;

		if (camera_x > totalWorld_width - screen_x)
			camera_x = totalWorld_width - screen_x;

		float threshold = 400.0f;

		if (MetalSlug->getPlayer()->getY() > threshold)
		{
			camera_y = MetalSlug->getPlayer()->getY() - threshold;
		}

		if (camera_y < 0)
			camera_y = 0;

		if (camera_y > totalWorld_height - screen_y)
			camera_y = totalWorld_height - screen_y;
		window.clear();

		MetalSlug->gamePlay(
			window,
			MetalSlug->getLevel()->getLvl(),
			64,
			MetalSlug->getWidth(),
			MetalSlug->getHeight(),
			camera_x,
			camera_y
		);

		MetalSlug->drawPlayer(window, camera_x, camera_y);
		MetalSlug->drawHue(window);

		window.display();
	}
	delete MetalSlug;
	return 0;
}