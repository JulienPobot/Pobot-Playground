/*-------------------------------------------------------
* Copyright (C) 2009 Digi International, All Rights Reserved.
*
* $Id: API_Routines.h,v 1.2 2010/04/05 22:35:37 DIGI\pmillett Exp $
*
* This software is provided as instructional material without charge 
* by Digi International for use by its employees and customers
* subject to the following terms.
*
* PERMISSION
* Permission is hereby granted, free of charge, to any person obtaining 
* a copy of this software, to deal with it without restriction, 
* including without limitation the rights to use, copy,  modify, merge, publish, 
* distribute, sublicense, and/or sell copies of it, and to permit persons to 
* whom it is furnished to do so, provided the above copyright notice 
* and this permission notice are included in all derived works
* and the use of this software is restricted to Digi products.
*
* WARRANTY
* THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, OR NONINFRINGEMENT.  
*
* LIABILITY
* IN NO EVENT SHALL DIGI INTERNATIONAL BE LIABLE FOR ANY CLAIM, DAMAGES, 
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, 
* ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE, OR THE USE 
* OR OTHER DEALINGS WITH THE SOFTWARE.
*
* REQUEST FOR IMPROVEMENTS
* If you have suggestions for improvements, or source code which corrects,
* enhances, or extends the functionality of this software, please forward
* them to the Tech Support group in the Lindon, Utah office 
* of Digi International addressed to the attention of 'Linus'.
*-------------------------------------------------------*/

#include "Byte.h"


#define BIGEND 1
#define LITEND 0

//
// Frames
//

typedef struct{
		uint8 a[MAX_RF_DATA+50];
} _Bytes;

// Modem Status (0x8A)
//
// Modem status messages are sent from the modem in response to specific conditions.
// It should be noted that a successful join and authentication will result in 
// two Modem Status messages being generated:  
// a 0x02 status to indicate joining was successful; 
// and a 0x10 (16 decimal) to indicate the subsequent authentication was successful.
//
// Received by external processor
typedef struct {
	uint8 status; // see _MODEM_STATUS
} _ModemStatus;

// AT Command (0x08)
// AT Command Queue Register Value (0x09)
//
// Both AT Command frame types support a query or set of a modem register. 
// The effects of the AT Command are immediate.
// But when setting registers with the AT Command Queue Register Value, 
// the values are queued and will not take effect until either an AT Command (0x08) 
// or ATAC command is issued. 
// Register queries for either frame type where no AT command data is present are returned immediately.
//
// AT Command Example: Query for "NJ" - [ 0x7E, 0X00, 0X04, 0X08, 0X52, 0X4E, 0X4A, 0X0D ]  
// AT Command Example: Set NJ to 0x40 - [ 0X7E, 0X00, 0X05, 0X08, 0X4D, 0X4E, 0X4A, 0X40, 0XD2 ]
//
// Transmitted by external processor
typedef struct {
	// Identifies the data frame for the host to match with a subsequent TxStatus.  
	// If set to zero, no response is sent.
	uint8 FrameId;	
	// Command name of 2 ASCII characters
	uint8 cmd[2];
	// If AT command data is present, then the command is a set.  
	// If none is present, then the command is a query (get).  
	// Binary data is sent as one, two, three, or four byte binary values.  
	// Keys are sent as big-endian hexadecimal values.  
	// Strings are terminated with a zero byte.
	uint8 cmdData[MAX_RF_DATA];
} _ATCommand, _ATCommandQueueRegisterValue;

// AT Command Response (0x88)
//
// In response to an AT Command message, the modem will send an AT Command Response message. 
// This frame will come back immediately  after using the AT Command Queue (0x09) 
// or after reinitializing (applying new register values) for AT Command ID (0x08) and ATAC command. 
// Note if a baud change is requested, the response will come at the new baud rate. 
// Some commands will send back multiple frames (ATMD,ATAS). 
// These commands will end by sending a frame with a status of ATCMD_OK.
//
// Received by external processor
typedef struct {
	// Identifies the data frame for the host to match with a subsequent TxStatus.  
	// If set to zero, no response is sent.
	uint8 FrameId;
	// Command name of 2 ASCII characters	
	uint8 cmd[2];
	uint8 status;	// see _COMMAND_STATUS
	// Parameter data in binary format in response to a preceding AT Command (0x08 or 0x09) query.  
	// If this frame is in response to a AT Command set request,  then this field is not returned.	
	uint8 cmdData[MAX_RF_DATA];
} _ATCommandResponse;

