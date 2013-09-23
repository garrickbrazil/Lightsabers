


#ifndef IMAGE_TGA_H
#define IMAGE_TGA_H


#include <fstream>
#include <cstdio>
#include <cstdlib>


// TGA Loader for 24BPP or 32BPP TGA images (does NOT support RLE compression)
// TGA is stored in BGR (Blue-Green-Red) format, so we need to convert this to Red-Green-Blue (RGB).
// See: http://en.wikipedia.org/wiki/Truevision_TGA
class ImageTGA 
{
	public :

		// ...
		bool load( const char* fileName, bool verbose = true ) {
			std::fstream filestr;
			filestr.open( fileName, std::ios::in | std::ios::binary );
			if( filestr.is_open() ) {
				filestr.read( (char*) &header, sizeof( struct ImageHeaderTGA ) );
				if( verbose ) {
					printf( "\nTGA header: %s\n", fileName );
					printf( "\tID length %d: length of the image ID field.\n", header.idLength );
					switch( header.colorMapType ) {
						case 0 : { printf( "\tColor map type 0: image file contains no color map.\n" ); break; }
						case 1 : { printf( "\tColor map type 1: color map is present.\n" ); break; }
						default : { printf( "\tColor map type %d: undefined color map type.\n", header.colorMapType ); break; } }
					switch( header.imageTypeCode ) {
						case 0 : { printf( "\tImage type 0: no image data is present.\n" ); break; }
						case 1 : { printf( "\tImage type 1: uncompressed color-mapped image.\n" ); break; }
						case 2 : { printf( "\tImage type 2: uncompressed true-color image.\n" ); break; }
						case 3 : { printf( "\tImage type 3: uncompressed black-and-white (grayscale) image.\n" ); break; }
						case 9 : { printf( "\tImage type 9: run-length encoded color-mapped image.\n" ); break; }
						case 10 : { printf( "\tImage type 10: run-length encoded true-color image.\n" ); break; }
						case 11 : { printf( "\tImage type 11: run-length encoded black-and-white (grayscale) image.\n" ); break; }
						default : { printf( "\tImage type %d: undefined image type.\n", header.imageTypeCode ); break; } }
					// Skip: color map specification (header.colorMapSpec).
					printf( "\tImage specification:\n" );
					printf( "\t\tOrigin (%d,%d): abs screen coord of low-left corner.\n", header.xOrigin, header.yOrigin );
					printf( "\t\tImage size (%d,%d): size in pixels.\n", header.width, header.height );
					printf( "\t\tPixel depth %d: bits per pixel.\n", header.bpp );
					printf( "\t\tImage descriptor %d: bits storing alpha chn depth and dir.\n\n", header.imageDesc ); }
				// Read pixel data.
				int imageSize = header.width * header.height * header.bpp;
				imageData = (char*) malloc( imageSize );
				filestr.read( (char*) imageData, imageSize );
				if( header.bpp == 24 ) {
					for( int i = 0; i < imageSize; i += 3 ) {
						char c = imageData[ i ];
						imageData[ i ] = imageData[ i+2 ];
						imageData[ i+2 ] = c; } }
				else if( header.bpp == 32 ) {
					for( int i = 0; i < imageSize; i += 4 ) {
						char c = imageData[ i ];
						imageData[ i ] = imageData[ i+2 ];
						imageData[ i+2 ] = c; } }
				filestr.close(); }
			else { return false; }
			return true; };
		
		// ...
		void draw() {
			glPixelStorei( GL_UNPACK_ROW_LENGTH, header.width );
			if( header.bpp == 32 ) {
				glPixelStorei( GL_UNPACK_ALIGNMENT, 2 );
				glDrawPixels( header.width, header.height, GL_RGBA, GL_UNSIGNED_BYTE, imageData ); }
			else if( header.bpp == 24 ) {
				glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
				glDrawPixels( header.width, header.height, GL_RGB, GL_UNSIGNED_BYTE, imageData ); } };

		// ...
		void release() { free( imageData); };

		// ...
		char* data() { return imageData; }
		int width() { return header.width; }
		int height() { return header.height; }
		int format() { 
			if( header.bpp == 24 ) { return GL_RGB; }
			return GL_RGBA; }

	private :

		struct ImageHeaderTGA {
			unsigned char idLength;
			unsigned char colorMapType;
			unsigned char imageTypeCode;
			unsigned char colorMapSpec[5];
			unsigned short xOrigin;
			unsigned short yOrigin;	
			unsigned short width;
			unsigned short height;
			unsigned char bpp;
			unsigned char imageDesc; };

		char* imageData;
		ImageHeaderTGA header;
};


#endif // IMAGE_TGA_H


