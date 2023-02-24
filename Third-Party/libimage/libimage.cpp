#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <thread>
#include <Windows.h>
#include "jpeglib.h"
#include "png.h"
#include "libimage.h"
#include "thorvg.h"
#include "zlib.h"
using namespace std;
#define BitmapWidth(cx, bpp)  (((cx * bpp + 31) & ~31) >> 3)
#define JPGHeader "\xff\xd8\xff"
const int c_JPEGInputBufferSize = 4096;
using namespace LIB_IMAGE;

const char * ERROR_ICON = "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d\x49\x48\x44\x52"
"\x00\x00\x02\x00\x00\x00\x02\x00\x08\x03\x00\x00\x00\xc3\xa6\x24"
"\xc8\x00\x00\x01\xdd\x50\x4c\x54\x45\x00\x00\x00\xc6\x42\x39\xc7"
"\x48\x3d\xc8\x46\x3e\xc8\x47\x3e\xc8\x48\x3e\xc8\x48\x3e\xc8\x49"
"\x3f\xc7\x47\x3e\xc8\x43\x37\xc7\x47\x3d\xc8\x49\x3e\xc8\x49\x3e"
"\xc6\x48\x3d\xc9\x46\x3e\xc8\x48\x3e\xc7\x48\x3f\xc7\x48\x40\xcc"
"\x33\x33\xc9\x49\x3f\xc8\x4a\x3f\xc6\x55\x39\xc8\x48\x3d\xc8\x48"
"\x3f\xc8\x48\x3f\xc6\x4a\x3e\xc9\x46\x3e\xc7\x49\x3e\xc8\x49\x3e"
"\xc7\x46\x3e\xc7\x48\x3f\xc8\x4a\x3f\xc8\x49\x3f\xc6\x48\x3d\xc8"
"\x47\x3b\xc7\x47\x3e\xbf\x40\x40\xc8\x49\x3f\xdb\x49\x49\xc8\x48"
"\x3e\xc9\x48\x3e\xc5\x4a\x42\xc8\x49\x3f\xc8\x49\x3e\xc9\x49\x3e"
"\xc8\x49\x40\xc4\x45\x3b\xc8\x47\x3c\xc9\x47\x3e\xc9\x47\x3f\xc8"
"\x48\x3f\xc7\x48\x3e\xc8\x49\x3f\xc8\x49\x3e\xc8\x49\x3e\xc7\x48"
"\x3e\xc1\x35\x2a\xc1\x35\x2a\xbe\x34\x29\xbf\x34\x29\xbb\x34\x28"
"\xbc\x34\x29\xb3\x31\x26\xb2\x31\x26\xa7\x2d\x24\x88\x27\x1e\x85"
"\x25\x1c\x3e\x0f\x0c\xc2\x35\x2a\x37\x10\x0c\x00\x00\x00\xb8\x32"
"\x29\xb7\x32\x28\x95\x28\x20\x95\x27\x21\x34\x0c\x08\xc1\x35\x2b"
"\xc1\x35\x2a\x31\x0c\x08\xa7\x2e\x24\xa7\x2d\x25\x43\x12\x0f\xc1"
"\x35\x2a\x40\x13\x0b\x9c\x2b\x22\x9a\x2a\x22\x12\x05\x05\xba\x32"
"\x29\xb9\x32\x28\x13\x05\x05\x52\x18\x11\xc0\x34\x2a\x56\x17\x14"
"\x83\x25\x1c\xc2\x35\x2a\x83\x24\x1d\x00\x00\x00\x93\x27\x1f\x00"
"\x00\x00\x92\x27\x21\x97\x29\x21\x00\x00\x00\xc1\x36\x2a\x00\x00"
"\x00\xb9\x33\x29\x54\x17\x14\x00\x00\x00\x0e\x05\x05\x98\x2b\x20"
"\x9d\x2b\x22\x00\x00\x00\x40\x13\x0f\x00\x00\x00\x00\x00\x00\x96"
"\x28\x20\xb8\x32\x28\x00\x00\x00\x00\x00\x00\x86\x25\x1c\xb3\x31"
"\x26\xbb\x33\x29\xc0\x33\x2a\xbb\x34\x29\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\xc8\x49\x3f\xc8\x47\x3d\xc6\x42\x37\xc5\x3e\x33"
"\xc4\x3b\x30\xc3\x38\x2d\xc2\x37\x2c\xc2\x35\x2a\xc5\x40\x36\xc3"
"\x39\x2e\xc7\x46\x3c\xc2\x36\x2b\xc8\x48\x3e\xc5\x3f\x35\xc6\x42"
"\x38\xc6\x41\x37\xc4\x3d\x32\xc3\x37\x2d\xff\xff\xff\x9b\x2a\x22"
"\xff\xfe\xfe\x9c\x2c\x24\xee\x8a\xf3\xa1\x00\x00\x00\x89\x74\x52"
"\x4e\x53\x00\x1b\x60\x98\xbe\xde\xf0\xfa\x97\x17\x76\xd1\xcf\x75"
"\x21\x99\xf7\x20\x05\xf4\x86\x09\x2e\xd8\xd7\x2d\x5f\xf6\xf5\x5b"
"\x72\xfd\xfe\x79\x70\x77\x08\xd6\x07\x83\x7f\x1f\x9a\x73\xce\x1c"
"\x1a\x61\x5e\x96\xbf\xbc\xe0\xef\xee\xfa\xf9\xf8\xed\xeb\xde\xdc"
"\xbd\xba\x9a\x6f\x6d\x42\xfe\x41\x33\xce\xcb\x7f\x7d\x40\xf6\xf5"
"\x3f\x9c\x99\x45\xf4\x44\x89\x86\x38\xd5\xd3\x37\x4b\xf2\x4d\x6d"
"\xfd\x6b\x32\x7b\x2f\x7c\x82\x2c\xf3\x25\xd4\x4c\x1d\x36\x7e\x8d"
"\x12\x44\x11\x05\x7e\xcc\x04\x29\x6e\xbb\xda\xee\xd9\x28\x16\x31"
"\x06\x19\x18\x01\x2a\x08\x30\x15\x17\x1a\x2b\xb8\x67\x9c\x54\x00"
"\x00\x08\x41\x49\x44\x41\x54\x78\xda\xed\xdd\x07\x73\x15\x65\x14"
"\x80\xe1\xab\x14\xc1\x82\xd8\xb0\xa1\xa0\xd8\x7b\x17\x7b\x44\x58"
"\xb1\x80\x88\x8a\x58\x40\x44\x01\x8d\x0d\x14\x45\x25\x41\x29\xb6"
"\x10\x82\x81\x80\x0d\xfd\xad\x0a\xc1\x90\x30\x4e\xcc\xbd\xdf\x97"
"\xf9\xb2\x7b\x9e\xf7\x07\xec\xec\x99\xf3\xdc\xdd\xbb\x7b\x33\x93"
"\x56\x4b\x92\x24\x49\x92\x24\x49\x92\x24\x49\x92\x24\x49\x92\x24"
"\x49\x92\x24\x49\x92\x24\x49\x92\x24\x35\xaf\x33\xce\x9c\x36\x7d"
"\xc6\xcc\xb3\x7e\xd2\x94\xea\xac\x99\x33\xa6\xcf\x3a\xf3\x8c\xc9"
"\xde\xfe\xec\xb3\xcf\x29\x3d\xa9\xc6\xeb\xdc\xf3\x66\x4f\xe2\xfa"
"\xe7\x9c\x3f\xb7\xf4\x80\xfa\xbf\xe6\x4e\xbb\x60\x92\xd6\x7f\xe1"
"\x79\x17\x95\x1e\x4e\x13\x69\xee\xc5\x97\x4c\xc6\xfe\xe7\x5d\x5a"
"\x7a\x30\x4d\xb4\xcb\x2e\xcf\xbf\xff\x2b\xae\x2c\x3d\x95\x26\xde"
"\xfc\xab\x72\xef\xff\xea\x05\xa5\x67\x52\x3b\x2d\xbc\x26\xef\xfe"
"\xaf\x2d\x3d\x90\xda\x6d\x51\xd6\xeb\xbf\xcf\x7f\xed\x5a\x90\xf1"
"\x2e\x30\xcf\xfd\xbf\x86\xcd\xcf\xf6\x4d\xf0\x3a\xdf\xff\x6b\xd9"
"\xf5\x37\x64\x02\x70\x63\xe9\x49\xd4\x59\x37\xe5\xd9\xff\x1c\xaf"
"\xff\x6a\xda\x95\x37\x67\x01\x70\x4b\xe9\x39\xd4\x69\xb3\x72\xec"
"\x7f\xb6\x0b\x40\x6d\x9b\x9b\xe3\x97\xa1\x45\xa5\xa7\x50\xe7\xdd"
"\x9a\x01\xc0\xe9\xbf\xff\xf6\xed\xef\x3f\x30\x70\xf0\x67\x4d\xb1"
"\x0e\x0e\x1c\xe8\xdf\xdf\x77\xda\xb2\x6e\x4b\xdf\xff\xed\x63\xb7"
"\x3f\x78\xa8\xf4\xa0\x1a\xaf\x43\x83\x63\x0c\x2c\xbc\x23\x19\xc0"
"\x9d\xa3\x0e\x77\xb8\x7f\xa8\xf4\x80\xfa\xbf\x86\xfa\x0f\x8f\x5a"
"\xd9\x5d\xc9\x00\xa6\x8d\x1c\xeb\xc8\x51\xeb\xaf\x45\x43\x47\x8f"
"\x8c\x2c\xed\xee\x64\x00\xf7\x8c\x7c\xfc\x07\x4a\x0f\xa6\x89\x36"
"\x30\x72\x11\xb8\x37\x19\xc0\x7d\x27\x8f\xb4\xdf\xc7\xbf\x46\x0d"
"\xfd\x72\x72\x6d\x33\x92\x01\xdc\x3f\x7c\xa0\xc1\xd2\x23\xa9\xbd"
"\x06\x87\xf7\xf6\x40\x32\x80\x07\x4f\x1c\xe7\xd7\xd2\xf3\xa8\xdd"
"\x86\x05\x3c\x98\x0c\x60\xf8\xfa\x5f\x7a\x1a\xb5\xdf\xf0\x5d\x20"
"\x0b\x80\xc3\xee\xff\x35\x6c\xe8\x70\x2e\x00\x47\x7c\xff\xaf\x65"
"\x03\x47\x32\x01\x38\x5a\x7a\x12\x75\xd6\xd1\x3c\x00\xdc\x00\xea"
"\xda\xf1\x9b\x40\x06\x00\xbf\x95\x9e\x43\x9d\xd6\x9f\x03\x40\x9f"
"\x0b\x40\x6d\x1b\xea\xcb\x00\xc0\x2b\xa0\x1a\x37\x98\x01\x80\xdf"
"\x7f\x6b\xdc\xa1\x74\x00\x7d\xa5\x67\x50\x4a\x7d\xc9\x00\xbc\x04"
"\xac\x75\xfb\x93\x01\xf4\x97\x1e\x41\x29\xf5\x27\x03\x38\x50\x7a"
"\x04\xa5\x74\x20\x19\xc0\xef\xa5\x47\x50\x4a\x03\xc9\x00\xfc\xfd"
"\x6f\xad\x3b\x98\x0c\xc0\x6b\xa0\x5a\x37\x94\x0c\xa0\xf4\x04\x4a"
"\x0b\x80\xe0\x01\x10\x3c\x00\x82\x07\x40\xf0\x00\x08\x1e\x00\xc1"
"\x03\x20\x78\x00\x04\x0f\x80\xe0\x01\x10\x3c\x00\x82\x07\x40\xf0"
"\x00\x08\x1e\x00\xc1\x03\x20\x78\x00\x04\x0f\x80\xe0\x01\x10\x3c"
"\x00\x82\x07\x40\xf0\x00\x08\x1e\x00\xc1\x03\x20\x78\x00\x04\x0f"
"\x80\xe0\x01\x10\x3c\x00\x82\x07\x40\xf0\x00\x08\x1e\x00\xc1\x03"
"\x20\x78\x00\x04\x0f\x80\xe0\x01\x10\x3c\x00\x82\x07\x40\xf0\x00"
"\x08\x1e\x00\xc1\x03\x20\x78\xe1\x01\xfc\xd1\x66\xa5\xcf\x37\x77"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x34\x2c\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x1a\x16\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0d"
"\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x86\x05\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x40\xc3\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xa0"
"\x61\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xd0\xb0\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x68\x58\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x34\x2c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1a\x16\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x0d\x0b\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x80\x86\x05\x00\x00\x00\x00\x00\x00\x00\x51\x01\x1c\x6b\xb3\xd2"
"\xe7\x9b\x3b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x68\x22\x80\xe8\x01\x10\x3c\x00"
"\x82\x07\x40\xf0\x00\x08\x1e\x00\xc1\x03\x20\x78\x00\x04\x0f\x80"
"\xe0\x01\x10\x3c\x00\x82\x07\x40\xf0\x00\x08\x1e\x00\xc1\x03\x20"
"\x78\x00\x04\x0f\x80\xe0\x01\x10\x3c\x00\x82\x07\x40\xf0\x00\x08"
"\x1e\x00\xc1\x03\x20\x78\x00\x04\x0f\x80\xe0\x01\x10\x3c\x00\x82"
"\x07\x40\xf0\x00\x08\x1e\x00\xc1\x03\x20\x78\x00\x04\x2f\x3c\x80"
"\x3f\xdb\xac\xf4\xf9\xe6\x2e\x3c\x00\xff\x31\x04\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x28\x7d\xbe\xb9\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xa0"
"\x61\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x40\x73\x01\xfc\xd5\x66\xa5\xcf\x37\x77\xe1\x01\x1c\x6b\xb3"
"\xd2\xe7\x9b\x3b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
"\x00\x00\x00\x00\x00\x00\x80\x86\x02\x88\x1e\x00\xc1\x03\x20\x78"
"\x00\x04\x0f\x80\xe0\x01\x10\x3c\x00\x82\x07\x40\xf0\x00\x08\x1e"
"\x00\xc1\x03\x20\x78\x00\x04\x0f\x80\xe0\x01\x10\x3c\x00\x82\x07"
"\x40\xf0\x00\x08\x1e\x00\xc1\x03\x20\x78\x00\x04\x0f\x80\xe0\x01"
"\x10\x3c\x00\x82\x07\x40\xf0\x92\x01\x2c\x2e\x3d\x81\x52\x7a\x28"
"\x19\xc0\xc3\xa5\x47\x50\x4a\x8f\x24\x03\x78\xb4\xf4\x08\x4a\xe9"
"\xb1\x64\x00\x8f\x97\x1e\x41\x29\x3d\x91\x0c\xa0\xab\xf4\x08\x4a"
"\xa9\x2b\x19\xc0\x93\xa5\x47\x50\x4a\x4b\x92\x01\x3c\xb5\xb4\xf4"
"\x0c\xea\xbc\xa5\xcb\x92\x01\x54\x4f\x97\x1e\x42\x9d\xb7\xbc\x4a"
"\x07\xf0\x4c\xe9\x21\xd4\x79\xcf\x66\x00\xf0\xdc\xf3\xa5\xa7\x50"
"\xa7\xad\x58\x99\x01\x40\xf5\x42\xe9\x31\xd4\x69\xab\xaa\x1c\x00"
"\x5e\x5c\x51\x7a\x0e\x75\xd6\xea\x97\xb2\x00\xa8\x5e\x2e\x3d\x88"
"\x3a\xeb\x95\x2a\x0f\x80\x35\xaf\x96\x9e\x44\x9d\xb4\xf6\xb5\x4c"
"\x00\xaa\xd7\xdf\x28\x3d\x8b\xda\x6f\xf5\x9b\x55\x2e\x00\xd5\xba"
"\xf5\xa5\xa7\x51\xbb\xad\x7f\xab\xca\x02\x60\xc3\x89\xc3\xbc\x5d"
"\x7a\x1c\xb5\xdb\x33\x27\x16\xb7\x21\x19\xc0\xc6\x13\xc7\xa9\xde"
"\x71\x0d\xa8\x55\x4b\xdf\x1d\xde\xdb\xc6\x64\x00\x9b\x86\x0f\x54"
"\xad\x5b\x5d\x7a\x26\x4d\xbc\xcd\x6f\x9d\x5c\xdb\xa6\x64\x00\x5b"
"\x4e\x1e\xa9\x7a\xf3\xbd\xd2\x53\x69\xa2\xad\x7d\xff\xdf\xad\x6d"
"\x49\x06\xd0\xfd\xef\xa1\xaa\x0f\x3e\xdc\x5c\x7a\x30\x4d\xa4\x15"
"\x1f\xad\x19\x59\x5a\x77\x32\x80\x8f\xab\x53\x7d\xd2\xe5\x77\x81"
"\x29\xdf\x8a\xae\x97\x46\xad\x6c\x6b\x32\x80\x6d\xd5\xe8\x9e\xfb"
"\xf4\xb3\xd2\x03\x6a\x9c\x96\x2e\xff\x74\xe5\x98\x85\x6d\x4f\x06"
"\xd0\xfa\xbc\x1a\xdb\xb2\x2f\xba\x76\x7c\xf9\x95\xbf\x16\x9f\x62"
"\x2d\x7e\xf8\xeb\x1d\xab\x96\x2c\x3b\x6d\x59\x3b\xd3\xf7\xdf\xea"
"\xa9\x54\xdb\x7a\x32\x00\xd8\xde\x5b\x7a\x0a\x75\x5a\x6f\x86\x3b"
"\xc0\xe8\xe7\x00\xd5\xac\xf4\x67\x80\xe3\xed\x72\x09\xa8\x69\xbd"
"\xdb\xb2\x00\x68\x7d\x53\x7a\x10\x75\xd6\xb7\x79\xf6\xdf\xda\xbd"
"\xa7\xf4\x24\xea\xa4\x3d\xbb\x33\x01\x68\xed\xdd\x57\x7a\x16\xb5"
"\x5f\xef\xde\x5c\xfb\x6f\xb5\xb6\x96\x1e\x46\xed\xb7\x35\xdf\xfe"
"\x5b\xad\xef\x4a\x4f\xa3\x76\xcb\xf1\x0a\x80\x80\xfa\xf6\x7d\xde"
"\xfd\xff\x73\x17\xf0\x30\x58\xa3\x7a\xb3\x5e\xff\x87\xdb\xeb\x59"
"\xa0\x36\xed\xc9\xf8\xfd\xef\x54\xbb\x7b\x5c\x04\x6a\x51\xef\x0f"
"\xd9\x9e\xff\x4e\x6b\x57\x37\x02\x53\xbe\xde\xee\x4c\xef\xff\xfe"
"\xb3\xed\x3d\x3b\x4b\x0f\xa8\xf1\xda\xd9\x93\xe5\xf7\x9f\xf1\xda"
"\xf6\x71\xf7\x96\x1f\xf7\x6d\x28\x3d\xa9\xc6\xb6\x61\xe3\x8f\x5b"
"\xba\xb7\x4e\xfa\xf6\x25\x49\x92\x24\x49\x92\x24\x49\x92\x24\x49"
"\x52\x0d\xfa\x1b\x05\xf0\x3d\xca\xb2\x3b\xb9\xb7\x00\x00\x00\x00"
"\x49\x45\x4e\x44\xae\x42\x60\x82";
int ERROR_ICON_SIZE = 2808;

