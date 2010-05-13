void setup()
{
  size(500,500,P3D);
  background(0);
  fill(255);
  rotate(45,45,45,45);
  //ortho(0,100,0,100,0,100);  
}

void draw()
{
  translate(mouseX,mouseY);
 box(2);
 delay(100);
}
