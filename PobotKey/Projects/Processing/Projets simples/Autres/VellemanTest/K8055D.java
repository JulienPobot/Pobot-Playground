package org.xvolks.jnative.util;

import processing.core.*;

import org.xvolks.test.bug.*;
import org.xvolks.jnative.util.ole.*;
import org.xvolks.jnative.util.windows.hooks.*;
import org.xvolks.jnative.pointers.*;
import org.xvolks.jnative.logging.*;
import org.xvolks.jnative.com.interfaces.*;
import org.xvolks.test.windows.trayicon.*;
import org.xvolks.jnative.util.mapi.*;
import org.xvolks.jnative.com.typebrowser.business.export.*;
import org.xvolks.jnative.misc.*;
import org.xvolks.jnative.util.mapi.structs.*;
import org.xvolks.jnative.misc.basicStructures.*;
import org.xvolks.test.com.*;
import org.xvolks.jnative.util.windows.hotkey.*;
import org.xvolks.jnative.pointers.memory.*;
import org.xvolks.jnative.misc.registry.*;
import org.xvolks.test.windows.*;
import org.xvolks.jnative.exceptions.*;
import org.xvolks.jnative.com.typebrowser.gui.*;
import org.xvolks.jnative.com.utils.*;
import org.xvolks.jnative.util.windows.*;
import org.xvolks.jnative.util.constants.winuser.*;
import org.xvolks.test.callbacks.*;
import org.xvolks.jnative.com.typebrowser.gui.panels.*;
import org.xvolks.jnative.*;
import org.xvolks.jnative.util.win32session.*;
import org.xvolks.test.callbacks.linux.*;
import org.xvolks.jnative.util.constants.*;
import org.xvolks.jnative.com.typebrowser.business.*;
import org.xvolks.test.*;
import org.xvolks.jnative.misc.machine.*;
import org.xvolks.jnative.util.*;
import org.xvolks.jnative.util.windows.structures.*;
import org.xvolks.jnative.com.*;

/**
 * @author Julien Holtzer 
 * @version 11-01-2009
 *
 * This class is not complete at all, just for example of using JNative
 * http://www.pobot.org/Utiliser-une-DLL-dans-un-programme.html
 */
public class K8055D extends PApplet
{

  public static final String DLL_NAME = "K8055D.DLL";

  // Native-side function names
  private static JNative nClearAllAnalog;
  private static JNative nClearAllDigital;
  private static JNative nClearAnalogChannel;
  private static JNative nClearDigitalChannel;
  private static JNative nCloseDevice;
  private static JNative nOpenDevice;
  private static JNative nOutputAllAnalog;
  private static JNative nOutputAnalogChannel;
  private static JNative nReadAllAnalog;
  private static JNative nReadAllDigital;
  private static JNative nReadAnalogChannel;
  private static JNative nReadCounter;
  private static JNative nReadDigitalChannel;
  private static JNative nResetCounter;
  private static JNative nSearchDevices;
  private static JNative nSetAllAnalog;
  private static JNative nSetAllDigital;
  private static JNative nSetAnalogChannel;
  private static JNative nSetCounterDebounceTime;
  private static JNative nSetCurrentDevice;
  private static JNative nSetDigitalChannel;
  private static JNative nVersion; 
  private static JNative nWriteAllDigital;

  /**
   * Init
   */
  public static void initCard(int address) 
  {
    JNative.setLoggingEnabled(false); // show what JNative is doing!
    try {      
      K8055D.OpenDevice(address);
      K8055D.ClearAllDigital();
      K8055D.ClearAllAnalog();
    } 
    catch (NativeException ne)
    {
      println("Error ! "+ne.toString()); 
    }
    catch (IllegalAccessException e)
    {
      println("Error ! "+e.toString()); 
    }    
  }

  ///////////////////////////////////////////////////////////////////////////////////////
  // Implementation of Arduino-like functions for easier access from Processing sketch //
  ///////////////////////////////////////////////////////////////////////////////////////

  /**
   *
   */
  public static void analogClear()
  {
    try {
      ClearAllAnalog();
    } 
    catch (NativeException ne)  {    
      println("Error ! "+ne.toString());   
    }
    catch (IllegalAccessException e)  {    
      println("Error ! "+e.toString());   
    }
  }

  /**
   *
   */
  public static void digitalClear()
  {
    try {
      ClearAllDigital();
    } 
    catch (NativeException ne)  {    
      println("Error ! "+ne.toString());   
    }
    catch (IllegalAccessException e)  {    
      println("Error ! "+e.toString());   
    }
  }