// Transmitted by external processor
// Response will be returned in _ZigBeeRxIndicator
typedef struct {
	uint8 FrameId;
	_Addr64 dest64Addr;
	_Addr16 dest16Addr;
	uint8 radius;
	uint8 options;
	uint8 RF_Data[MAX_RF_DATA];
} _ZigBeeTxRequest;

// Explicit Addressing ZigBee Command Frame (0x11)
//
// Note: multiple byte fields in the RF data should use little-endian byte order when the packet is addressed to the ZDO.
//
// Transmitted by external processor
// Response will be returned in _ZigBeeExplicitRxIndicator
typedef struct {
	// Identifies the data frame for the host to match with a subsequent TxStatus.  
	// If set to zero, no response is sent.
	uint8 FrameId;
	// Destination 64-bit (extended) address.  Broadcast address is 0xFFFF.  
	// Coordinator address is all 0x00's.	
	_Addr64 dest64Addr;
	// Destination network address (if known).  
	// Set to 0xFFFE for broadcast transmissions or if the destination network address is unknown.
	_Addr16 dest16Addr;
	// Source Endpoint for the transmission. 0x00 for ZDO.
	uint8 srcEndpoint;
	// Destination endpoint for the transmission.  0x00 for ZDO.
	uint8 destEndpoint;
	// Cluster ID used in the transmission.
	uint8 clusterId[2]; 
	// Profile ID used in the transmission (0x0109 for Smart Energy).
	uint8 profileId[2];
	// Sets the maximum number of hops a broadcast transmission can traverse.  
	// If set to 0, the transmission radius will be set to the network maximum hops value.
	uint8 radius;
	// 0x20 - Enable APS end-to-end security.
	uint8 options;
	// A field of up to 255 bytes in length, reduced by encryption, source routing, 
	// and APS security overhead.  See the section on Fragmentation.
	uint8 RF_Data[MAX_RF_DATA];
} _ExplicitAddressingZigBeeCommandFrame;

// ZigBee Register Joining Device (0x16)
//
// Registers a new device into the trust center's key table.  
// An “ATKY” command can be used to set the new device’s initial link key.
//
// Transmitted by external processor
typedef struct {
	// Identifies the UART data frame for the host to match with a subsequent Tx Status.  
	// If zero, no response is sent.
	uint8 FrameId;
	// 64 bit device address
	_Addr64 dev64Addr;
	// 16 bit device address.  Set to 0xFFFE	
	_Addr16 dev16Addr;
	// set to zero	
	uint8 keyOptions;
	// The initial Trust Center link key of the device, settable up to 16 bytes.  
	// If less than 16 bytes are used, the upper unspecified bytes of the key will be zero padded.	
	uint8 key[16];
} _ZigBeeRegisterJoiningDevice;

// ZigBee Register Joining Device Status (0x96)
//
// This frame returns the status of a request to register a new device into the trust center's key table
// Received by external processor
typedef struct {
	// Identifies the UART data frame for the host to match with a subsequent Tx Status.  
	// If zero, no response is sent.
	uint8 FrameId;			
	// Indicates the status of the registration request.	
	uint8 status; // see _JOINING_STATUS
} _ZigBeeRegisterJoiningDeviceStatus;

// Transmitted by external processor
typedef struct {
	uint8 FrameId;
	_Addr64 dest64Addr;
	_Addr16 dest16Addr;
	uint8 options;
	uint16 cmd;
	uint8 cmdData[MAX_RF_DATA];
} _ZigBeeRemoteATCommand;

