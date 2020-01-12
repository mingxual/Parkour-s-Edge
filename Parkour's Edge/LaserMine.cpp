#include "LaserMine.h"
#include "Game.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "LaserComponent.h"

LaserMine::LaserMine(Game* game, Actor* parent)
	:Actor(game, parent)
{
	mc = new MeshComponent(this);
	mc->SetMesh(GetGame()->GetRenderer()->GetMesh("Assets/LaserMine.gpmesh"));
	lc = new LaserComponent(this);
}
