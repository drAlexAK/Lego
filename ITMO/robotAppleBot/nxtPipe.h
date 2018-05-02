
/*
nxtPipe.h - provides a number of frequently used RS485 related functions
that are useful for communication two Lego brick across S4 port
*/

/* message header                  and      body
______ _________________ ______     ____ ___ ___ ___
|0     |1           |2   |3     |   |4  |5  |6  |n   |
| 0x07 | message id | cs | size | + | 1 | 2 | 3 | xx |
|______|____________|____|______|   |___|___|___|____|
|          |       |     |
|          |       |     |___ body size, bytes
|          |       |___ check summ
|          |____ message number
|____ message type, must be 0x7
*/
#define MSG_HEADER_SIZE   4
#define MSG_TYPE					0x7
#define MSG_HEAD_INDEX_TYPE 0
#define MSG_HEAD_INDEX_ID 1
#define MSG_HEAD_INDEX_CS 2
#define MSG_HEAD_INDEX_BODY_SIZE 3
//-------------------------------------
/* replay header
______ _________________ ______
|0     |1           |2   |3     |
| 0x0c | message id |  0 |  0   |
|______|____________|____|______|
|          |       |     |
|          |       |     |___status 0 - error, 1 - recived, 2 - completed
|          |       |___ must be 0, reserved
|          |____ message number
|____ message type, must be 0x0c
*/
#define RPL_TYPE									0x0c
#define RPL_HEAD_INDEX_TYPE 			0
#define RPL_HEAD_INDEX_ID 				1
#define RPL_HEAD_INDEX_RESERVED 	2
#define RPL_HEAD_INDEX_STATUS 		3
//-------------------------------------

typedef char msgHeader[MSG_HEADER_SIZE]; /*!< 4 bytes array for message header */
typedef char msgBody[32]; 							 /*!< 32 bytes array for message body */
//---------------------------------------
typedef enum MSG_STATUS {
	MSG_STATUS_SENT 				= 1,
	MSG_STATUS_DELIVERED		= 2,
	MSG_STATUS_COMPLETED		= 3,
	MSG_STATUS_ERROR 				= 4
} MSG_STATUS;

typedef struct {
	int Size;
	int Attempts;
	char Msg[36];
	MSG_STATUS Status;
} Delivery;

//---------------------------------------
ubyte msgLastID = 0;
/**
* returns check summ
* @param a
* @param size
* @return check summ
*/
ubyte GetCheckSumm(ubyte *a, int size);
/**
* configure S4 port for communiction
*/
void InitialyzePipe();
/**
* send completed replay message
* @id incomming message id
*/
void SendCompleteReplayMsg(ubyte msgId);
/**
* send replay message
* @id incomming message id
* @status incomming message status
*/
void SendReplayMsg(ubyte id, MSG_STATUS status);
/**
* send message safe with multithreading synchronization
* @param msg pointer to message array
* @param size message size, bytes
*/
void SendSafe(char *msg, ubyte size);
task ReadMsg();
//-------------------
Delivery outDelivery;
Delivery inDelivery;
int UnknowMessageCounter = 0; /* contains numbers of incomming messages unknow type */
bool lockSend = false; 				/* multithreading synchronization */
//---------------------------------------
/**
* returns check summ
* @param a
* @param size
* @return check summ
*/
ubyte GetCheckSumm(ubyte *a, int size){
	ubyte sum =0;
	for(int i =0; i < size; i++){
		sum += a[i];
	}
	return sum;
}

bool SendMsg(ubyte *body, int size, bool waitComplete, int attempts, int timeOut){

	Delivery d;
	msgLastID++;
	d.Status = MSG_STATUS_SENT;
	d.Attempts = 1;
	msgHeader header;
	header[MSG_HEAD_INDEX_TYPE]      = MSG_TYPE;
	header[MSG_HEAD_INDEX_ID]        = msgLastID;
	header[MSG_HEAD_INDEX_CS]        = GetCheckSumm(body, size);
	header[MSG_HEAD_INDEX_BODY_SIZE] = size;
	d.Size = size + MSG_HEADER_SIZE;

	memcpy(d.Msg, header, sizeof(ubyte) * MSG_HEADER_SIZE);
	memcpy(&d.Msg[4], body, sizeof(ubyte) * size);

	if (timeOut !=0) clearTimer(T1); // start timer

	SendSafe(d.Msg, d.Size);
	memcpy(outDelivery, d, sizeof(d) );

	while (((outDelivery.Status != MSG_STATUS_COMPLETED) && (waitComplete)) ||
		((outDelivery.Status != MSG_STATUS_DELIVERED) && (waitComplete == false)))
	{
		if ((timeOut !=0) && (time1[T1] >= timeOut)) return false; // timeout is exceeded
			if (outDelivery.Status == MSG_STATUS_ERROR) {
			if ((attempts != 0) && (outDelivery.Attempts >= attempts)) return false; // attempts are exceeded
				outDelivery.Attempts++;
			outDelivery.Status = MSG_STATUS_SENT;
			SendSafe(d.Msg, d.Size);
		}
		sleep(100);
	}
	return true;
}
/**
* send message safe with multithreading synchronization
* @param msg pointer to message array
* @param size message size, bytes
*/
void SendSafe(char *msg, ubyte size){

	while (lockSend) { sleep (10); }
	lockSend = true;
	nxtWriteRawHS(msg, size, 0);
	lockSend = false;

}