// ZigBee Tx Status (0x8B)
//
// When a ZigBee Tx Request is completed, the modem sends a ZigBee Tx status message.
// This message will indicate if the packet was transmitted successfully or if there was a failure.
// It also shows if any Route Discovery, Network Address Discovery, or Retries took place.
// 
// Received by external processor
// After _ZigBeeTxRequest or _ExplicitAddressingZigBeeCommandFrame is sent
typedef struct {
	// The value indicates which ZigBee TX frame is associated with this Status frame.
	uint8 FrameId;
	// If it was successfully delivered, the Network Address (16-bit) of the destination. 
	// Otherwise, the Destination Network Address which was provided in the Tx ZigBee Request Frame.
	_Addr16 remote16Addr;
	// The number of application transmission retries that took place.
	uint8 TxRetryCount;
	uint8 deliveryStatus;	// See _TX_STATUS
	uint8 discoveryStatus;	// See _DISCOVERY_STATUS
} _ZigBeeTxStatus;

// ZigBee Create Source Route (0x21)
//
// Used to inform an aggregator/concentator of the source route to use for transmitting to a destination node.
//
// Sent by external processor
// Before a _ExplicitAddressingZigBeeCommandFrame is sent
//
typedef struct {
	// The value indicates which ZigBee TX frame is associated with this Status frame.
	uint8 FrameId;
	// Extended (64-bit) Address of the Destination
	_Addr64 dest64Addr;
	// Short(16-bit) Address of the Destination
	_Addr16 dest16Addr;
	// Set to 0x00
	uint8 RxOptions;
	// The number of route records (16-bit addresses) which follow.
	uint8 NumRouteRecords;
	// Each record holds a 16-bit address.  The first record is the 16-bit address
	// of the neighbor ot the device that sent the route record.  The last record
	// is the 16-bit address of the last hop of the route record (neighbor of the recepient).
	uint16 RouteRecords[(MAX_RF_DATA>>1)];
} _ZigBeeCreateSourceRoute;

// Received by external processor
// in response to _ZigBeeTxRequest
typedef struct {
//	uint8 FrameId;
	_Addr64 src64Addr;
	_Addr16 src16Addr;
	uint8 options;
	uint8 RF_Data[MAX_RF_DATA];
} _ZigBeeRxIndicator;

// ZigBee Explicit Rx Indicator (0x91)
//
// When the modem receives a ZigBee RF packet it is sent out the UART 
// using this message type if the EXPLICIT_RECEIVE_OPTION bit is set in AO.  
// Bytes in the RF Data are sent out in the same order as they were received.
//
// Received by external processor
// in response to _ExplicitAddressingZigBeeCommandFrame
typedef struct {
	// Identifies the UART data frame for the host to match with a subsequent Tx Status.  
	// If set to 0, no response is sent.
	//uint8 FrameId;
	// Extended (64-bit) Address of the Source Module
	_Addr64 src64Addr;
	// Network (16-bit) Address of the Source Module
	_Addr16 src16Addr;
	// Endpoint on the source device that initiated the transmission.
	uint8 srcEndpoint;
	// Destination endpoint to which the packet was addressed.
	uint8 destEndpoint;
	// Cluster ID to which the packet was addressed
	uint8 clusterId[2];
	// Profile ID to which the packet was addressed.
	uint8 profileId[2];
	// 0x01 indicates packet was acknowledged.
	// 0x02 Indicates packet was received from a broadcast address.
	uint8 options;
	// Up to 255 bytes, reduced by encryption and source routing overhead.  
	// See NP command description.
	uint8 RF_Data[MAX_RF_DATA];
} _ZigBeeExplicitRxIndicator;

