#define MODE_RGBA

int charWidth = 12;
int charHeight = 18;

int numChars = 16;
int verticalChars = 16;

int height = charHeight * verticalChars;
int width = charWidth * numChars;

#ifdef MODE_RGBA
int size = width * height * 4;
#elif
int size = width * height * 4;
#endif

static unsigned char *image = new unsigned char[size];

unsigned char *getImage(char *name, int &width, int &size) {
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      if (x == y) {
        image[y * width + x] = 0b1111;
        image[y * width + x + 1] = 0b1111;
        image[y * width + x + 2] = 0b1111;
#ifdef MODE_RGBA
        image[y * width + x + 3] = 0b0000;
#endif
      }
    }
  }
  return image;
};