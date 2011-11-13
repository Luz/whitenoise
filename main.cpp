#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>
#include <fstream> //read the urandom and random device
#include <time.h> //use the "second" as seed generator if the random devices do fail
#include <sys/stat.h> //for create directory
#include <signal.h> //for ctrl+c to abort
#include <tclap/CmdLine.h>
 
int beenden = 0; //Program flow

unsigned int generateSeed();
void catcher(int sigtype);

int main(int argc, char* args[])
{
	unsigned int surfaceWidth, surfaceHeight, folderNumber;
	SDL_Surface *surface = NULL;
	char foldername[32];
	char filename[32];
	Uint32 *ptr;

	signal(SIGINT, catcher);
	srand(generateSeed());

	try
	{
		TCLAP::CmdLine cmd("Lustige Programmbeschreibung", ' ', "0.1");
		TCLAP::ValueArg<unsigned int> widthArg("x", "width", "Width in pixels", false, 960, "unsigned int");
		TCLAP::ValueArg<unsigned int> heightArg("y", "height", "Height in pixels", false, 960, "unsigned int");
		TCLAP::ValueArg<unsigned int> foldersArg("n", "amount", "Amount of folder you'd like to fill", false, 1, "unsigned int");

		cmd.add(widthArg);
		cmd.add(heightArg);
		cmd.add(foldersArg);

		cmd.parse(argc, args);
		surfaceWidth = widthArg.getValue();
		surfaceHeight = heightArg.getValue();
		folderNumber = foldersArg.getValue();
	}
	catch (TCLAP::ArgException & e)
	{
		std::cerr << "I don't understand " << e.argId() << ".\n" << e.error() << std::endl;
		return -1;
	}

	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
		return -1;
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, surfaceWidth, surfaceHeight, 24, 0xFF0000, 0x00FF00, 0x0000FF, 0);
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	mkdir("output",0755);
	chdir("output");
	for(unsigned int i=0; i<folderNumber; i++)
	{
		sprintf(foldername,"%.3d",i);
		mkdir(foldername,0755);
		for(unsigned int j=0; j<200; j++)
		{
			ptr = (Uint32 *)surface->pixels;
			for(unsigned int k=0; k<surfaceHeight*surface->pitch/4; k++)
				ptr[k] = rand();
			sprintf(filename,"%.3d/%.3d.bmp",i,j);
			SDL_SaveBMP(surface, filename);
			if (beenden==1)
				break;
		}
		if (beenden==1)
			break;
	}

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
	SDL_FreeSurface(surface);
	SDL_Quit();
	return 0;
}

unsigned int generateSeed()
{
	unsigned int seed, seedX, seedY, seedZ; 
	seedX = 0;
	seedY = 0;
	seedZ = time(0);

	char * tmp;
	const int size = sizeof(int);
	tmp = new char[size];

	using namespace std;
	ifstream file1("/dev/urandom", ios::binary);
	if(file1.is_open())
	{
		file1.read(tmp, size);
		file1.close();
		seedX = *reinterpret_cast<int*>(tmp);
	}

	ifstream file2("/dev/random", ios::binary);
	if(file2.is_open())
	{
		file2.read(tmp, size);
		file2.close();
		seedY = *reinterpret_cast<int*>(tmp);
	}

	delete[] tmp;
	seed = seedX xor seedY xor seedZ;
//	cout << "seed:" << seed << " seedX:" << seedX << " seedY:" << seedY << " seedZ:" << seedZ << endl;
	return seed;
}

void catcher(int sigtype)
{
	beenden=1;
}