struct jpeg_membuffer_t
{
	const unsigned char*	amem_buff;
	size_t					pos,
		size;
};

struct cDataManagerJPEGSource
{
	jpeg_source_mgr			pub;
	jpeg_membuffer_t*		read_buffer;
	JOCTET*					buffer;
	bool					sof;
};

struct cDataManagerJPEGDest
{
	jpeg_destination_mgr	mgr;
	unsigned char*			buffer;
};

struct png_membuffer_t
{
	const unsigned char* amem_buff;
	size_t					pos,
		size;
};

void png_read_memory_data_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_membuffer_t* membuff = (png_membuffer_t*)png_get_io_ptr(png_ptr);
	size_t nbytes = membuff->size - membuff->pos;
	nbytes = nbytes > length ? length : nbytes;
	memcpy(data, membuff->amem_buff + membuff->pos, nbytes);
	membuff->pos += nbytes;
}

HBITMAP CreateDIB(HWND hwnd, int aWidth, int aHeight, int aPixelBit)
{
	HBITMAP hBitmap = NULL;

	BITMAPINFO      pBitmapInfo = { 0 };
	
	pBitmapInfo.bmiHeader.biSize = sizeof(pBitmapInfo.bmiHeader);
	pBitmapInfo.bmiHeader.biBitCount = aPixelBit;
	pBitmapInfo.bmiHeader.biCompression = BI_RGB;
	pBitmapInfo.bmiHeader.biWidth = aWidth;
	pBitmapInfo.bmiHeader.biHeight = -aHeight;
	pBitmapInfo.bmiHeader.biPlanes = 1;
	pBitmapInfo.bmiHeader.biSizeImage = BitmapWidth(pBitmapInfo.bmiHeader.biWidth, pBitmapInfo.bmiHeader.biBitCount) * pBitmapInfo.bmiHeader.biHeight;
	void* BBits = NULL;
	hBitmap = CreateDIBSection(GetDC(hwnd), &pBitmapInfo, DIB_RGB_COLORS, &BBits, 0, 0);

	return hBitmap;
}

