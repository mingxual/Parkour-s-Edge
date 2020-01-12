//
//  Game.cpp
//  Game-mac
//
//  Created by Sanjay Madhav on 5/31/17.
//  Copyright © 2017 Sanjay Madhav. All rights reserved.
//

#include "Game.h"
#include <algorithm>
#include "Actor.h"
#include <fstream>
#include "Renderer.h"
#include "Random.h"
#include "Player.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "LevelLoader.h"
#include "Checkpoint.h"
#include "Arrow.h"
#include <SDL/SDL_ttf.h>

Game::Game()
:mIsRunning(true), mPlayer(NULL), mRenderer(NULL), mTimer(0.0f), mCoin(0)
{
	
}

bool Game::Initialize()
{
	Random::Init();

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_GetRelativeMouseState(nullptr, nullptr);

	mRenderer = new Renderer(this);
	bool init = mRenderer->Initialize(1024.0f, 768.0f);
	if (!init) return false;
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_AllocateChannels(32);
	Mix_GroupChannels(22, 31, 1);

	TTF_Init();

	LoadData();
	mTicksCount = SDL_GetTicks();
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
		if (mNextLevel.length() != 0) LoadNextLevel();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	for (auto actor : mActors)
	{
		actor->ProcessInput(state);
	}
}

void Game::UpdateGame()
{
	// Compute delta time
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	if (deltaTime > 0.01667f)
	{
		deltaTime = 0.01667f;
	}
	mTicksCount = SDL_GetTicks();
	
	// Make copy of actor vector
	// (iterate over this in case new actors are created)
	std::vector<Actor*> copy = mActors;
	// Update all actors
	for (auto actor : copy)
	{
		actor->Update(deltaTime);
	}

	// Add any actors to destroy to a temp vector
	std::vector<Actor*> destroyActors;
	for (auto actor : mActors)
	{
		if (actor->GetState() == ActorState::Destroy)
		{
			destroyActors.emplace_back(actor);
		}
	}

	// Delete the destroyed actors (which will
	// remove them from mActors)
	for (auto actor : destroyActors)
	{
		delete actor;
	}
}

void Game::GenerateOutput()
{
	// TODO: tell renderer to draw
	mRenderer->Draw();
}

void Game::LoadData()
{
	GetSound("Assets/Sounds/Checkpoint.wav");
	GetSound("Assets/Sounds/Coin.wav");
	GetSound("Assets/Sounds/Jump.wav");
	GetSound("Assets/Sounds/Land.wav");
	GetSound("Assets/Sounds/Laser.wav");
	GetSound("Assets/Sounds/Music.ogg");
	GetSound("Assets/Sounds/Running.wav");
	GetSound("Assets/Sounds/SecurityDetected.wav");
	GetSound("Assets/Sounds/CameraMotor.wav");
	GetSound("Assets/Sounds/CameraMotorStop.wav");

	Matrix4 projection = Matrix4::CreatePerspectiveFOV(1.22f, 1024.0f, 768.0f, 10.0f, 10000.0f);
	mRenderer->SetProjectionMatrix(projection);
	Matrix4 view = Matrix4::CreateLookAt(Vector3(-300.0f, 0.0f, 100.0f), Vector3(20.0f, 0.0f, 0.0f), Vector3::UnitZ);
	mRenderer->SetViewMatrix(view);

	LevelLoader::Load(this, "Assets/Tutorial.json");
	mCheckpoints.front()->SetActive(true);
	Arrow* curr = new Arrow(this, NULL);

	Mix_PlayChannel(-1, GetSound("Assets/Sounds/Music.ogg"), -1);
}

void Game::UnloadData()
{
	// Delete actors
	// Because ~Actor calls RemoveActor, have to use a different style loop
	while (!mActors.empty())
	{
		delete mActors.back();
	}

	// Destroy sounds
	for (auto s : mSounds)
	{
		Mix_FreeChunk(s.second);
	}
	mSounds.clear();
}

Mix_Chunk* Game::GetSound(const std::string& fileName)
{
	Mix_Chunk* chunk = nullptr;
	auto iter = mSounds.find(fileName);
	if (iter != mSounds.end())
	{
		chunk = iter->second;
	}
	else
	{
		chunk = Mix_LoadWAV(fileName.c_str());
		if (!chunk)
		{
			SDL_Log("Failed to load sound file %s", fileName.c_str());
			return nullptr;
		}

		mSounds.emplace(fileName, chunk);
	}
	return chunk;
}

void Game::Shutdown()
{
	UnloadData();
	Mix_CloseAudio();
	// TODO: Delete renderer
	mRenderer->Shutdown();
	delete mRenderer;
	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
	auto iter = std::find(mActors.begin(), mActors.end(), actor);
	if (iter != mActors.end())
	{
		// Swap to end of vector and pop off (avoid erase copies)
		auto iter2 = mActors.end() - 1;
		std::iter_swap(iter, iter2);
		mActors.pop_back();
	}
}

void Game::LoadNextLevel()
{
	while (!mActors.empty())
	{
		delete mActors.back();
	}
	while (mCheckpoints.size() > 0) mCheckpoints.pop();
	LevelLoader::Load(this, mNextLevel);
	mCheckpoints.front()->SetActive(true);
	Arrow* curr = new Arrow(this, NULL);
	mNextLevel.clear();
}