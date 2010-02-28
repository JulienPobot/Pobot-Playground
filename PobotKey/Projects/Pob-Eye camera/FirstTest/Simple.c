 #include <pob-eye.h>

int main(void)
{
        InitPobeye2();
        InitUART0(115200);

        SendString("Hello world from POB-EYE2!");

        while(1) ;      

        return 0;
}