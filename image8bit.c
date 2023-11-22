/// image8bit - A simple image processing module.
///
/// This module is part of a programming project
/// for the course AED, DETI / UA.PT
///
/// You may freely use and modify this code, at your own risk,
/// as long as you give proper credit to the original and subsequent authors.
///
/// João Manuel Rodrigues <jmr@ua.pt>
/// 2013, 2023

// Student authors (fill in below):
// NMec:  Name:
// 
// 
// 
// Date:
//

#include "image8bit.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "instrumentation.h"

// The data structure
//
// An image is stored in a structure containing 3 fields:
// Two integers store the image width and height.
// The other field is a pointer to an array that stores the 8-bit gray
// level of each pixel in the image.  The pixel array is one-dimensional
// and corresponds to a "raster scan" of the image from left to right,
// top to bottom.
// For example, in a 100-pixel wide image (img->width == 100),
//   pixel position (x,y) = (33,0) is stored in img->pixel[33];
//   pixel position (x,y) = (22,1) is stored in img->pixel[122].
// 
// Clients should use images only through variables of type Image,
// which are pointers to the image structure, and should not access the
// structure fields directly.

// Maximum value you can store in a pixel (maximum maxval accepted)
const uint8 PixMax = 255;

// Internal structure for storing 8-bit graymap images
struct image {
  int width;
  int height;
  int maxval;   // maximum gray value (pixels with maxval are pure WHITE)
  uint8* pixel; // pixel data (a raster scan)
};


// This module follows "design-by-contract" principles.
// Read `Design-by-Contract.md` for more details.

/// Error handling functions

// In this module, only functions dealing with memory allocation or file
// (I/O) operations use defensive techniques.
// 
// When one of these functions fails, it signals this by returning an error
// value such as NULL or 0 (see function documentation), and sets an internal
// variable (errCause) to a string indicating the failure cause.
// The errno global variable thoroughly used in the standard library is
// carefully preserved and propagated, and clients can use it together with
// the ImageErrMsg() function to produce informative error messages.
// The use of the GNU standard library error() function is recommended for
// this purpose.
//
// Additional information:  man 3 errno;  man 3 error;

// Variable to preserve errno temporarily
static int errsave = 0;

// Error cause
static char* errCause;

/// Error cause.
/// After some other module function fails (and returns an error code),
/// calling this function retrieves an appropriate message describing the
/// failure cause.  This may be used together with global variable errno
/// to produce informative error messages (using error(), for instance).
///
/// After a successful operation, the result is not garanteed (it might be
/// the previous error cause).  It is not meant to be used in that situation!
char* ImageErrMsg() { ///
  return errCause;
}


// Defensive programming aids
//
// Proper defensive programming in C, which lacks an exception mechanism,
// generally leads to possibly long chains of function calls, error checking,
// cleanup code, and return statements:
//   if ( funA(x) == errorA ) { return errorX; }
//   if ( funB(x) == errorB ) { cleanupForA(); return errorY; }
//   if ( funC(x) == errorC ) { cleanupForB(); cleanupForA(); return errorZ; }
//
// Understanding such chains is difficult, and writing them is boring, messy
// and error-prone.  Programmers tend to overlook the intricate details,
// and end up producing unsafe and sometimes incorrect programs.
//
// In this module, we try to deal with these chains using a somewhat
// unorthodox technique.  It resorts to a very simple internal function
// (check) that is used to wrap the function calls and error tests, and chain
// them into a long Boolean expression that reflects the success of the entire
// operation:
//   success = 
//   check( funA(x) != error , "MsgFailA" ) &&
//   check( funB(x) != error , "MsgFailB" ) &&
//   check( funC(x) != error , "MsgFailC" ) ;
//   if (!success) {
//     conditionalCleanupCode();
//   }
//   return success;
// 
// When a function fails, the chain is interrupted, thanks to the
// short-circuit && operator, and execution jumps to the cleanup code.
// Meanwhile, check() set errCause to an appropriate message.
// 
// This technique has some legibility issues and is not always applicable,
// but it is quite concise, and concentrates cleanup code in a single place.
// 
// See example utilization in ImageLoad and ImageSave.
//
// (You are not required to use this in your code!)


