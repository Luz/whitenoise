#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <fstream> //read the urandom and random device
#include <boost/lexical_cast.hpp> //convert number to string
#include <sys/stat.h> //for create directory
#include <signal.h> //for ctrl+c to abort
 
using namespace std;

int beenden=0; //Program flow

unsigned int generateSeed();
void catcher(int sigtype);

void die(int retval)
{
	fprintf(stderr, "usage:\t./whitenoise");
	fprintf(stderr, "\t./whitenoise amountoffolders");
	fprintf(stderr, "\t./whitenoise amountoffolders width height");
	exit(retval);
}

int main(int argc, char* args[])
{
	unsigned int surfaceWidth = 2048;
	unsigned int surfaceHeight = 2048;
	unsigned int folderNumber = 4;
	const int SCREEN_BPP = 24;

	SDL_Surface *surface = NULL;
	unsigned int i,j,k;
	char foldername[32];
	char filename[32];
	Uint32 *ptr;

	signal(SIGINT, catcher);
	srand(generateSeed());

	if((argc != 1) && (argc != 2) && (argc != 4))
	{
		die(1);
	}
	if(argc == 2)
	{
		try
		{
			folderNumber = boost::lexical_cast<int>(args[1]);
		}
		catch(boost::bad_lexical_cast&)
		{
			fprintf(stderr, "Foldernumber parameter needs to be numeric!");
			die(1);
		}
	}
	if(argc == 4)
	{
		try
		{
			folderNumber = boost::lexical_cast<int>(args[1]);
			surfaceWidth = boost::lexical_cast<int>(args[2]);
			surfaceHeight = boost::lexical_cast<int>(args[3]);
		}
		catch(boost::bad_lexical_cast&)
		{
			fprintf(stderr, "Width and height parameters needs to be numeric!");
			die(1);
		}
	}

	if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
		return -1;
	surface = SDL_CreateRGBSurface(SDL_SWSURFACE, surfaceWidth, surfaceHeight, SCREEN_BPP, 0xFF0000, 0x00FF00, 0x0000FF, 0);
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);


	mkdir("output",0755);
	chdir("output");
	for(i=0; i<folderNumber; i++)
	{
		sprintf(foldername,"%.3d",i);
		mkdir(foldername,0755);
		for(j=0; j<200; j++)
		{
			ptr = (Uint32 *)surface->pixels;
			for(k=0; k<surfaceHeight*surface->pitch/4; k++)
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
	seedZ = std::time(0);

	char * tmp;
	int size = sizeof(int);
	tmp = new char[size];

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
