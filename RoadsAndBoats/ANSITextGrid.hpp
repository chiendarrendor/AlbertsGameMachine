#include <ostream>
#include <map>


enum ANSIProperties
{
	RESET = 0,
	BRIGHT = 1,
	DIM = 2,
	UNDERSCORE = 4,
	BLINK = 5,
	REVERSE = 7,
	HIDDEN = 8,
	FORE_BLACK = 30,
	FORE_RED = 31,
	FORE_GREEN = 32,
	FORE_YELLOW = 33,
	FORE_BLUE = 34,
	FORE_MAGENTA = 35,
	FORE_CYAN = 36,
	FORE_WHITE = 37,
	BACK_BLACK = 40,
	BACK_RED = 41,
	BACK_GREEN = 42,
	BACK_YELLOW = 43,
	BACK_BLUE = 44,
	BACK_MAGENTA = 45,
	BACK_CYAN = 46,
	BACK_WHITE = 47
};

struct ANSIText
{
  char m_c;
  ANSIProperties m_attr;
  ANSIProperties m_fore;
  ANSIProperties m_back;
};

struct ANSIKey
{
  int m_x;
  int m_y;
};

	

class ANSITextGrid
{
public:
	ANSITextGrid(int i_width,int i_height);

	void SetChar(int i_x,int i_y,char i_c,
		     ANSIProperties i_attr,ANSIProperties i_back,ANSIProperties i_fore);
	
	friend std::ostream &operator<<(std::ostream &o,const ANSITextGrid &i_atg);
private:

	int m_width;
	int m_height;
	
	std::map<ANSIKey,ANSIText> m_chars;


};

std::ostream &operator<<(std::ostream &o,const ANSITextGrid &i_atg);