// Check a condition and set errCause to failmsg in case of failure.
// This may be used to chain a sequence of operations and verify its success.
// Propagates the condition.
// Preserves global errno!
static int check(int condition, const char* failmsg) {
  errCause = (char*)(condition ? "" : failmsg);
  return condition;
}


/// Init Image library.  (Call once!)
/// Currently, simply calibrate instrumentation and set names of counters.
void ImageInit(void) { ///
  InstrCalibrate();
  InstrName[0] = "pixmem";  // InstrCount[0] will count pixel array acesses
  // Name other counters here...
  
}

// Macros to simplify accessing instrumentation counters:
#define PIXMEM InstrCount[0]
// Add more macros here...

// TIP: Search for PIXMEM or InstrCount to see where it is incremented!


/// Image management functions

/// Create a new black image.
///   width, height : the dimensions of the new image.
///   maxval: the maximum gray level (corresponding to white).
/// Requires: width and height must be non-negative, maxval > 0.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageCreate(int width, int height, uint8 maxval) { ///
  assert (width >= 0);
  assert (height >= 0);
  assert (0 < maxval && maxval <= PixMax);
  // Insert your code here!
  
  Image newImg = (Image)malloc(sizeof(struct image));

  if (newImg == NULL)
  {
    errCause = "Falha na alocação de memória para a estrutura da imagem"; // Em caso de falha na alocação de memória define a mensagem de erro
    return NULL;
  }
  
  newImg->height = height; // Define a altura da imagem
  newImg->width = width; // Define a largura da imagem
  newImg->maxval = maxval; // Define o valor máximo de cinza
  newImg->pixel = (uint8*)malloc(width * height * sizeof(uint8));

  if (newImg->pixel == NULL)
  {
    errCause = "Falha na alocação de memória para os pixeis"; // Define a mensagem de erro
    free(newImg); // Liberta a memória alocada para a estrutura da imagem
    return NULL;
  }

// Inicializa todos os pixels da imagem com o valor mínimo de intensidade (0) como padrão
  for (int i = 0; i < width * height; i++) {
        newImg->pixel[i] = 0;
  }
  return newImg;
}



/// Destroy the image pointed to by (*imgp).
///   imgp : address of an Image variable.
/// If (*imgp)==NULL, no operation is performed.
/// Ensures: (*imgp)==NULL.
/// Should never fail, and should preserve global errno/errCause.


void ImageDestroy(Image* imgp) {
    assert(imgp != NULL);

    // Insert your code here!

    // Liberta a lista de pixels
    free((*imgp)->pixel);
    (*imgp)->pixel = NULL;

    // Liberta a estrutura da imagem
    free(*imgp);
    *imgp = NULL;
}


/// PGM file operations

// See also:
// PGM format specification: http://netpbm.sourceforge.net/doc/pgm.html

// Match and skip 0 or more comment lines in file f.
// Comments start with a # and continue until the end-of-line, inclusive.
// Returns the number of comments skipped.
static int skipComments(FILE* f) {
  char c;
  int i = 0;
  while (fscanf(f, "#%*[^\n]%c", &c) == 1 && c == '\n') {
    i++;
  }
  return i;
}


/// Load a raw PGM file.
/// Only 8 bit PGM files are accepted.
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.


Image ImageLoad(const char* filename) { ///
  int w, h;
  int maxval;
  char c;
  FILE* f = NULL;
  Image img = NULL;

  int success = 
  check( (f = fopen(filename, "rb")) != NULL, "Open failed" ) &&
  // Parse PGM header
  check( fscanf(f, "P%c ", &c) == 1 && c == '5' , "Invalid file format" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d ", &w) == 1 && w >= 0 , "Invalid width" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d ", &h) == 1 && h >= 0 , "Invalid height" ) &&
  skipComments(f) >= 0 &&
  check( fscanf(f, "%d", &maxval) == 1 && 0 < maxval && maxval <= (int)PixMax , "Invalid maxval" ) &&
  check( fscanf(f, "%c", &c) == 1 && isspace(c) , "Whitespace expected" ) &&
  // Allocate image
  (img = ImageCreate(w, h, (uint8)maxval)) != NULL &&
  // Read pixels
  check( fread(img->pixel, sizeof(uint8), w*h, f) == w*h , "Reading pixels" );
  PIXMEM += (unsigned long)(w*h);  // count pixel memory accesses

  // Cleanup
  if (!success) {
    errsave = errno;
    ImageDestroy(&img);
    errno = errsave;
  }
  if (f != NULL) fclose(f);
  return img;
}

