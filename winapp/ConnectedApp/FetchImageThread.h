#pragma once
#include "PokemonData.h"

class FetchImageThread
{

public:
	void operator()(CommonPokemonObjects& common, bool back);
	void SetUrl(std::string_view new_url);

private:
	std::string _download_url;

};