void JPEGInitSource(j_decompress_ptr cinfo)
{
	cDataManagerJPEGSource* src = (cDataManagerJPEGSource*)cinfo->src;
	src->sof = true;
}

boolean JPEGFillInputBuffer(j_decompress_ptr cinfo)
{
	cDataManagerJPEGSource* src = (cDataManagerJPEGSource*)cinfo->src;

	size_t nbytes = src->read_buffer->size - src->read_buffer->pos;
	nbytes = nbytes > c_JPEGInputBufferSize ? c_JPEGInputBufferSize : nbytes;
	memcpy(src->buffer, src->read_buffer->amem_buff + src->read_buffer->pos, nbytes);
	src->read_buffer->pos += nbytes;

	if (nbytes <= 0) {
		if (src->sof)	return(FALSE);

		src->buffer[0] = (JOCTET)0xFF;
		src->buffer[1] = (JOCTET)JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->sof = false;

	return(TRUE);
}

void JPEGSkipInputData(j_decompress_ptr cinfo, long num_bytes)
{
	cDataManagerJPEGSource* src = (cDataManagerJPEGSource*)cinfo->src;
	if (num_bytes <= 0) return;

	while (num_bytes > (long)src->pub.bytes_in_buffer) {
		num_bytes -= (long)src->pub.bytes_in_buffer;
		JPEGFillInputBuffer(cinfo);
	}

	src->pub.next_input_byte += (size_t)num_bytes;
	src->pub.bytes_in_buffer -= (size_t)num_bytes;
}

void JPEGTermSource(j_decompress_ptr cinfo)
{

}

void JPEGStdioSrc(j_decompress_ptr cinfo, jpeg_membuffer_t* file)
{
	cDataManagerJPEGSource* src = 0;


	if (cinfo->src == 0) {

		cinfo->src = (struct jpeg_source_mgr *) (*cinfo->mem->alloc_small)
			((j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(cDataManagerJPEGSource));
		src = (cDataManagerJPEGSource*)cinfo->src;

		src->buffer = (JOCTET*)(*cinfo->mem->alloc_small)
			((j_common_ptr)cinfo, JPOOL_PERMANENT, c_JPEGInputBufferSize * sizeof(JOCTET));
		memset(src->buffer, 0, c_JPEGInputBufferSize * sizeof(JOCTET));
	}
	src = (cDataManagerJPEGSource*)cinfo->src;

	src->pub.init_source = JPEGInitSource;
	src->pub.fill_input_buffer = JPEGFillInputBuffer;
	src->pub.skip_input_data = JPEGSkipInputData;
	src->pub.resync_to_restart = jpeg_resync_to_restart;
	src->pub.term_source = JPEGTermSource;

	src->read_buffer = file;

	src->pub.bytes_in_buffer = 0;
	src->pub.next_input_byte = 0;
}

static HBITMAP PrepareFromBufferJPG(HWND hwnd, const void* pDataBuffer, int nBufferSize, int &widthd, int &heightd)
{
	try
	{
		if (pDataBuffer == NULL || nBufferSize <= 0)
			return NULL;
		if (memcmp(pDataBuffer, JPGHeader, min(strlen(JPGHeader), nBufferSize)) != 0)
			return NULL;
		jpeg_decompress_struct	cinfo;
		jpeg_membuffer_t buf;
		jpeg_error_mgr jerr;

		buf.amem_buff = (unsigned char*)pDataBuffer;
		buf.pos = 0;
		buf.size = nBufferSize;

		cinfo.err = jpeg_std_error(&jerr);
		jpeg_create_decompress(&cinfo);

		JPEGStdioSrc(&cinfo, &buf);
		jpeg_read_header(&cinfo, TRUE);

		if (!jpeg_start_decompress(&cinfo))
			return NULL;

		if(cinfo.output_components != 3)
			return NULL;
		if (cinfo.output_width <= 0 || cinfo.output_height <= 0)
		{
			jpeg_destroy_decompress(&cinfo);
			return NULL;
		}
		widthd = cinfo.output_width;
		heightd = cinfo.output_height;

		HBITMAP hOutputImage;
		hOutputImage = (HBITMAP)CreateDIB(hwnd, widthd, heightd, cinfo.output_components * 8);
		if (!hOutputImage)
		{
			jpeg_destroy_decompress(&cinfo);
			return NULL;
		}
		BITMAP btmOutputImage;
		::GetObject(hOutputImage, sizeof(BITMAP), &btmOutputImage);
		unsigned char* pImageBuffer = (unsigned char*)btmOutputImage.bmBits;
		if (!pImageBuffer)
		{
			DeleteObject(hOutputImage);
			jpeg_destroy_decompress(&cinfo);
			return NULL;
		}
		JSAMPROW* rows = new JSAMPROW[btmOutputImage.bmHeight];
		for (unsigned int i = 0; i < heightd; i++)
			rows[heightd - i - 1] = pImageBuffer + BitmapWidth(widthd, cinfo.output_components * 8) * (heightd - i - 1);

		while (cinfo.output_scanline < heightd)
		{
			jpeg_read_scanlines(&cinfo, rows + cinfo.output_scanline, 1);
			if(cinfo.output_components == 3)
				swap_rgb(rows[cinfo.output_scanline - 1], widthd * 3);
		}
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		delete[] rows;
		return hOutputImage;
	}
	catch (std::runtime_error e)
	{
		MessageBoxA(0, e.what(), "Error!", MB_ICONERROR);
	}
	return NULL;
}

inline void colortorgb(DWORD color, int& r, int& g, int& b)
{
	r = (color >> 16) & 0xff;
	g = (color >> 8) & 0xff;
	b = (color) & 0xff;
}

static void ApplyAlphaToLineWidth(unsigned char* pInputImageBuffer, unsigned char* pOutputImageBuffer, int width, DWORD Color)
{
	int r, g, b;
	colortorgb(Color, r, g, b);

	for (int i = 0, s = 0; i < width * 4; i+=4, s+=3)
	{
		pOutputImageBuffer[s] = (unsigned char)(r + (pInputImageBuffer[i] - r) * (((float)pInputImageBuffer[i + 3]) / 255));
		pOutputImageBuffer[s + 1] = (unsigned char)(g + (pInputImageBuffer[i + 1] - g) * (((float)pInputImageBuffer[i + 3]) / 255));
		pOutputImageBuffer[s + 2] = (unsigned char)(b + (pInputImageBuffer[i + 2] - b) * (((float)pInputImageBuffer[i + 3]) / 255));
	}
}

static HBITMAP ApplyAlphaLayerToColor(HWND hwnd, HBITMAP alpha_image, DWORD Color)
{
	BITMAP btmInputImage;
	::GetObject(alpha_image, sizeof(BITMAP), &btmInputImage);

	HBITMAP hOutputImage = (HBITMAP)CreateDIB(hwnd, btmInputImage.bmWidth, btmInputImage.bmHeight, 24);

	if (btmInputImage.bmBitsPixel != 32 || hOutputImage == NULL)
		return alpha_image;
	BITMAP btmOutputImage;
	::GetObject(hOutputImage, sizeof(BITMAP), &btmOutputImage);
	unsigned char* pInputImageBuffer = (unsigned char*)btmInputImage.bmBits;
	unsigned char* pOutputImageBuffer = (unsigned char*)btmOutputImage.bmBits;
	for (int nY = 0; nY < btmOutputImage.bmHeight; ++nY)
		ApplyAlphaToLineWidth(&pInputImageBuffer[nY * BitmapWidth(btmInputImage.bmWidth, btmInputImage.bmBitsPixel)], &pOutputImageBuffer[nY * BitmapWidth(btmOutputImage.bmWidth, btmOutputImage.bmBitsPixel)], btmOutputImage.bmWidth, Color);
	DeleteObject(alpha_image);
	return hOutputImage;
}

win_image& win_image::make_RGB(HWND hwnd, DWORD Color)
{
	hPicture = ApplyAlphaLayerToColor(hwnd, hPicture, Color);
	return *this;
}

static HBITMAP PrepareFromBufferPNG(HWND hwnd, const void* pDataBuffer, int nBufferSize, int& widthP, int& heightP)
{
	const int number = 8;

	if (pDataBuffer == NULL || nBufferSize <= 0)
		return NULL;

	unsigned char* pBuffer = (unsigned char*)pDataBuffer;

	png_byte  sig[number] = { 0 };
	if (!png_check_sig(const_cast<png_bytep>(pBuffer), number))
		return NULL;

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	png_infop info_ptr = png_create_info_struct(png_ptr);

	png_membuffer_t membuff = { 0 };

	membuff.amem_buff = pBuffer + number;
	membuff.size = nBufferSize - number;

	png_set_read_fn(png_ptr, (void*)&membuff, png_read_memory_data_fn);
	png_set_sig_bytes(png_ptr, number);
	png_read_info(png_ptr, info_ptr);

	png_uint_32 width = 0,
		height = 0;

	int			bit_depth = 0;
	int			color_type = 0;

	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);


	if (bit_depth == 16)
		png_set_strip_16(png_ptr);
	if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8)
		png_set_palette_to_rgb(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);
	if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	double	dGamma = 0.0f;

	if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
		png_set_gamma(png_ptr, 2.2, dGamma);
	else
		png_set_gamma(png_ptr, 2.2, 0.45455);

	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);

	const int nBitsCount = (PNG_COLOR_TYPE_RGBA == color_type) ? (32) : (24);

	png_uint_32		nChannelRow = (png_uint_32)png_get_channels(png_ptr, info_ptr);
	png_byte** row_pointers = new png_byte * [height];

	HBITMAP hOutputImage = (HBITMAP)CreateDIB(hwnd, width, height, nBitsCount);

	if (hOutputImage == NULL)
	{
		delete[] row_pointers;
		return NULL;
	}

	BITMAP btmOutputImage;
	::GetObject(hOutputImage, sizeof(BITMAP), &btmOutputImage);

	unsigned char* pImageBuffer = (unsigned char*)btmOutputImage.bmBits;

	for (unsigned int nRowNum = 0; nRowNum < height; nRowNum++)
		row_pointers[nRowNum] = pImageBuffer + nRowNum * BitmapWidth(width, nChannelRow * 8);

	png_read_image(png_ptr, row_pointers);
	for (int nY = 0; nY < btmOutputImage.bmHeight; ++nY)
	{
		switch (color_type)
		{
		case PNG_COLOR_TYPE_RGB:
		{
			swap_rgb(&pImageBuffer[nY * BitmapWidth(btmOutputImage.bmWidth, btmOutputImage.bmBitsPixel)], btmOutputImage.bmWidthBytes);
			break;
		}
		case PNG_COLOR_TYPE_RGB_ALPHA:
		{
			swap_rgba(&pImageBuffer[nY * BitmapWidth(btmOutputImage.bmWidth, btmOutputImage.bmBitsPixel)], btmOutputImage.bmWidthBytes);
			break;
		}
		}
	}
	delete[] row_pointers;
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	widthP = btmOutputImage.bmWidth;
	heightP = btmOutputImage.bmHeight;

	return hOutputImage;
}

