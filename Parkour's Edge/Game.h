#pragma once
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "Math.h"
#include <queue>

class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void Shutdown();

	void AddActor(class Actor* actor);
	void RemoveActor(class Actor* actor);

	Mix_Chunk* GetSound(const std::string& fileName);

	class Renderer* GetRenderer() { return mRenderer; }
	class Player* mPlayer;
	int channel = 0;
	std::vector<class Actor*> mBlocks;
	std::queue<class Checkpoint*> mCheckpoints;
	void SetNextLevel(std::string str) { mNextLevel = str; }
	void LoadNextLevel();

	float mTimer;
	int mCoin;
	std::vector<class SecurityCamera*> mCameras;

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void LoadData();
	void UnloadData();

	std::unordered_map<std::string, Mix_Chunk*> mSounds;

	// All the actors in the game
	std::vector<class Actor*> mActors;
	class Renderer* mRenderer;
	std::string mNextLevel;

	Uint32 mTicksCount;
	bool mIsRunning;
};
