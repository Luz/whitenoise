#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream> //read the urandom and random device
#include <time.h> //use the "second" as seed generator if the random devices do fail
#include <sys/stat.h> //for create directory
#include <signal.h> //for ctrl+c to abort
#include <tclap/CmdLine.h>
#include <jpeglib.h>
 
int beenden = 0; //Program flow

unsigned int generateSeed();
void catcher(int sigtype);
bool write_jpeg_file( char *filename, unsigned int width, unsigned int height, unsigned char* data );

int main(int argc, char* args[])
{
	using namespace std;
	unsigned int surfaceWidth, surfaceHeight, folderNumber;
	SDL_Surface *surface = NULL;
	char foldername[32];
	char filename[32];
	string compression;
	Uint32 *ptr;

	signal(SIGINT, catcher);
	srand(generateSeed());

	try
	{
		TCLAP::CmdLine cmd("Creates pictures with random content", ' ', "0.1");
		TCLAP::ValueArg<unsigned int> widthArg("x", "width", "Width in pixels", false, 960, "unsigned int");
		TCLAP::ValueArg<unsigned int> heightArg("y", "height", "Height in pixels", false, 960, "unsigned int");
		TCLAP::ValueArg<unsigned int> foldersArg("n", "folders", "Number of folders you'd like to fill", false, 1, "unsigned int");
		vector<string> compressionList;
		compressionList.push_back("bmp");
		compressionList.push_back("jpg");
		TCLAP::ValuesConstraint<string> compressionMethods( compressionList );
		TCLAP::ValueArg<string> compressionArg("c", "compression", "Type of compression to use", false, "bmp", &compressionMethods);

		cmd.add(compressionArg);
		cmd.add(foldersArg);
		cmd.add(heightArg);
		cmd.add(widthArg);

		cmd.parse(argc, args);
		surfaceWidth = widthArg.getValue();
		surfaceHeight = heightArg.getValue();
		folderNumber = foldersArg.getValue();
		compression = compressionArg.getValue();
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
			
			sprintf(filename,"%.3d/%.3d.%s",i,j,compression.c_str());
			if(compression == "bmp")
				SDL_SaveBMP(surface, filename);
			else if(compression == "jpg")
				write_jpeg_file( filename, surface->w, surface->h, (unsigned char*)surface->pixels);

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

/**
* write_jpeg_file writes the raw image data stored in the data buffer
* to a jpeg image with default compression and smoothing options in the file
* specified by filename. Source: http://www.aaronmr.com/en/2010/03/test/
*/
bool write_jpeg_file( char *filename, unsigned int width, unsigned int height, unsigned char* data )
{
	int bytes_per_pixel = 3;
	J_COLOR_SPACE color_space = JCS_RGB;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// this is a pointer to one row of image data
	JSAMPROW row_pointer[1];
	FILE *outfile = fopen( filename, "wb" );

	if ( !outfile )
	{
		fprintf(stderr, "Error opening output jpeg file %s\n!", filename );
		return false;
	}
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	// Setting the parameters of the output file here
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = bytes_per_pixel;
	cinfo.in_color_space = color_space;
	// default compression parameters, we shouldn't be worried about these

	jpeg_set_defaults( &cinfo );
	cinfo.num_components = 3;
	//cinfo.data_precision = 4;
	cinfo.dct_method = JDCT_FLOAT;
	jpeg_set_quality(&cinfo, 15, TRUE);
	// Now do the compression
	jpeg_start_compress( &cinfo, TRUE );
	// like reading a file, this time write one row at a time
	while( cinfo.next_scanline < cinfo.image_height )
	{
		row_pointer[0] = &data[ cinfo.next_scanline * cinfo.image_width * cinfo.input_components];
		jpeg_write_scanlines( &cinfo, row_pointer, 1 );
	}
	// similar to read file, clean up after we're done compressing
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
	fclose( outfile );
	
	return true;
}