/// Save image to PGM file.
/// On success, returns nonzero.
/// On failure, returns 0, errno/errCause are set appropriately, and
/// a partial and invalid file may be left in the system.
int ImageSave(Image img, const char* filename) { ///
  assert (img != NULL);
  int w = img->width;
  int h = img->height;
  uint8 maxval = img->maxval;
  FILE* f = NULL;

  int success =
  check( (f = fopen(filename, "wb")) != NULL, "Open failed" ) &&
  check( fprintf(f, "P5\n%d %d\n%u\n", w, h, maxval) > 0, "Writing header failed" ) &&
  check( fwrite(img->pixel, sizeof(uint8), w*h, f) == w*h, "Writing pixels failed" ); 
  PIXMEM += (unsigned long)(w*h);  // count pixel memory accesses

  // Cleanup
  if (f != NULL) fclose(f);
  return success;
}


/// Information queries

/// These functions do not modify the image and never fail.

/// Get image width
int ImageWidth(Image img) { ///
  assert (img != NULL);
  return img->width;
}

/// Get image height
int ImageHeight(Image img) { ///
  assert (img != NULL);
  return img->height;
}

/// Get image maximum gray level
int ImageMaxval(Image img) { ///
  assert (img != NULL);
  return img->maxval;
}

/// Pixel stats
/// Find the minimum and maximum gray levels in image.
/// On return,
/// *min is set to the minimum gray level in the image,
/// *max is set to the maximum.
void ImageStats(Image img, uint8* min, uint8* max) { ///
  assert (img != NULL);
  assert (min != NULL); // Verificar se *min é diferente de NULL
  assert (max != NULL); // Verificar se *max é diferente de NULL 
// Insert your code here!

  *min = *max = ImageGetPixel(img, 0, 0); // Inicializa *min e *max com o valor do primeiro pixel da imagem
  int height = img->height; // Obtém a altura da imagem
  int width = img->width; // Obtém a largura da imagem

  // Percorrer todos os pixels da imagem
  for (int i = 0;i < height; i++) {
    for (int j = 0; j < width; j++) {
      uint8 pixel = ImageGetPixel(img, j, i); // Obtém o valor do pixel
      if (pixel < *min) {
        *min = pixel; // Caso seja menor que *min este passa a ter o valor do pixel
      }
      if (pixel > *max) {
        *max = pixel; // Caso seja maior que *max este passa a ter o valor do pixel
      }  
    }
  }
}

/// Check if pixel position (x,y) is inside img.
int ImageValidPos(Image img, int x, int y) { ///
  assert (img != NULL);
  return (0 <= x && x < img->width) && (0 <= y && y < img->height);
}

/// Check if rectangular area (x,y,w,h) is completely inside img.
int ImageValidRect(Image img, int x, int y, int w, int h) { ///
  assert (img != NULL);
  // Insert your code here!
  return ImageValidPos(img, x, y) && (0 <= w && w < img->width) && (0 <= h && h < img->height); // Verifica se a posição (x,y) é válida e verifica se a largura e altura são maiores que 0 e menores que a largura e altura da imagem.
}
/// Pixel get & set operations

/// These are the primitive operations to access and modify a single pixel
/// in the image.
/// These are very simple, but fundamental operations, which may be used to 
/// implement more complex operations.

// Transform (x, y) coords into linear pixel index.
// This internal function is used in ImageGetPixel / ImageSetPixel. 
// The returned index must satisfy (0 <= index < img->width*img->height)
static inline int G(Image img, int x, int y) {
  int index = y * img->width + x;
  // Insert your code here!
  //printf("\nwidth: %d, height: %d, x: %d, y: %d, index: %d", img->width,img->height,x,y,index);
  assert (0 <= index && index < img->width*img->height);
  return index;
}

/// Get the pixel (level) at position (x,y).
uint8 ImageGetPixel(Image img, int x, int y) { ///
  assert (img != NULL);
  assert (ImageValidPos(img, x, y));
  PIXMEM += 1;  // count one pixel access (read)
  return img->pixel[G(img, x, y)];
}
/// Set the pixel at position (x,y) to new level.
void ImageSetPixel(Image img, int x, int y, uint8 level) { ///
  assert (img != NULL);
  assert (ImageValidPos(img, x, y));
  PIXMEM += 1;  // count one pixel access (store)
  img->pixel[G(img, x, y)] = level;
} 