std::string LIB_IMAGE::GetDataFromResourceUtil(std::wstring ResType, int Res, HMODULE hMod)
{
	std::string Ret;
	HRSRC hResBin = FindResourceW(hMod, MAKEINTRESOURCEW(Res), ResType.c_str());
	if (!hResBin)
		return Ret;
	HGLOBAL mem_block = LoadResource(hMod, hResBin);
	if(!mem_block)
		return Ret;
	void* pointer = LockResource(mem_block);
	size_t SizeB = (int)SizeofResource(hMod, hResBin);

	if (SizeB <= 0 || mem_block == NULL)
	{
		if (mem_block != NULL)
			FreeResource(mem_block);
		return Ret;
	}
	Ret.resize(SizeB);
	memcpy((char*)Ret.data(), pointer, SizeB);
	FreeResource(mem_block);
	return Ret;
}

static win_image makeErrorBitmap(HWND hwnd, win_image_size size,  bool show_error_bitmap) {
	if (show_error_bitmap) {
		auto err = PrepareFromBufferPNG(hwnd, ERROR_ICON, ERROR_ICON_SIZE, size.size_x, size.size_y);
		return win_image(err, size);
	}
	return win_image(nullptr, size);
}

win_image LIB_IMAGE::PrepareImageFromBufferByType(HWND hwnd, const void* pDataBuffer, int nBufferSize, win_image_type Type, bool show_error_bitmap)
{
	win_image_size size;
	if (Type == win_image_type::WIN_IMAGE_JPG)
	{
		HBITMAP bp = PrepareFromBufferJPG(hwnd, pDataBuffer, nBufferSize, size.size_x, size.size_y);
		if (bp == NULL) {
			return makeErrorBitmap(hwnd, size, show_error_bitmap);
		}
		else
			return win_image(bp, size);
	}
	else if (Type == win_image_type::WIN_IMAGE_PNG)
	{
		HBITMAP bp = PrepareFromBufferPNG(hwnd, pDataBuffer, nBufferSize, size.size_x, size.size_y);
		if (bp == NULL) {
			return makeErrorBitmap(hwnd, size, show_error_bitmap);
		}
		else
			return win_image(bp, size);
	}
	return makeErrorBitmap(hwnd, size, show_error_bitmap);
}

