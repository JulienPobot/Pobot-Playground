void separator()
{
  Serial.print(";");
}

int allocRam()
{
  if((int)__brkval != 0)
    return ((int)__brkval) - ((int)&__bss_end);
  else
    return 0;
}

int freeRam()
{
  int free_memory;

  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);

  return free_memory;
}


int limit(int mn,int in,int mx) 
{
  int out;

  if (in<mn) out=mn;
  else if (in>mx) out=mx;
  else if (in>=-DEADBAND && in<=DEADBAND) out=0;
  else out=in;

  return out;
}

float arctan2(float y, float x) {
  // Taken from: http://www.dspguru.com/comp.dsp/tricks/alg/fxdatan2.htm
  float coeff_1 = PI/4;
  float coeff_2 = 3*coeff_1;
  float abs_y = abs(y)+1e-10;      // kludge to prevent 0/0 condition
  float r, angle;

  if (x >= 0) {
    r = (x - abs_y) / (x + abs_y);
    angle = coeff_1 - coeff_1 * r;
  }
  else {
    r = (x + abs_y) / (abs_y - x);
    angle = coeff_2 - coeff_1 * r;
  }
  if (y < 0)
    return(-angle);     // negate if in quad III or IV
  else
    return(angle);
}