task ReadMsg(){
	int size = 0;
	ubyte cs = 0;
	Delivery d;

	ubyte header[MSG_HEADER_SIZE];
	while(true){
		while ( nxtGetAvailHSBytes() < MSG_HEADER_SIZE)
			sleep(100); /* waiting messages */
		nxtReadRawHS(header, MSG_HEADER_SIZE);
		if(header[MSG_HEAD_INDEX_TYPE] == RPL_TYPE){ /* analyze reply message */
			if (outDelivery.Msg[MSG_HEAD_INDEX_ID] == header[RPL_HEAD_INDEX_ID])
				outDelivery.Status = (MSG_STATUS) header[RPL_HEAD_INDEX_STATUS]; /* sets outgoing message status */
		}
		else if (header[MSG_HEAD_INDEX_TYPE] == MSG_TYPE){ /* analyze incomming message */
			memcpy(d.Msg, header, sizeof(ubyte) * MSG_HEADER_SIZE);
			cs = 0; /* set 0 check summ for body less messages */
			d.Attempts = 0;
			d.Status = MSG_STATUS_DELIVERED;
			d.Size = MSG_HEADER_SIZE + header[MSG_HEAD_INDEX_BODY_SIZE];
			if (header[MSG_HEAD_INDEX_BODY_SIZE] > 0) {
				while (nxtGetAvailHSBytes() < (byte) header[MSG_HEAD_INDEX_BODY_SIZE])
					sleep(100); /* waiting messages */
				nxtReadRawHS(d.Msg[MSG_HEADER_SIZE], header[MSG_HEAD_INDEX_BODY_SIZE]);
				cs = GetCheckSumm(d.Msg[MSG_HEADER_SIZE], header[MSG_HEAD_INDEX_BODY_SIZE]);
			}
			memcpy(inDelivery , d, sizeof(d) );
			if (cs != header[MSG_HEAD_INDEX_CS])
				SendReplayMsg(header[MSG_HEAD_INDEX_ID], MSG_STATUS_ERROR);
			else
				SendReplayMsg(header[MSG_HEAD_INDEX_ID], MSG_STATUS_DELIVERED);
			} else {
			UnknowMessageCounter++; /* increase unknow incomming messages */
		}
	}
}
/**
* send completed replay message
* @id incomming message id
*/
void SendCompleteReplayMsg(ubyte msgId)
{
	SendReplayMsg(msgId, MSG_STATUS_COMPLETED);
}

/**
* send replay message
* @id incomming message id
* @status incomming message status
*/
void SendReplayMsg(ubyte id, MSG_STATUS status){
	msgHeader h;
	h[RPL_HEAD_INDEX_TYPE]			= RPL_TYPE;
	h[RPL_HEAD_INDEX_ID]				= id;
	h[RPL_HEAD_INDEX_RESERVED]	= 0;
	h[RPL_HEAD_INDEX_STATUS]		= status;
	SendSafe(&h[0], MSG_HEADER_SIZE);
}

/**
* configure S4 port for communiction
*/
void InitialyzePipe(){
	nxtEnableHSPort();			/* configure S4 as a high-speed port */
	nxtHS_Mode = hsRawMode; /* Set port mode. This can be one of hsRawMode, hsMsgModeMaster, hsMsgModeSlave.
	RS485 is a half duplex protocol,
	that means that only one BXT can say something at any given time. */
	nxtSetHSBaudRate(9600);	/* configure S4 as a high-speed port and select a BAUD rate */
	sleep(100);
	startTask(ReadMsg);
}

/*
task main()
{
char a[5] = {'h', 'e', 'l', 'l', 'o'};
Initialyze();
bool res  = SendMsg(a, 5, true, 0, 5000);
while (true) sleep(100);
}
*/
