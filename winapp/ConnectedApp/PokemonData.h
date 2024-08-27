#pragma once
#include <atomic>
#include <string>
#include <vector>
#include "FavoritePokemonMetadata.h"
#include "AbilityData.h"
#include "MoveData.h"
#include "ItemData.h"

struct PokemonData
{
	//identifiers
	std::string name;
	std::string url;
	int pokemonID;

	//pokemon stats and specs
	int weight;
	std::vector<AbilityData> abilities;
	std::vector<MoveData> moves;
	std::vector<ItemData> items;

	//sprite & texture data
	std::string img;
	std::string img_back;
	int my_image_width = 0;
	int my_image_height = 0;
	void* my_texture = NULL;
	void* my_texture_back = NULL;
	void* my_texture_shiny_back = NULL;
	void* my_texture_hover = NULL;
	bool hover = false;
	bool fetchedImage = false;
	bool fetchedImageBack = false;

	bool showDetails = false; //details window show/hide flag

	FavoritePokemonMetadata favMetadata; //favorite information

	bool fetchedDetails = false;
};

//Common "global" objects & variable across all Pokemon
struct CommonPokemonObjects
{
	int heartImageWidth = 0;
	int heartImageHeight = 0;
	void* heartEmptyTexture = NULL;
	void* heartFullTexture = NULL;

	std::atomic_bool exit_flag = false;
	std::atomic_bool start_download = false;
	std::atomic_bool data_ready = false;
	std::string url;
	PokemonData* current_pokemon;

	bool favoritesLoaded = false;
	bool favoritesFilter = false;
	int pageNumber = 1;

	std::vector<PokemonData> pokemonData;
};
