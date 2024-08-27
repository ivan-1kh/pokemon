#include "PokemonData.h"
#include <iostream>
#include <fstream>

//Loads user favorites from database into program memory
void LoadUserFavorites(CommonPokemonObjects& common, std::string fileName)
{
	//Checks if the file exists
	std::ifstream favoritesFile("databases/" + fileName);

	if (favoritesFile.is_open()) //database exists
	{
		std::string line;
		int lineNumber = 0;

		//Reads every line in database to parse favorite Pokemon
		//Pokemon are uniquely identified by name
		while (std::getline(favoritesFile, line))
		{
			if (line.size() >= 2) //if valid pokemon on this line
			{
				for (auto& rec : common.pokemonData)
				{
					if (_strcmpi(rec.name.c_str(), line.c_str()) == 0)
					{
						//Adds to global favorites vector
						rec.favMetadata = { true, lineNumber };
					}
				}
			}

			lineNumber++;
		}

		favoritesFile.close(); //Closes the database file
	}
	else
	{
		//Creates the file if it doesn't exist
		std::ofstream outputFile("databases/" + fileName);

		if (outputFile.is_open())
		{
			//File created successfully
			outputFile.close();
			std::cout << "Favorites DB created: " << fileName << std::endl;
		}
		else
		{
			std::cerr << "Error creating DB file: " << fileName << std::endl;
		}
	}

	common.favoritesLoaded = true;
}

//Adds Pokemon to user favorites database
void AddFavoritePokemon(CommonPokemonObjects& common, std::string fileName, PokemonData* pokemon)
{
	//Checks if already favorited
	if (!common.favoritesLoaded || pokemon->favMetadata.isFav)
		return;

	//Opens favorites database
	std::ifstream favoritesFile("databases/" + fileName); // Open for reading

	if (favoritesFile.is_open()) //Database exists and opened successfully:
	{
		std::string content;
		std::string temp;
		int currentLine = 0;

		//Reads the entire file into a string
		while (std::getline(favoritesFile, temp))
		{
			content += temp + '\n';
			currentLine++;
		}

		content += pokemon->name + '\n';

		favoritesFile.close(); //Closes the *read* stream file

		//Opens the database in *write* mode
		std::ofstream outputFile("databases/" + fileName);

		if (outputFile.is_open())
		{
			//Writes the updated data back to the file
			outputFile << content;

			outputFile.close(); //Closes the *output* stream file
		}

		//Adds Pokemon to favorites in memory as well
		pokemon->favMetadata = { true, currentLine };
	}
}

//Removes Pokemon from user favorites database
void RemoveFavoritePokemon(CommonPokemonObjects& common, std::string fileName, PokemonData* pokemon)
{
	//Checks if favorites data has been loaded into the program
	if (!common.favoritesLoaded)
		return;

	std::ifstream favoritesFile("databases/" + fileName); //Opens for reading

	if (favoritesFile.is_open())
	{
		std::string content;
		std::string temp;
		int currentLine = 0;

		//Reads the entire file into a string
		while (std::getline(favoritesFile, temp))
		{
			if (currentLine != pokemon->favMetadata.favDBLinePointer)
			{
				content += temp + '\n';
			}
			else
			{
				//Skips removed Pokemon's line (deletes from favorites)
				content += '\n';
			}
			currentLine++;
		}

		favoritesFile.close(); //Closes the *read* stream file

		//Opens the database in *write* mode
		std::ofstream outputFile("databases/" + fileName);

		if (outputFile.is_open())
		{
			//Writes the updated data back to the file
			outputFile << content;

			outputFile.close(); //Closes the *output* stream file
		}

		//Removes Pokemon from favorites in memory as well
		pokemon->favMetadata = { false, 0 };
	}
}
