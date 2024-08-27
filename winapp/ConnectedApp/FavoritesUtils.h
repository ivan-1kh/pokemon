#pragma once
#include "PokemonData.h"

void LoadUserFavorites(CommonPokemonObjects& common, std::string fileName);
void AddFavoritePokemon(CommonPokemonObjects& common, std::string fileName, PokemonData* pokemon);
void RemoveFavoritePokemon(CommonPokemonObjects& common, std::string fileName, PokemonData* pokemon);
