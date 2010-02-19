import eeml.*;
DataOut dOut;

void setup(){
    // set up DataOut object; requires the port that you are serving on
    dOut = new DataOut(this, 5210);
    
    //  and add and tag a datastream    
    dOut.addData(0,"my tag 0, tag2, tag3");
}


void draw()
{
    // do whatever needs doing in the main loop
}

// onReceiveRequest is run every time your app receives a request for data; so you 
// would normally run d.update on all your data streams. 
void onReceiveRequest(DataOut d){ 
    d.update(0, random(1000)); // updates stream 0 with a value as required
}