/// Pixel transformations

/// These functions modify the pixel levels in an image, but do not change
/// pixel positions or image geometry in any way.
/// All of these functions modify the image in-place: no allocation involved.
/// They never fail.


/// Transform image to negative image.
/// This transforms dark pixels to light pixels and vice-versa,
/// resulting in a "photographic negative" effect.

// transformar imagens na sua versão negativa. Inverte os niveis de pixel. pixeis escuros -> pixeis claros
void ImageNegative(Image img) { ///
  // verificar se a imagem não é nula para poder prosseguir
  assert (img != NULL); 

  // obter info da imagem para eventual uso
  int width = img->width;
  int height = img->height;
  int maxval = img->maxval;

  // iterar sobre cada pixel da imagem, para poder aceder a cada pixel individualmente
  for(int i = 0; i < height; i++){
    for(int j = 0; j < width; j++){
      // obter o pixel atual para posteriormente ser alterada a sua cor
      uint8 actual_pixel = ImageGetPixel(img, j, i);
      // definir o valor do pixel, calculando o valor do pixel maxval
      ImageSetPixel(img, j, i, (uint8)(maxval - actual_pixel)); 
    }
  }
}

/// Apply threshold to image.
/// Transform all pixels with level<thr to black (0) and
/// all pixels with level>=thr to white (maxval).
void ImageThreshold(Image img, uint8 thr) { ///
  assert (img != NULL);
  // Insert your code here!
  // obter info da imagem para eventual uso
  int height = img->height;
  int width = img->width;
  int maxval = img->maxval;

  // iterar sobre cada pixel da imagem, para poder aceder a cada pixel individualmente
  for (int i = 0; i < height; i++){
    for (int j = 0; j < width; j++){
      // obter o pixel atual nas coordenadas i,j
      uint8 actual_pixel = ImageGetPixel(img,j,i);
      // se o pixel actual for menor que thr, o pixel é definido para preto
      if (actual_pixel < thr){
        ImageSetPixel(img,j,i,(uint8)(0));
      
      } else{
        ImageSetPixel(img,j,i,(uint8)(maxval)); // caso contrário é definido para o nivel maxval
      }
    }
  }

}

/// Brighten image by a factor.
/// Multiply each pixel level by a factor, but saturate at maxval.
/// This will brighten the image if factor>1.0 and
/// darken the image if factor<1.0.
void ImageBrighten(Image img, double factor) { 
  assert (img != NULL);
  // ? assert (factor >= 0.0);
  // Insert your code here!

  int height = img->height;
  int width = img->width;
  int maxval = img->maxval;

  for (int i = 0; i < height; i++){
    for (int j = 0; j < width; j++){
      // obter o pixel atual nas coordenadas i,j
      uint8 actual_pixel = ImageGetPixel(img,j,i);
      double brighten = actual_pixel * factor;
      // arredondamento (correção erro "byte 90, linha 4")
      double decimal = brighten - (int)brighten;
      if (decimal >= 0.5) {
          brighten = (int)brighten + 1;
      } else {
          brighten = (int)brighten;
      }

      if(brighten > maxval){
        brighten = maxval; 
      }
      ImageSetPixel(img,j,i,(uint8)brighten);
    }
  }
} 


/// Geometric transformations

/// These functions apply geometric transformations to an image,
/// returning a new image as a result.
/// 
/// Success and failure are treated as in ImageCreate:
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.

// Implementation hint: 
// Call ImageCreate whenever you need a new image!

/// Rotate an image.
/// Returns a rotated version of the image.
/// The rotation is 90 degrees anti-clockwise.
/// Ensures: The original img is not modified.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageRotate(Image img) { ///
  assert (img != NULL);
  // Insert your code here!

  // Obter a largura e altura da imagem para eventual uso
  int width = img->width;
  int height = img->height;

  Image rotImg = ImageCreate(height, width, img->maxval);    // Criação nova imagem chamada rotImg

  // Percorrer todos os pixeis da imagem
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      uint8 pixel = ImageGetPixel(img, j, i); // Obter o valor do pixel
      ImageSetPixel(rotImg, i, width - j - 1, pixel); // Define o valor do pixel na nova imagem
    }
  }
  return rotImg;  // Retornar a imagem rodada em 90 graus anti-horáriox
}