win_image LIB_IMAGE::PrepareImageFromBufferAutoType(HWND hwnd, const void* pDataBuffer, int nBufferSize, bool show_error_bitmap)
{
	win_image_size size;
	if (pDataBuffer == NULL || nBufferSize < strlen(JPGHeader) || nBufferSize < 8) {
		return makeErrorBitmap(hwnd, size, show_error_bitmap);
	}
	if (memcmp(pDataBuffer, JPGHeader, min(strlen(JPGHeader), nBufferSize)) == 0)
	{
		HBITMAP bp = PrepareFromBufferJPG(hwnd, pDataBuffer, nBufferSize, size.size_x, size.size_y);
		if (bp == NULL) {
			return makeErrorBitmap(hwnd, size, show_error_bitmap);
		}
		else
			return win_image(bp, size);
	}
	else
	{
		const int number = 8;
		unsigned char* pBuffer = (unsigned char*)pDataBuffer;
		png_byte  sig[number] = { 0 };
		if (!png_check_sig(const_cast<png_bytep>(pBuffer), number))
			return makeErrorBitmap(hwnd, size, show_error_bitmap);
		HBITMAP bp = PrepareFromBufferPNG(hwnd, pDataBuffer, nBufferSize, size.size_x, size.size_y);
		if (bp == NULL) {
			return makeErrorBitmap(hwnd, size, show_error_bitmap);
		}
		else
			return win_image(bp, size);
	}
	return makeErrorBitmap(hwnd, size, show_error_bitmap);
}

