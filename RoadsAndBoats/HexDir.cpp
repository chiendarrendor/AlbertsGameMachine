
int HexDirX(int i_x,int i_y,int i_dir)
{
  switch(i_dir)
  {
  case 3: return i_x + 1; 
  case 6: return i_x - 1; 
  case 2:
  case 4: return i_x + (i_y % 2);
  case 1:
  case 5: return i_x - 1 + (i_y % 2); 
  }
  // shouldn't get here...
  return -100;
}

int HexDirY(int i_x,int i_y,int i_dir)
{
  switch(i_dir)
  {
  case 1:
  case 2: return i_y - 1;
  case 4:
  case 5: return i_y + 1;
  case 3:
  case 6: return i_y;
  }
  // shouldn't get here
  return -100;
}

int BackDir(int i_dir)
{
  switch(i_dir)
  {
  case 1: return 4;
  case 2: return 5;
  case 3: return 6;
  case 4: return 1;
  case 5: return 2;
  case 6: return 3;
  }
  return -100;
}