/// Mirror an image = flip left-right.
/// Returns a mirrored version of the image.
/// Ensures: The original img is not modified.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageMirror(Image img) { ///
  assert (img != NULL);
  // Insert your code here!

  // Obter a largura e altura da imagem para eventual uso
  int width = img->width;
  int height = img->height;

  Image mirrorImg = ImageCreate(height, width, img->maxval); // Criação nova imagem chamada mirrorImg

  // Percorrer todos os pixeis da imagem
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      uint8 pixel = ImageGetPixel(img, j, i); // Obter o valor do pixel
      ImageSetPixel(mirrorImg, width-j-1, i, pixel); // Define o valor do pixel imagem espelhada
    }
  }
  return mirrorImg; // Retornar a imagem espelhada
} 

/// Crop a rectangular subimage from img.
/// The rectangle is specified by the top left corner coords (x, y) and
/// width w and height h.
/// Requires:
///   The rectangle must be inside the original image.
/// Ensures:
///   The original img is not modified.
///   The returned image has width w and height h.
/// 
/// On success, a new image is returned.
/// (The caller is responsible for destroying the returned image!)
/// On failure, returns NULL and errno/errCause are set accordingly.
Image ImageCrop(Image img, int x, int y, int w, int h) { ///
  assert (img != NULL);
  assert (ImageValidRect(img, x, y, w, h)); // retangulo deve estar dentro da imagem original 
  // Insert your code here!
  int maxval = img->maxval;

  Image cropImg = ImageCreate(h, w, maxval);  // Criar nova imagem chamada cropImg

  if(cropImg == NULL){
    errCause = "Erro na criação da imagem!";
    return NULL;
  }

  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      uint8 pixel = ImageGetPixel(img, x + j, y + i);   // Obter o pixel da imagem cuja posição é (x + j, y + i)
      ImageSetPixel(cropImg, j, i, pixel);      // Definir o pixel da imagem cortada na posição (j, i)
      }
    }
  return cropImg;           // Retorna a imagem cortada
}

/// Operations on two images

/// Paste an image into a larger image.
/// Paste img2 into position (x, y) of img1.
/// This modifies img1 in-place: no allocation involved.
/// Requires: img2 must fit inside img1 at position (x, y).
void ImagePaste(Image img1, int x, int y, Image img2) { ///
  assert(img1 != NULL);
  assert(img2 != NULL);
  // Obter a largura e altura da imagem2 para eventual uso
  int img2_width = ImageWidth(img2);
  int img2_height = ImageHeight(img2);
  assert(ImageValidRect(img1, x, y, img2_width, img2_height)); // Verifica se a imagem2 que vai ser colada cabe dentro da imagem1

  // Percorrer todos os pixeis da imagem2
  for (int i = 0; i < img2_height; i++) {
    for (int j = 0; j < img2_width; j++) {
      uint8 pixel = ImageGetPixel(img2, j, i);  // Obter o valor do pixel da posição (j, i) da img2
      ImageSetPixel(img1, x + j, y + i, pixel); // Colar o pixel na posição (x+j, y+i) em img1
    }
  }
}

/// Blend an image into a larger image.
/// Blend img2 into position (x, y) of img1.
/// This modifies img1 in-place: no allocation involved.
/// Requires: img2 must fit inside img1 at position (x, y).
/// alpha usually is in [0.0, 1.0], but values outside that interval
/// may provide interesting effects. Over/underflows should saturate.
void ImageBlend(Image img1, int x, int y, Image img2, double alpha) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidRect(img1, x, y, img2->width, img2->height));
  // Insert your code here!

  // Percorrer os pixeis da imagem2
  for (int j=0; j < img2->height; j++) {
    for (int i=0; i < img2->width; i++) {
      uint8 pixel1 = ImageGetPixel(img1, x + j, y + i); // Obter o valor do pixel da posição (x+j, y+i) da img1
      uint8 pixel2 = ImageGetPixel(img2, j, i); // Obter o valor do pixel da posição (j,i) da img2

      // Obter o valor do pixel resultante da mistura 
      uint8 pixel3 = (uint8)((1-alpha) * pixel1 + alpha * pixel2);

      // Colar o pixel na posição (x+j, y+i) em img1
      ImageSetPixel(img1, x + j, y + i, pixel3);
    }
  }
}

