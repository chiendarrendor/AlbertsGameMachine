#include "ANSITextGrid.hpp"

ANSITextGrid::ANSITextGrid(int i_width,int i_height) :
	m_width(i_width),
	m_height(i_height)
{
}

void ANSITextGrid::SetChar(int i_x,int i_y,char i_c,
		     ANSIProperties i_attr,ANSIProperties i_back,ANSIProperties i_fore)
{
	ANSIText at;
	at.m_c = i_c;
	at.m_attr = i_attr;
	at.m_fore = i_fore;
	at.m_back = i_back;

	ANSIKey ak;
	ak.m_x = i_x;
	ak.m_y = i_y;

	m_chars[ak] = at;
}

std::ostream &operator<<(std::ostream &o,const ANSIText &i_at)
{
	o << '\033';
	o << '[';
	o << i_at.m_fore;
	o << ';';
	o << i_at.m_back;
	o << ';';
	o << i_at.m_attr;
	o << 'm';
	o << i_at.m_c;
	return o;
}

bool operator<(const ANSIKey &i_left,
               const ANSIKey &i_right)
{
  if (i_left.m_x != i_right.m_x)
  {
    return i_left.m_x < i_right.m_x;
  }
  return i_left.m_y < i_right.m_y;
}



std::ostream &operator<<(std::ostream &o,const ANSITextGrid &i_atg)
{
	int i,j;
	ANSIText defat;
	defat.m_c = ' ';
	defat.m_attr = RESET;
	defat.m_fore = FORE_BLACK;
	defat.m_back = BACK_WHITE;


	for (i = 0 ; i < i_atg.m_height ; ++i)
	{
		for (j = 0 ; j <= i_atg.m_width ; ++j)
		{
			ANSIKey ak;
			ak.m_x = j;
			ak.m_y = i;
			std::map<ANSIKey,ANSIText>::const_iterator cit =  i_atg.m_chars.find(ak);
			if (cit == i_atg.m_chars.end())
			{
				o << defat;
			}
			else
			{
				o << cit->second;
			}
		}
    o << std::endl;
	}
	return o;
}

