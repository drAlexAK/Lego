//--------------------------------------------
// wheel
#define WHEEL_ENCODER_MAX                145
#define WHEEL_SPEED						            60
#define WHEEL_DIST_MAX_ERROR            1200
#define WHEEL_INACCURACY_ENCODER           2
//dist
#define INIT_FRONT_DIST_MIN		           100
#define INIT_FRONT_DIST_MAX		           800
#define INIT_SIDE_DIST_MIN		           300
#define INIT_SIDE_DIST_MAX		          1500
// motor
#define MOTOR_SPEED_MAX									 100
#define MOTOR_SPEED_MIN									   0
#define MOTOR_SPEED_CHECK_INTERVAL				 5
#define MOTOR_SPEED_UP_STEP								 5
#define MOTOR_SPEED_UP_SLEEP							 5
#define MOTOR_SPEED_DOWN_STEP							-5
#define MOTOR_SPEED_DOWN_SLEEP						 5

//--------------------------------------------
// tasks
task wheelControl();
task getDist();
task motorControl();
//--------------------------------------------
// functions
bool selfTest();
int normalizeSide(int dist);
int normalizeFront(int dist);
int getWheelEncoderByDistErr(int err);
bool isWheelCurrentEncoderOk(int currentEncoder, int targetEncoder);
//--------------------------------------------