/// Compare an image to a subimage of a larger image.
/// Returns 1 (true) if img2 matches subimage of img1 at pos (x, y).
/// Returns 0, otherwise.
int ImageMatchSubImage(Image img1, int x, int y, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  assert (ImageValidPos(img1, x, y));
  // Insert your code here!

  // Percorrer os pixeis da imagem2
  for (int j=0; j < img2->height; j++) {
    for (int i=0; i < img2->width; i++) {
      uint8 pixel1 = ImageGetPixel(img1, x + j, y + i); // Obter o valor do pixel da posição (x+j, y+i) da img1
      uint8 pixel2 = ImageGetPixel(img2, j, i); // Obter o valor do pixel da posição (j,i) da img

      // Caso os pixeis sejam diferentes retorna 0, implicando que a img2 correspondente a uma subimagem da img1 
      if (pixel1 != pixel2) {
        return 0;
      }
    }
  }
  return 1; // Caso nem todos os pixeis forem correspondentes então retorna 1
}

/// Locate a subimage inside another image.
/// Searches for img2 inside img1.
/// If a match is found, returns 1 and matching position is set in vars (*px, *py).
/// If no match is found, returns 0 and (*px, *py) are left untouched.
int ImageLocateSubImage(Image img1, int* px, int* py, Image img2) { ///
  assert (img1 != NULL);
  assert (img2 != NULL);
  // Insert your code here!

  // Obter valores das img1 e img2 para serem usados posteriormente
  int height = img1->height;
  int width = img1->width;
  int height2 = img2->height;
  int width2 = img2->width;

  int height3 = height - height2;
  int width3 = width - width2;

  // Percorrer todos os pixeis da img1
  for (int j = 0; j < height3; j++) {
    for (int i = 0;i < width3; i++) {
      // Verifica se a img2 corresponde a uma subimagem da img1
      if (ImageMatchSubImage(img1, i, j, img2)) {
        *px = i; // Define o valor de *px
        *py = j; // Define o valor de *py
        return 1; // Retorna 1 caso seja localizada uma subimagem
      }
    }
  }
  return 0; // Retorna 0 caso seja falso
}

/// Filtering
/// Blur an image by a applying a (2dx+1)x(2dy+1) mean filter.
/// Each pixel is substituted by the mean of the pixels in the rectangle
/// [x-dx, x+dx]x[y-dy, y+dy].
/// The image is changed in-place.
void ImageBlur(Image img, int dx, int dy) { ///
  // Insert your code here!
  assert(img!=NULL);


  int height = img->height;
  int width = img->width;
  int maxval = img->maxval;

  Image blurImg = ImageCreate(width, height, maxval); // Criar imagem para ser desfocada 

  // Percorrer todos os pixeis dessa imagem
  for (int j=0; j < height; j++) {
    for (int i=0; i < width; i++) { 
      int count = 0;                        // Inicialização de uma variável de contagem de pixeis
      int soma = 0;                         // Inicialização de variavel da soma de pixeis 

      for (int x = -dx; x<=dx; x++) {                       // Percorrer pixeis entre as posições (x-dx, x+dx)
        for (int y = -dy; y<dy; y++) {                      // Percorrer pixeis entre as posições (y-dy, y+dy)
          if (ImageValidPos(img, i+y, j+x)){                // Verifica se o posição está dentro da imagem
            soma += ImageGetPixel(img, i+y, j+x);           // Somar o valor dos pixeis válidos
            count++;                                        // Incrementa o contador
          }     
        }
      }
      uint8 pixel = (uint8)(soma / count);    // Calcula a média dos valores dos pixeis
      ImageSetPixel(blurImg, i, j, pixel);      // Define o valor na imagem desfocada

    
  // Percorre os valores da imagem desfocada de modo a substituí-los na imagem original
  for (int y = 0; y < img->height; y++) {
    for (int x = 0; x < img->width; x++) {
        uint8 blurredPixel = ImageGetPixel(blurImg, x, y);
        ImageSetPixel(img, x, y, blurredPixel);  // Substitui os pixels pelos valores filtrados
        }
    }
    }
  }
}