std::string win_image::decompress_gzip(const std::string& str)
{
	z_stream zs;                        // z_stream is zlib's control structure
	memset(&zs, 0, sizeof(zs));
	const int MOD_GZIP_ZLIB_WINDOWSIZE = 15;
	if (inflateInit2(&zs, MOD_GZIP_ZLIB_WINDOWSIZE + 16) != Z_OK)
		throw(std::runtime_error("inflateInit failed while decompressing."));

	zs.next_in = (Bytef*)str.data();
	zs.avail_in = str.size();

	int ret;
	std::vector<char> outbuffer(32768);
	std::string outstring;

	// get the decompressed bytes blockwise using repeated calls to inflate
	do {
		zs.next_out = reinterpret_cast<Bytef*>(outbuffer.data());
		zs.avail_out = sizeof(outbuffer);

		ret = inflate(&zs, 0);

		if (outstring.size() < zs.total_out) {
			outstring.append(outbuffer.data(),
				zs.total_out - outstring.size());
		}

	} while (ret == Z_OK);

	inflateEnd(&zs);

	if (ret != Z_STREAM_END) {          // an error occurred that was not EOF
		std::ostringstream oss;
		oss << "Exception during zlib decompression: (" << ret << ") "
			<< zs.msg;
		throw(std::runtime_error(oss.str()));
	}

	return outstring;
}