  /**
   *
   */
  public static void analogFull()
  {
    try {
      SetAllAnalog();
    } 
    catch (NativeException ne)  {    
      println("Error ! "+ne.toString());   
    }
    catch (IllegalAccessException e)  {    
      println("Error ! "+e.toString());   
    }
  }

  /**
   *
   */
  public static void digitalFull()
  {
    try {
      SetAllDigital();
    } 
    catch (NativeException ne)  {    
      println("Error ! "+ne.toString());   
    }
    catch (IllegalAccessException e)  {    
      println("Error ! "+e.toString());   
    }
  }

  /**
   * Write a high or low value to a digital pin (or channel as K8055 refers to)
   * @arg pin : channel from 1 to 5
   * @arg value : true or false for HIGH or LOW
   */
  public static void digitalWrite(int pin, boolean value)
  {
    try {
      if (value) {
        SetDigitalChannel(pin);      
      } 
      else {
        ClearDigitalChannel(pin); 
      } 
    } 
    catch (NativeException ne)  {    
      println("Error ! "+ne.toString());   
    }
    catch (IllegalAccessException e)  {    
      println("Error ! "+e.toString());   
    }
  }

  /**
   * Reads the value from a specified digital pin (or channel as K8055 refers to)
   * @arg pin : channel from 1 to 5
   * @return boolean value (true for HIGH and false for LOW)
   */
  public static boolean digitalRead(int pin)
  {
    try {
      return (ReadDigitalChannel(pin) == 1);
    } 
    catch (NativeException ne)  {    
      println("Error ! "+ne.toString());   
    }
    catch (IllegalAccessException e)  {    
      println("Error ! "+e.toString());   
    }
    return false;
  }

  /////////////////////////////////////////////
  // JNative implementation of DLL functions //
  /////////////////////////////////////////////

  /**
   *
   */
  private static void ClearAllAnalog() throws NativeException, IllegalAccessException 
  {
    if(nClearAllAnalog == null) 
    {
      nClearAllAnalog = new JNative(DLL_NAME, "ClearAllAnalog");  // The actual "decorated" name in the DLL
    }
    nClearAllAnalog.invoke();  // Finally, execute the native function 
  }

  /**
   *
   */
  public static void ClearAllDigital() throws NativeException, IllegalAccessException 
  {
    if(nClearAllDigital == null) 
    {
      nClearAllDigital = new JNative(DLL_NAME, "ClearAllDigital");  // The actual "decorated" name in the DLL
    }
    nClearAllDigital.invoke();  // Finally, execute the native function 
  } 

  /**
   *
   */
  public static void ClearAnalogChannel(int channel) throws NativeException, IllegalAccessException 
  {
    if(nClearAnalogChannel == null) 
    {
      nClearAnalogChannel = new JNative(DLL_NAME, "ClearAnalogChannel");  // The actual "decorated" name in the DLL
    }
    nClearAnalogChannel.setParameter(0, channel);
    nClearAnalogChannel.invoke();  // Finally, execute the native function 
  }

  /**
   *
   */
  public static void ClearDigitalChannel(int channel) throws NativeException, IllegalAccessException 
  {
    if(nClearDigitalChannel == null) 
    {
      nClearDigitalChannel = new JNative(DLL_NAME, "ClearDigitalChannel");  // The actual "decorated" name in the DLL
    }
    nClearDigitalChannel.setParameter(0, channel);
    nClearDigitalChannel.invoke();  // Finally, execute the native function 
  }

  /**
   *
   */
  public static void CloseDevice() throws NativeException, IllegalAccessException 
  {
    if(nCloseDevice == null) 
    {
      nCloseDevice = new JNative(DLL_NAME, "CloseDevice");  // The actual "decorated" name in the DLL
    }
    nCloseDevice.invoke();  // Finally, execute the native function 
  }

  /**
   *
   */
  public static int OpenDevice(int address) throws NativeException, IllegalAccessException 
  {
    if(nOpenDevice == null) 
    {
      nOpenDevice = new JNative(DLL_NAME, "OpenDevice");  // The actual "decorated" name in the DLL
      nOpenDevice.setRetVal(Type.INT);      
    }
    nOpenDevice.setParameter(0, address);
    nOpenDevice.invoke();  // Finally, execute the native function 
    return nOpenDevice.getRetValAsInt(); // get native call's return value
  }

