#pragma once
#include "Serializable.h"

struct LevelSettings :public Serializable
{
	std::array<std::string, 6> skyboxTextures;
	bool renderShadows, renderSkybox;
	std::vector<std::string> gameSpaces;
	Vector4 fogColor;
	bool enableFog = false;
	float fogDistance = 500.0f;
	LevelSettings():
		skyboxTextures{"CompiledAssets\\Sprites\\blood-stain-gorge_lf.dds",
	"CompiledAssets\\Sprites\\blood-stain-gorge_rt.dds",
	"CompiledAssets\\Sprites\\blood-stain-gorge_dn.dds",
	"CompiledAssets\\Sprites\\blood-stain-gorge_up.dds",
	"CompiledAssets\\Sprites\\blood-stain-gorge_bk.dds",
	"CompiledAssets\\Sprites\\blood-stain-gorge_ft.dds" },
		renderShadows(true), renderSkybox{ true }
	{
		AddSerializable("SkyboxLeft", skyboxTextures[0]);
		AddSerializable("SkyboxRight", skyboxTextures[1]);
		AddSerializable("SkyboxDown", skyboxTextures[2]);
		AddSerializable("SkyboxUp", skyboxTextures[3]);
		AddSerializable("SkyboxBack", skyboxTextures[4]);
		AddSerializable("SkyboxFront", skyboxTextures[5]);
		AddSerializable("Render Shadows", renderShadows);
		AddSerializable("Render Skybox", renderSkybox);
		AddSerializable("Game Spaces", gameSpaces);
		AddSerializable("Draw Fog", enableFog);
		AddSerializable("Fog Color", fogColor);
		AddSerializable("Fog Distance", fogDistance);
	}
};