win_image LIB_IMAGE::PrepareImageFromSVG(HWND hwnd, int targetWidth, int targetHeight, const void* pDataBuffer, int nBufferSize, uint32_t bgColor) {
	win_image_size size;
	unique_ptr<tvg::SwCanvas> canvas;
	//Canvas Engine
	tvg::CanvasEngine tvgEngine = tvg::CanvasEngine::Sw;

	//Threads Count
	auto threads = thread::hardware_concurrency();

	//Initialize ThorVG Engine
	if (tvg::Initializer::init(tvgEngine, threads) != tvg::Result::Success) {
		return makeErrorBitmap(hwnd, size, true);
	}

	//Create a Canvas
	canvas = tvg::SwCanvas::gen();
	if (!canvas) {
		tvg::Initializer::term(tvgEngine);
		return makeErrorBitmap(hwnd, size, true);
	}

	auto picture = tvg::Picture::gen();
	tvg::Result result = picture->load((const char*)pDataBuffer, nBufferSize, "svg", false);
	if (result != tvg::Result::Success) {
		tvg::Initializer::term(tvgEngine);
		return makeErrorBitmap(hwnd, size, true);
	}
	if (targetWidth == 0 || targetHeight == 0) {
		float fw, fh;
		picture->size(&fw, &fh);
		targetWidth = static_cast<uint32_t>(fw);
		targetHeight = static_cast<uint32_t>(fh);
	}
	else {
		picture->size(targetWidth, targetHeight);
	}
	size.size_x = targetWidth;
	size.size_y = targetHeight;
	HBITMAP hOutputImage = (HBITMAP)CreateDIB(hwnd, targetWidth, targetHeight, 32);
	if (hOutputImage == NULL) {
		tvg::Initializer::term(tvgEngine);
		return makeErrorBitmap(hwnd, size, true);
	}

	BITMAP btmOutputImage;
	::GetObject(hOutputImage, sizeof(BITMAP), &btmOutputImage);

	unsigned char* pImageBuffer = (unsigned char*)btmOutputImage.bmBits;
	if (canvas->target((uint32_t*)pImageBuffer, targetWidth, targetWidth, targetHeight, tvg::SwCanvas::ARGB8888) != tvg::Result::Success) {
		tvg::Initializer::term(tvgEngine);
		DeleteObject(hOutputImage);
		return makeErrorBitmap(hwnd, size, true);
	}
	//Background color if needed
	if (bgColor != 0xffffffff) {
		uint8_t r = (uint8_t)((bgColor & 0xff0000) >> 16);
		uint8_t g = (uint8_t)((bgColor & 0x00ff00) >> 8);
		uint8_t b = (uint8_t)((bgColor & 0x0000ff));

		auto shape = tvg::Shape::gen();
		shape->appendRect(0, 0, targetWidth, targetHeight, 0, 0);
		shape->fill(r, g, b, 255);

		if (canvas->push(move(shape)) != tvg::Result::Success) {
			tvg::Initializer::term(tvgEngine);
			DeleteObject(hOutputImage);
			return makeErrorBitmap(hwnd, size, true);
		}
	}

	//Drawing
	canvas->push(move(picture));
	canvas->draw();
	canvas->sync();
	canvas->clear(true);
	tvg::Initializer::term(tvgEngine);
	return win_image(hOutputImage, size);
}