// ZigBee Route Record Indicator (0xA1)
//
// This message is received when a ZigBee route record frame is received.
//
// Received by external processor
typedef struct {
	// Extended (64-bit) address of the source module that sent the record frame.
	_Addr64 src64Addr;
	// Network (16-bit) address fo the source module that sent the route record frame.
	_Addr16 src16Addr;
	// 1 - packet was acknowledged, 2 - packet was received from a broadcast address.
	uint8 options;
	// number of route records (16-bit addresses) which follow.	
	uint8 numRecords;
	// Each record holds a 16-bit address.  
	// The first record is the 16-bit address of the neighbor of the device that sent the route record.
	// The last record is the 16-bit address of the last hop of the route record (neighbor of the recepient).	
	_Addr16 RF_Data[(MAX_RF_DATA)>>2];	
} _ZigBeeRouteRecordIndicator;

// ZigBee Device Authenticated Indicator (0xA2)
//
// This frame is sent out the UART of the Trust Center when a new device is authenticated on a Smart Energy network.
//
// Received by external processor
typedef struct {
	// Extended (64-bit) address of the authenticated device.
	_Addr64 src64Addr;
	// Network (16-bit) address of the authenticated device.	
	_Addr16	src16Addr;
	// 0 - Success.	
	uint8	status;
} _ZigBeeDeviceAuthenticatedIndicator;

// ZigBee Many to One Route Request Indicator (0xA3)
//
// This frame is sent out the UART when a device receives a many-to-one route request.  Only supported on routers
// and coordinator.  End devices will not receive this API frame.
// 
// Received by external processor
typedef struct {
	// Extended (64-bit) address of the device.
	_Addr64 src64Addr;
	// Network (16-bit) address of the device.
	_Addr16 src16Addr;
	uint8 reserved;
} _ZigBeeManyToOneRouteRequestIndicator;

//
// A packet must be capable of receiving or transmitting every kind of API frame
//
typedef struct {
	uint8 delim;
	uint8 lengthMSB;
	uint8 lengthLSB;
	uint8 cmdType;
	union{
		_Bytes	fBytes;
		_ModemStatus    				fModemStatus;
		_ATCommand						fATCommand; 
		_ATCommandQueueRegisterValue	fATCommandQueueRegisterValue;
		_ATCommandResponse				fATCommandResponse;
		_ZigBeeTxRequest				fZigBeeTxRequest;
		_ExplicitAddressingZigBeeCommandFrame	fExplicitAddressingZigBeeCommandFrame;
		_ZigBeeRegisterJoiningDevice	fZigBeeRegisterJoiningDevice;
		_ZigBeeRegisterJoiningDeviceStatus	fZigBeeRegisterJoiningDeviceStatus;
		_ZigBeeRemoteATCommand			fZigBeeRemoteATCommand;
		_ZigBeeTxStatus					fZigBeeTxStatus;
		_ZigBeeRxIndicator				fZigBeeRxIndicator;
		_ZigBeeCreateSourceRoute		fZigBeeCreateSourceRoute;
		_ZigBeeExplicitRxIndicator		fZigBeeExplicitRxIndicator;
		_ZigBeeRouteRecordIndicator		fZigBeeRouteRecordIndicator;
		_ZigBeeDeviceAuthenticatedIndicator	fZigBeeDeviceAuthenticatedIndicator;
		_ZigBeeManyToOneRouteRequestIndicator fZigBeeManyToOneRouteRequestIndicator;
	} data;
	uint8 checksum;		
}_Packet;


// define the smallest and largest valid frames, plus one byte for the cmdType
#define FRAME_XMIT_MIN (1+sizeof(_ModemStatus))
//#define FRAME_XMIT_MAX (1+255+sizeof(_ExplicitAddressingZigBeeCommandFrame))
#define FRAME_XMIT_MAX (1+MAX_RF_DATA+sizeof(_ExplicitAddressingZigBeeCommandFrame))



extern uint8 *	Byte_Xmit_ptr;
extern uint16	Byte_Xmit_length;
extern enum _ByteState Byte_State;

// the implementor supplies a routine to process notice of transmission of a packet
extern void FrameTransmitPacket( _Packet * p );
void SendAT_API_Data_Packet(char* ,char);
extern void Process_API_Packet(char*);
extern void Zigbee_Transmit_Request(char* , unsigned int);

extern unsigned char AddressBuf_64bit[];