  /**
   *
   */
  public static void OutputAllAnalog(int value0, int value1) throws NativeException, IllegalAccessException 
  {
    if(nOutputAllAnalog == null) 
    {
      nOutputAllAnalog = new JNative(DLL_NAME, "OutputAllAnalog");  // The actual "decorated" name in the DLL
    }
    nOutputAllAnalog.setParameter(0, value0);
    nOutputAllAnalog.setParameter(1, value1);
    nOutputAllAnalog.invoke();  // Finally, execute the native function 
  }

  /**
   *
   */
  public static void OutputAnalogChannel(int channel, int value) throws NativeException, IllegalAccessException 
  {
    if(nOutputAnalogChannel == null) 
    {
      nOutputAnalogChannel = new JNative(DLL_NAME, "OutputAnalogChannel");  // The actual "decorated" name in the DLL
    }
    nOutputAnalogChannel.setParameter(0, channel);
    nOutputAnalogChannel.setParameter(1, value);
    nOutputAnalogChannel.invoke();  // Finally, execute the native function 
  }

  /**
   * Not implemented in this version. Mail to contact@pobot.org
   */
  // public static void ReadAllAnalog(int data1, int data2) throws NativeException, IllegalAccessException 

  /**
   *
   */
  public static int ReadAnalogChannel(int channel) throws NativeException, IllegalAccessException 
  {
    if(nReadAnalogChannel == null) 
    {
      nReadAnalogChannel = new JNative(DLL_NAME, "ReadAnalogChannel");  // The actual "decorated" name in the DLL
      nReadAnalogChannel.setRetVal(Type.INT);
    }
    nReadAnalogChannel.setParameter(0, channel);
    nReadAnalogChannel.invoke();  // Finally, execute the native function 
    return nReadAnalogChannel.getRetValAsInt(); // get native call's return value
  }  

  /**
   *
   */
  public static int ReadDigitalChannel(int channel) throws NativeException, IllegalAccessException 
  {
    if(nReadDigitalChannel == null) 
    {
      nReadDigitalChannel = new JNative(DLL_NAME, "ReadDigitalChannel");  // The actual "decorated" name in the DLL
      nReadDigitalChannel.setRetVal(Type.INT);
    }
    nReadDigitalChannel.setParameter(0, channel);
    nReadDigitalChannel.invoke();  // Finally, execute the native function 
    return nReadDigitalChannel.getRetValAsInt(); // get native call's return value
  }

  /**
   *
   */
  public static int SearchDevices() throws NativeException, IllegalAccessException 
  {
    if(nSearchDevices == null) 
    {
      nSearchDevices = new JNative(DLL_NAME, "SearchDevices");  // The actual "decorated" name in the DLL
      nSearchDevices.setRetVal(Type.INT);
    }
    nSearchDevices.invoke();  // Finally, execute the native function     
    return nSearchDevices.getRetValAsInt(); // get native call's return value
  }

  /**
   *
   */
  public static void SetAllAnalog() throws NativeException, IllegalAccessException 
  {
    if(nSetAllAnalog == null) 
    {
      nSetAllAnalog = new JNative(DLL_NAME, "SetAllAnalog");  // The actual "decorated" name in the DLL
    }
    nSetAllAnalog.invoke();  // Finally, execute the native function 
  }

  /**
   *
   */
  public static void SetAllDigital() throws NativeException, IllegalAccessException 
  {
    if(nSetAllDigital == null) 
    {
      nSetAllDigital = new JNative(DLL_NAME, "SetAllDigital");  // The actual "decorated" name in the DLL
    }
    nSetAllDigital.invoke();  // Finally, execute the native function 
  }   

  /**
   *
   */
  public static void SetDigitalChannel(int channel) throws NativeException, IllegalAccessException 
  {
    if(nOutputAnalogChannel == null) 
    {
      nOutputAnalogChannel = new JNative(DLL_NAME, "SetDigitalChannel");  // The actual "decorated" name in the DLL
    }
    nOutputAnalogChannel.setParameter(0, channel);
    nOutputAnalogChannel.invoke();  // Finally, execute the native function 
  }

  // Java-side function name
  public static int Version() throws NativeException, IllegalAccessException 
  {
    if(nVersion == null) 
    {
      nVersion = new JNative(DLL_NAME, "Version");  // The actual "decorated" name in the DLL
      //BOOL is in fact an INT
      nVersion.setRetVal(Type.INT);  // tell JNative's DLL what kind of return data to expect
    }

    nVersion.invoke();  // Finally, execute the native function 
    return nVersion.getRetValAsInt(); // get native call's return value
  }
}

