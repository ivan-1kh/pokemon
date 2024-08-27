#pragma once
#include "PokemonData.h"

void FetchPokemon(CommonPokemonObjects& common, PokemonData* pokemonData);
void FetchPokemonImage(CommonPokemonObjects& common, PokemonData* pokemonData);
void FetchPokemonImageBack(CommonPokemonObjects& common, PokemonData* pokemonData);
PokemonData* FetchPokemonByQuery(CommonPokemonObjects& common, std::string buffer);