HBITMAP LIB_IMAGE::PrepareImageFromSVGResourceToIcon(HWND hwnd, int Res, int padding, uint32_t bgColor) {
	std::string DataPic = GetDataFromResourceUtil(L"SVG", Res);
	RECT jj;
	GetClientRect(hwnd, &jj);
	return PrepareImageFromSVG(hwnd, jj.right - jj.left - padding, jj.bottom - jj.top - padding, DataPic.data(), (int)DataPic.size(), bgColor).get_hBitmap(true);
}

win_image LIB_IMAGE::PrepareImageFromSVGResource(HWND hwnd, int Res, int padding, uint32_t bgColor) {
	std::string DataPic = GetDataFromResourceUtil(L"SVG", Res);
	RECT jj;
	GetClientRect(hwnd, &jj);
	return PrepareImageFromSVG(hwnd, jj.right - jj.left - padding, jj.bottom - jj.top - padding, DataPic.data(), (int)DataPic.size(), bgColor);
}

static PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{
	BITMAP bmp;
	PBITMAPINFO pbmi;
	WORD    cClrBits;

	// Retrieve the bitmap color format, width, and height.  
	GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);

	// Convert the color format to a count of bits.  
	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	// Allocate memory for the BITMAPINFO structure. (This structure  
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD  
	// data structures.)  

	if (cClrBits < 24)
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1 << cClrBits));

	// There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel 

	else
		pbmi = (PBITMAPINFO)LocalAlloc(LPTR,
			sizeof(BITMAPINFOHEADER));

	// Initialize the fields in the BITMAPINFO structure.  

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = bmp.bmWidth;
	pbmi->bmiHeader.biHeight = bmp.bmHeight;
	pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbmi->bmiHeader.biClrUsed = (1 << cClrBits);

	// If the bitmap is not compressed, set the BI_RGB flag.  
	pbmi->bmiHeader.biCompression = BI_RGB;

	// Compute the number of bytes in the array of color  
	// indices and store the result in biSizeImage.  
	// The width must be DWORD aligned unless the bitmap is RLE 
	// compressed. 
	pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8
		* pbmi->bmiHeader.biHeight;
	// Set biClrImportant to 0, indicating that all of the  
	// device colors are important.  
	pbmi->bmiHeader.biClrImportant = 0;
	return pbmi;
}

void LIB_IMAGE::CreateBMPFile(LPCTSTR pszFile, HBITMAP hBMP)
{
	HANDLE hf;                 // file handle  
	BITMAPFILEHEADER hdr;       // bitmap file-header  
	PBITMAPINFOHEADER pbih;     // bitmap info-header  
	LPBYTE lpBits;              // memory pointer  
	DWORD dwTotal;              // total count of bytes  
	DWORD cb;                   // incremental count of bytes  
	BYTE* hp;                   // byte pointer  
	DWORD dwTmp;
	PBITMAPINFO pbi;
	HDC hDC;

	hDC = CreateCompatibleDC(GetWindowDC(GetDesktopWindow()));
	SelectObject(hDC, hBMP);

	pbi = CreateBitmapInfoStruct(hBMP);

	pbih = (PBITMAPINFOHEADER)pbi;
	lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS);

	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);

	hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL);

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwTmp, (NULL));

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL);

	// Close the .BMP file.  
	CloseHandle(hf);

	// Free memory.  
	GlobalFree((HGLOBAL)lpBits);
}
